#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>
#include<vector>
#include<string>
#include<fbxsdk.h>
#include<unordered_map>

// �X�P���g��
// �����̃{�[�����Ǘ�
struct skeleton
{
	// ���b�V���̃{�[�����
	struct bone
	{
		uint64_t unique_id{ 0 };	// ����ID
		std::string name;
		// �e�{�[���̈ʒu���Q�Ƃ���C���f�b�N�X	-1...�e����
		int64_t parent_index{ -1 };
		// �V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
		int64_t node_index{ 0 };

		// ���f��(���b�V��)��Ԃ���{�[��(�m�[�h)�ɕϊ����邽�߂Ɏg�p
		DirectX::XMFLOAT4X4 offset_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		// true...�e�����̃{�[��
		bool IsOrphans() const { return parent_index < 0; };
	};
	std::vector<bone> bones;

	int64_t IndexOf(uint64_t unique_id)const
	{
		int64_t index{ 0 };
		for (const bone& bone_ : bones)
		{
			if (bone_.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
};

// �A�j���[�V����
struct animation
{
	// �A�j���[�V�����̖��O
	std::string name;
	// �T���v�����O���[�g
	float sampling_rate{ 0 };

	// �L�[�t���[��
	struct keyframe
	{
		struct node
		{
			// �L�[�t���[���Ɋ܂܂��m�[�h�̍s��
			DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		
			DirectX::XMFLOAT3 scaling{ 1,1,1 };
			DirectX::XMFLOAT4 rotation{ 0,0,0,1 };
			DirectX::XMFLOAT3 translation{ 0,0,0 };
		};
		// �L�[�t���[���Ɋ܂܂��S�Ẵm�[�h�̍s��
		std::vector<node> nodes;
	};
	// �A�j���[�V�����P���̃f�[�^
	std::vector<keyframe> sequence;
};

struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };	// ����ID
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };
	};
	std::vector<node> nodes;
	int64_t IndexOf(uint64_t unique_id)const
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
	// �ő�{�[���e���l
	static const int MAX_BONE_INFLUENCES{ 4 };

	struct vertex
	{
		DirectX::XMFLOAT3 position;			// ���_���W
		DirectX::XMFLOAT3 normal{ 0,1,0 };	// �@��
		DirectX::XMFLOAT2 texcoord{ 0,0 };	// �e�N�X�`�����W
		float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };	// �E�F�C�g�l
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};		// �{�[���ԍ�
	};

	static const int MAX_BONES{ 256 };
	// �萔�o�b�t�@�t�H�[�}�b�g
	struct constants
	{
		DirectX::XMFLOAT4X4 world;			// ���[���h�s��
		DirectX::XMFLOAT4 material_color;	// �}�e���A���J���[
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
	};

	//���b�V�����
	struct mesh
	{
		uint64_t unique_id{ 0 };		// ����ID
		std::string name;				// ���b�V����
		int64_t node_index{ 0 };		// �m�[�hID
		std::vector<vertex> vertices;	// ���_���W
		std::vector<uint32_t> indices;	// ���_�C���f�b�N�X

		// �T�u�Z�b�g���
		struct subset
		{
			uint64_t material_unique_id{ 0 };	// ����ID
			std::string material_name;			// �}�e���A����

			uint32_t start_index_location{ 0 };	// �C���f�b�N�X�̊J�n�ʒu
			uint32_t index_count{ 0 };			// �C���f�b�N�X�̐�(���_)
		};
		std::vector<subset> subsets;

		// ���b�V�����Ƃ̃��[���h�s��
		DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	
		// �o�C���h�|�[�Y(�����p��)
		skeleton bind_pose;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;	// ���_�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;	// �C���f�b�N�X�o�b�t�@
		friend class Model;
	};
	std::vector<mesh> meshes;

	//�}�e���A��
	struct material
	{
		uint64_t unique_id{ 0 };	// ����ID
		std::string name;	// �}�e���A����

		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		// �e�N�X�`���t�@�C����
		std::string texture_filename[4];
		// �e�N�X�`�����
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
	};
	// �ǂݍ��񂾃}�e���A��
	std::unordered_map<uint64_t, material> materials;

	// �S�ẴA�j���[�V�����̃f�[�^
	std::vector<animation> animation_clips;
public:
	// "triangulate">true...���p�`�ō��ꂽ�|���S�����O�p�`��
	Model(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
	virtual ~Model() = default;

	// �`�揈��
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& material_color, const animation::keyframe* keyframe);

	//	�A�j���[�V�����X�V
	void UpdateAnimation(animation::keyframe& keyframe);

	// �A�j���[�V�����̒ǉ�
	bool AppendAnimations(const char* animation_filename, float sampling_rate);

	// �A�j���[�V�����̘A��
	void BlendAnimations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);

	// ���b�V�����̎��o��
	void FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);

	// �}�e���A�����̎��o��
	void FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

	// �o�C���h�|�[�Y���̎��o��
	void FetchSkeletons(FbxMesh* fbx_mesh, skeleton& bind_pose);

	// �A�j���[�V�������̎��o��
	void FetchAnimations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
		float sampling_rate);

	// �o�b�t�@�̐���
	void CreateComObjects(ID3D11Device* devvice, const char* fbx_filename);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;		// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;		// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;		// �萔�o�b�t�@
protected:
	// ����fbx�̐e�V�[��
	scene scene_view;
};