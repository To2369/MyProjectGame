#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"Shader.h"
class CapsuleOneWay
{
public:
    CapsuleOneWay(ID3D11Device* device,
        float height,
        float radius,
        uint32_t slices,
        uint32_t ellipsoid_stacks,
        uint32_t stacks);
    ~CapsuleOneWay(){};

    void RegenerateGeometry();
    void Update(float elapsedTime);
    void Render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& material_color);
    void SetGrowthRate(const float& rate) { this->growthRate = rate; }

    const float GetCurrentHeight()const { return currentHeight; }
    // ���_�t�H�[�}�b�g
    struct vertex
    {
        DirectX::XMFLOAT3 position; // �ʒu
        DirectX::XMFLOAT3 normal;   // �@��
    };
    // �萔�o�b�t�@�p�t�H�[�}�b�g
    struct constants
    {
        DirectX::XMFLOAT4X4 world;          // ���[���h�s��
        DirectX::XMFLOAT4 material_color;   // �}�e���A���J���[
    };

    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);
private:
    float currentHeight = 0;        // ���݂̍���
    float growthRate = 0;           // �����̑������x
    float radius = 0;               // �J�v�Z���̔��a
    uint32_t slices = {};            // ������
    uint32_t stacks = {};            // �~���̕�����
    ID3D11Device* device = {};       // �f�o�C�X�Q�Ƃ�ێ�

    // ���_�V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;

    // ���̓��C�A�E�g�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    // �萔�o�b�t�@�̃I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    // �萔�o�b�t�@
    D3D11_BUFFER_DESC buffer_desc{};

    // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    // �C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    D3D11_SUBRESOURCE_DATA subresource_data{};
};
