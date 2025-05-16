#include "Shader.h"
#include"../Misc.h"
#include <sstream>
#include<WICTextureLoader.h>
#include<wrl.h>
#include<filesystem>
#include<DDSTextureLoader.h>
#include<string>
#include<map>

// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT ShaderManager::CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
    ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{    
    // �t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long csoSz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    // �t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>csoData{ std::make_unique<unsigned char[]>(csoSz) };
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    if (inputLayout)
    {
        hr = device->CreateInputLayout(
            inputElementDesc,                 // ���̓��C�A�E�g�̐ݒ���s�����߂̍\���� 
            numElements,                       // �\���̂̔z��̐�
            csoData.get(),                     // �R���p�C�����ꂽ���_�V�F�[�_�[������
            csoSz,                             // �R���p�C�����ꂽ���_�V�F�[�_�[�̃T�C�Y
            inputLayout);                      // �������̓��̓��C�A�E�g�I�u�W�F�N�g
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT ShaderManager::CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
    // �t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "SCO File not found");

    // �t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long csoSz{ ftell(fp) };// �R���p�C�����ꂽ �s�N�Z���V�F�[�_�̃T�C�Y
    fseek(fp, 0, SEEK_SET);

    // �t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>csoData{ std::make_unique<unsigned char[]>(csoSz) };
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };

    hr = device->CreatePixelShader(
        csoData.get(),         // �R���p�C�����ꂽ�s�N�Z���V�F�[�_�[������
        csoSz,                 // �R���p�C�����ꂽ�s�N�Z���V�F�[�_�[�̃T�C�Y
        nullptr,                // �N���X �����P�[�W �C���^�[�t�F�C�X�ւ̃|�C���^�[,�l��NULL�ɂ��邱�Ɖ\
        pixelShader);          // �������̃s�N�Z���V�F�[�_�[
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}

// �������ꂽ�V�F�[�_���\�[�X�r���[
// ���������炱���ɕۑ����Ă����t�@�C�����ŕۑ�
// �Q��ڈȍ~�͂�������擾����
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

// �O���t�@�C������e�N�X�`����ǂݍ���
HRESULT ShaderManager::LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView,
    D3D11_TEXTURE2D_DESC* texture2dDesc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    // ���Ƀ��[�h���ꂽ�e�N�X�`��������
    auto it = resources.find(filename);
    // ���Ƀ��[�h����Ă���΁A������g�p���A�����łȂ���ΐV�������[�h
    if (it != resources.end()) 
    {
        *shaderResourceView = it->second.Get();
        (*shaderResourceView)->AddRef();
        (*shaderResourceView)->GetResource(resource.GetAddressOf());
    }
    // �V�����e�N�X�`�������[�h����
    else
    {
        // DDS�t�@�C�������݂��邩�ǂ����`�F�b�N
        std::filesystem::path ddsFileneme(filename);
        ddsFileneme.replace_extension("dds");
        // DDS�t�@�C�������݂�����DDS�e�N�X�`���t�@�C���ǂݍ���
        if (std::filesystem::exists(ddsFileneme.c_str()))
        {
            hr = DirectX::CreateDDSTextureFromFile(device, ddsFileneme.c_str(), resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        // DDS �t�@�C�������݂��Ȃ���Βʏ�̃e�N�X�`���ǂݍ���
        else
        {
            // �t�@�C������e�N�X�`�����쐬���Ashader_resource_view�ɐݒ�
            hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        resources.insert(std::make_pair(filename, *shaderResourceView));
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    // ���\�[�X����e�N�X�`���𐶐�
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    // �e�N�X�`������e�N�X�`���������o��
    texture2d->GetDesc(texture2dDesc);

    return hr;
}

HRESULT ShaderManager::LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    hr = DirectX::CreateDDSTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
        size, resource.GetAddressOf(), shaderResourceView);
    if (hr != S_OK)
    {
        hr = DirectX::CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
            size, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

// �S�Ẵe�N�X�`�������
void ShaderManager::ReleaseAllTextures()
{
    resources.clear();
}

HRESULT ShaderManager::MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value,
    UINT dimension)
{
    HRESULT hr{ S_OK };

    // �_�~�[�̃e�N�X�`�����
    D3D11_TEXTURE2D_DESC texture2dDesc{};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // �_�~�[�̃e�N�Z������ݒ�
    size_t texsels = dimension * dimension;
    std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texsels) };
    for (size_t i = 0; i < texsels; i++)
    {
        sysmem[i] = value;
    }

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    // �_�~�[�̏��Ńe�N�X�`�����쐬
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �_�~�[�̃e�N�X�`�����ŃV�F�[�_�[���\�[�X�r���[���쐬
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(
        texture2d.Get(),
        &shaderResourceViewDesc,
        shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}