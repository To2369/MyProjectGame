#pragma once
#include<DirectXMath.h>
#include<d3d11.h>
#include<wrl.h>
#include"Shader.h"
// �􉽃v���~�e�B�u
// �􉽃v���~�e�B�u�Ƃ�...�����́A���A�~���Ȃǂ̌`��̐}�`
class GeometricPrimitive
{
public:
    // �R���X�g���N�^��protected��
    virtual ~GeometricPrimitive() = default;

    void Render(ID3D11DeviceContext* dc,
        const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& materialColor);

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
        DirectX::XMFLOAT4 materialColor;   // �}�e���A���J���[
    };
protected:
    GeometricPrimitive(ID3D11Device* device);

    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertexCount,
        uint32_t* indices, size_t indexCount);

    // ���_�V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    // ���̓��C�A�E�g�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    // �萔�o�b�t�@�̃I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    // �萔�o�b�t�@
    D3D11_BUFFER_DESC bufferDesc{};
private:
    // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    // �C���f�b�N�X�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    D3D11_SUBRESOURCE_DATA subresourceData{};
};

// �����`
class GeometricCube :public GeometricPrimitive
{
public:
    GeometricCube(ID3D11Device* device);
};

// �~��
class GeometricCylinder :public GeometricPrimitive
{
public:
    // ��Q�����̓V�����_�[�̉~�����������邩�̐����w��
    GeometricCylinder(ID3D11Device* device,float radius,float height, uint32_t slices);
};

// ��
class GeometricSphere :public GeometricPrimitive
{
public:
    GeometricSphere(ID3D11Device* device,
        float radius,
        uint32_t slices,
        uint32_t stacks);
};

// �J�v�Z��
class GeometricCapsule :public GeometricPrimitive
{
public:
    GeometricCapsule(ID3D11Device* device,
        float height,
        float radius,
        uint32_t slices,
        uint32_t ellipsoidStacks,
        uint32_t stacks);
};