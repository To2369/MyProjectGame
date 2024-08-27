#include "Shader.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
#include<wrl.h>
#include<filesystem>
#include<DDSTextureLoader.h>
#include<string>
#include<map>

// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT ShaderManager::CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{    
    // �t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    // �t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    if (input_layout)
    {
        hr = device->CreateInputLayout(
            input_element_desc,                 // ���̓��C�A�E�g�̐ݒ���s�����߂̍\���� 
            num_elements,                       // �\���̂̔z��̐�
            cso_data.get(),                     // �R���p�C�����ꂽ���_�V�F�[�_�[������
            cso_sz,                             // �R���p�C�����ꂽ���_�V�F�[�_�[�̃T�C�Y
            input_layout);                      // �������̓��̓��C�A�E�g�I�u�W�F�N�g
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
HRESULT ShaderManager::CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
    // �t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "SCO File not found");

    // �t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };// �R���p�C�����ꂽ �s�N�Z���V�F�[�_�̃T�C�Y
    fseek(fp, 0, SEEK_SET);

    // �t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };

    hr = device->CreatePixelShader(
        cso_data.get(),         // �R���p�C�����ꂽ�s�N�Z���V�F�[�_�[������
        cso_sz,                 // �R���p�C�����ꂽ�s�N�Z���V�F�[�_�[�̃T�C�Y
        nullptr,                // �N���X �����P�[�W �C���^�[�t�F�C�X�ւ̃|�C���^�[,�l��NULL�ɂ��邱�Ɖ\
        pixel_shader);          // �������̃s�N�Z���V�F�[�_�[
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}

// �������ꂽ�V�F�[�_���\�[�X�r���[
// ���������炱���ɕۑ����Ă����t�@�C�����ŕۑ�
// �Q��ڈȍ~�͂�������擾����
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

// �O���t�@�C������e�N�X�`����ǂݍ���
HRESULT ShaderManager::LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view,
    D3D11_TEXTURE2D_DESC* texture2d_desc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    // ���Ƀ��[�h���ꂽ�e�N�X�`��������
    auto it = resources.find(filename);
    // ���Ƀ��[�h����Ă���΁A������g�p���A�����łȂ���ΐV�������[�h
    if (it != resources.end()) 
    {
        *shader_resource_view = it->second.Get();
        (*shader_resource_view)->AddRef();
        (*shader_resource_view)->GetResource(resource.GetAddressOf());
    }
    // �V�����e�N�X�`�������[�h����
    else
    {
        // DDS�t�@�C�������݂��邩�ǂ����`�F�b�N
        std::filesystem::path dds_fileneme(filename);
        dds_fileneme.replace_extension("dds");
        // DDS�t�@�C�������݂�����DDS�e�N�X�`���t�@�C���ǂݍ���
        if (std::filesystem::exists(dds_fileneme.c_str()))
        {
            hr = DirectX::CreateDDSTextureFromFile(device, dds_fileneme.c_str(), resource.GetAddressOf(), shader_resource_view);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        // DDS �t�@�C�������݂��Ȃ���Βʏ�̃e�N�X�`���ǂݍ���
        else
        {
            // �t�@�C������e�N�X�`�����쐬���Ashader_resource_view�ɐݒ�
            hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        resources.insert(std::make_pair(filename, *shader_resource_view));
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    // ���\�[�X����e�N�X�`���𐶐�
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    // �e�N�X�`������e�N�X�`���������o��
    texture2d->GetDesc(texture2d_desc);

    return hr;
}

// �S�Ẵe�N�X�`�������
void ShaderManager::release_all_textures()
{
    resources.clear();
}

HRESULT ShaderManager::MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value,
    UINT dimension)
{
    HRESULT hr{ S_OK };

    // �_�~�[�̃e�N�X�`�����
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = dimension;
    texture2d_desc.Height = dimension;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // �_�~�[�̃e�N�Z������ݒ�
    size_t texsels = dimension * dimension;
    std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texsels) };
    for (size_t i = 0; i < texsels; i++)
    {
        sysmem[i] = value;
    }

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = sysmem.get();
    subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

    // �_�~�[�̏��Ńe�N�X�`�����쐬
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �_�~�[�̃e�N�X�`�����ŃV�F�[�_�[���\�[�X�r���[���쐬
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(
        texture2d.Get(),
        &shader_resource_view_desc,
        shader_resource_view);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}