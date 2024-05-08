#include "Shader.h"
#include"../misc.h"
#include <sstream>

HRESULT ShaderManager::CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    
    //�t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    //�t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    //�t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateInputLayout(
        input_element_desc,                 //���̓��C�A�E�g�̐ݒ���s�����߂̍\���� 
        num_elements,                       //�\���̂̔z��̐�
        cso_data.get(),                     //�R���p�C�����ꂽ���_�V�F�[�_�[������
        cso_sz,                             //�R���p�C�����ꂽ���_�V�F�[�_�[�̃T�C�Y
        input_layout);                     //�������̓��̓��C�A�E�g�I�u�W�F�N�g
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

HRESULT ShaderManager::CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    //�t�@�C�����J��
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "SCO File not found");

    //�t�@�C���̃T�C�Y�𒲂ׂ�
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };//�R���p�C�����ꂽ �s�N�Z���V�F�[�_�̃T�C�Y
    fseek(fp, 0, SEEK_SET);

    //�t�@�C�����̕����� cso_data �ɕ�����Ƃ��Ď��o���A�t�@�C�������
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };

    hr = device->CreatePixelShader(
        cso_data.get(),         //�R���p�C�����ꂽ�s�N�Z���V�F�[�_�[������
        cso_sz,                 //�R���p�C�����ꂽ�s�N�Z���V�F�[�_�[�̃T�C�Y
        nullptr,                //�N���X �����P�[�W �C���^�[�t�F�C�X�ւ̃|�C���^�[,�l��NULL�ɂ��邱�Ɖ\
        pixel_shader);         //�������̃s�N�Z���V�F�[�_�[
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}