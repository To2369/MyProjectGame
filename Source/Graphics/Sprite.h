#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include"Shader.h"
#include<string>
class Sprite
{
public:
    //���_�t�H�[�}�b�g
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    Sprite(ID3D11Device* device, const wchar_t* filename);
    virtual ~Sprite();

    //�`��
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //�Z�`�̍���̍��W(�X�N���[�����W�n)
        float dw, float dh,     //�Z�`�̃T�C�Y(�X�N���[�����W�n)
        float r, float g,float b, float a,       //�F�Ɠ����x
        float angle             //�p�x(degree)
    );

    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //�Z�`�̍���̍��W(�X�N���[�����W�n)
        float dw, float dh,     //�Z�`�̃T�C�Y(�X�N���[�����W�n)
        float r, float g, float b, float a,       //�F�Ɠ����x
        float angle,             //�p�x(degree)
        float sx, float sy, float sw, float sh  //��,��,��,����
    );

    //��ʂɕ������o��
    void Textout(ID3D11DeviceContext* immediate_context,
        std::string s,
        float x, float y, float w, float h,
        float r, float g, float b, float a);
private:
    //���_�V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    //�s�N�Z���V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    //���̓��C�A�E�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    //���_�f�[�^
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    //�V�F�[�_�[���\�[�X�r���[
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    //�e�N�X�`��
    D3D11_TEXTURE2D_DESC texture2d_desc;

    ShaderManager* shaderMgr;
};