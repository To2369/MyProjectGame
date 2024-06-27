#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl.h>
#include<vector>
#include"Shader.h"

class SpriteBatch
{
public:
    //���_�t�H�[�}�b�g
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    SpriteBatch(ID3D11Device* device, const wchar_t* filename,size_t max_sprites);
    ~SpriteBatch();

    //�V�F�[�_�[�ƃe�N�X�`���̐ݒ�
    void Begin(ID3D11DeviceContext* immediate_context,
        ID3D11PixelShader* replaced_pixsel_shader = nullptr,
        ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);

    //�ύX�������_�f�[�^�̕ҏW�ƈꊇ�`��
    void End(ID3D11DeviceContext* immediate_context);

    //�`��
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //�Z�`�̍���̍��W(�X�N���[�����W�n)
        float dw, float dh     //�Z�`�̃T�C�Y(�X�N���[�����W�n)
    );

    //�`��
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //�Z�`�̍���̍��W(�X�N���[�����W�n)
        float dw, float dh,     //�Z�`�̃T�C�Y(�X�N���[�����W�n)
        float r, float g, float b, float a,       //�F�Ɠ����x
        float angle             //�p�x(degree)
    );

    //�`��
    void Render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,     //�Z�`�̍���̍��W(�X�N���[�����W�n)
        float dw, float dh,     //�Z�`�̃T�C�Y(�X�N���[�����W�n)
        float r, float g, float b, float a,       //�F�Ɠ����x
        float angle,             //�p�x(degree)
        float sx, float sy, float sw, float sh  //��,��,��,����
    );

    ID3D11PixelShader* GetReplaced_pixel_shader() const
    {
        return replaced_pixel_shader.Get();
    }
    ID3D11ShaderResourceView* GetReplaced_Shader_resource_view() const
    {
        return replaced_shader_resource_view.Get();
    }
private:
    static void rotate(float& x, float& y, float cx, float cy, float sin, float cos)
    {
        x -= cx;
        y -= cy;

        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    }
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

    //�e�N�X�`��
    D3D11_TEXTURE2D_DESC replaced_texture2d_desc;
    //�����ւ��p�s�N�Z���V�F�[�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;
    // �����ւ��p�̃e�N�X�`��
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;
private:
    const size_t max_vertices;
    std::vector<vertex> vertices;
};