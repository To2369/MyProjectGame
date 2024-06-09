#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

//�X�^�e�B�b�N���b�V��
//�X�^�e�B�b�N���b�V���Ƃ̓X�P���g���������Ă��Ȃ��ό`������悤�ȃA�j���[�V�������ł��Ȃ����b�V���̎��ł��B
//�Ⴆ�Έ֎q���A�ǂƂ��������b�V���̓X�^�e�B�b�N���b�V���ɂ�����܂���
//���b�V���Ƃ̓|���S���̏W�܂�̂��ƁB�C���[�W�Ƃ��Ă͒��g�̂Ȃ��\�ʂ����̃f�[�^
class StaticMesh
{
public:
	StaticMesh(ID3D11Device* device, const wchar_t* obj_filename);
	virtual ~StaticMesh() = default;

	//�`�揈��
	void Render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

public:
	//���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;	//���W
		DirectX::XMFLOAT3 normal;	//�@��
	};

	//�萔�o�b�t�@�p�t�H�[�}�b�g
	struct constants
	{
		DirectX::XMFLOAT4X4 world;	//���[���h�s��
		DirectX::XMFLOAT4 material_color;	//�}�e���A���J���[
	};
protected:
	//���_�o�b�t�@�I�u�W�F�N�g�̍쐬
	void Create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);
private:
	//���_�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//�C���f�b�N�X�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	//���_�V�F�[�_�[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//���̓��C�A�E�g�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	//�萔�o�b�t�@�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	//�萔�o�b�t�@
	D3D11_BUFFER_DESC buffer_desc{};

	D3D11_SUBRESOURCE_DATA subresource_data{};
};