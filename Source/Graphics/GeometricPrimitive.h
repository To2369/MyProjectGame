#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"Shader.h"
//�􉽃v���~�e�B�u
//�􉽃v���~�e�B�u�Ƃ�...�����́A���A�~���Ȃǂ̌`��̐}�`
class GeometricPrimitive
{
public:
    GeometricPrimitive(ID3D11Device* device);
    virtual ~GeometricPrimitive() = default;

    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& material_color);

    //���_�t�H�[�}�b�g
    struct vertex
    {
        DirectX::XMFLOAT3 position; //�ʒu
        DirectX::XMFLOAT3 normal;   //�@��
    };
    //�萔�o�b�t�@�p�t�H�[�}�b�g
    struct constants
    {
        DirectX::XMFLOAT4X4 world;          //���[���h�s��
        DirectX::XMFLOAT4 material_color;   //�}�e���A���J���[
    };
protected:
    //���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    void Create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);

private:
    //���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    //�C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    //���_�V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    //���̓��C�A�E�g�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    //�萔�o�b�t�@�̃I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    //�萔�o�b�t�@
    D3D11_BUFFER_DESC buffer_desc{};

    D3D11_SUBRESOURCE_DATA subresource_data{};

    ShaderManager* shaderMgr;
};