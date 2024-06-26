#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>
#include<vector>
#include<string>
#include<fbxsdk.h>

struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };
	};
	std::vector<node> nodes;
	int64_t indexof(uint64_t unique_id)const
	{
		int64_t index{ 0 };
		for (const node& node_ : nodes)
		{
			if (node_.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
};

class Model
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;			//���_���W
		DirectX::XMFLOAT3 normal{ 0,1,0 };	//�@��
		DirectX::XMFLOAT2 texcoord{ 0,0 };	//�e�N�X�`�����W
	};

	//�萔�o�b�t�@�t�H�[�}�b�g
	struct constants
	{
		DirectX::XMFLOAT4X4 world;			//���[���h�s��
		DirectX::XMFLOAT4 material_color;	//�}�e���A���J���[
	};
public:
	//"triangulate">true...���p�`�ō��ꂽ�|���S�����O�p�`��
	Model(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
	virtual ~Model() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	//���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixsl_shader;		//�s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;		//���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;		//�萔�o�b�t�@
protected:
	//����fbx�̐e�V�[��
	scene scene_view;
};