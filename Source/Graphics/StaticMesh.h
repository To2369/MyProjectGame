#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include<string>
#include<vector>
enum class STATICMESH_STATE
{
	NONE,
	BUMP,

	Enum_Max,
};

enum class PIXEL_SHADER_STATE
{
	DEFAULT,
	GEOMETRICPRIMITEVE,

	Enum_Max
};
// �X�^�e�B�b�N���b�V��
// �X�^�e�B�b�N���b�V���Ƃ̓X�P���g���������Ă��Ȃ��ό`������悤�ȃA�j���[�V�������ł��Ȃ����b�V���̎��ł��B
class StaticMesh
{
public:
	// DCC�c�[���ɂ���ď㉺�����⌴�_���Ⴄ�ꍇ������̂Ńe�N�X�`�������]���Ă���ꍇ
	// flippingVcoodinates��true�ɂ���
	StaticMesh(ID3D11Device* device, const wchar_t* objFilename, bool flippingVcoordinates);
	virtual ~StaticMesh() = default;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
		PIXEL_SHADER_STATE state);

public:
	// ���_�t�H�[�}�b�g
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	// ���W
		DirectX::XMFLOAT3 normal;	// �@��
		DirectX::XMFLOAT2 texcoord;	// �e�N�X�`�����W
	};

	// �萔�o�b�t�@�p�t�H�[�}�b�g
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;	// ���[���h�s��
		DirectX::XMFLOAT4 Ka;
		DirectX::XMFLOAT4 Kd;
		DirectX::XMFLOAT4 Ks;
		DirectX::XMFLOAT4 materialColor;
	};

	// �}�e���A������DrawIndex()�ɕK�v�ȏ�񕔕����T�u�Z�b�g��
	struct Subset
	{
		std::wstring usemtl;		// �}�e���A����
		uint32_t indexStart{ 0 };	// �C���f�b�N�X�̊J�n�ʒu
		uint32_t indexCount{ 0 };	// �C���f�N�X�̐��i���_���j
	};
	std::vector<Subset> subsets;

	// �}�e���A��
	struct Material
	{
		// �}�e���A����
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };
		// �e�N�X�`���t�@�C����
		std::wstring textureFilenames[static_cast<int>(STATICMESH_STATE::Enum_Max)];
		// �e�N�X�`�����
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[static_cast<int>(STATICMESH_STATE::Enum_Max)];
	};
	// �ǂݍ��񂾃}�e���A��
	std::vector<Material> materials;

	DirectX::XMFLOAT3 boundingBox[2]{
		{D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX},
		{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
	};
protected:
	// ���_�o�b�t�@�I�u�W�F�N�g�̍쐬
	void CreateComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
		uint32_t* indices, size_t indexCount);
private:
	// ���_�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// ���_�V�F�[�_�[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::Enum_Max)];
	// ���̓��C�A�E�g�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	// �萔�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	// �萔�o�b�t�@
	D3D11_BUFFER_DESC bufferDesc{};

	D3D11_SUBRESOURCE_DATA subresourceData{};
};