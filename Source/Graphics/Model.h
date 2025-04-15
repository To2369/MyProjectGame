#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>
#include<vector>
#include<string>
#include<fbxsdk.h>
#include<unordered_map>
#include<optional>
#include<stdlib.h>
#include <cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>
#include<cereal/types/set.hpp>
#include<cereal/types/unordered_map.hpp>

namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y));
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z));
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w));
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

// �X�P���g��
// �����̃{�[�����Ǘ�
struct Skeleton
{
	// ���b�V���̃{�[�����
	struct Bone
	{
		uint64_t uniqueID{ 0 };	// ����ID
		std::string name;
		// �e�{�[���̈ʒu���Q�Ƃ���C���f�b�N�X	-1...�e����
		int64_t parentIndex{ -1 };
		// �V�[���̃m�[�h�z����Q�Ƃ���C���f�b�N�X
		int64_t nodeIndex{ 0 };

		// ���f��(���b�V��)��Ԃ���{�[��(�m�[�h)�ɕϊ����邽�߂Ɏg�p
		DirectX::XMFLOAT4X4 offsetTransform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		// true...�e�����̃{�[��
		bool IsOrphans() const { return parentIndex < 0; };

		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID, name, parentIndex, nodeIndex, offsetTransform);
		}
	};
	std::vector<Bone> bones;

	int64_t IndexOf(uint64_t uniqueID)const
	{
		int64_t index{ 0 };
		for (const Bone& bone : bones)
		{
			if (bone.uniqueID == uniqueID)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive(bones);
	}
};

// �A�j���[�V����
struct Animation
{
	// �A�j���[�V�����̖��O
	std::string name;
	// �T���v�����O���[�g
	float samplingRate{ 0 };

	// �L�[�t���[��
	struct Keyframe
	{
		struct Node
		{
			// �L�[�t���[���Ɋ܂܂��m�[�h�̍s��
			DirectX::XMFLOAT4X4 globalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		
			DirectX::XMFLOAT3 scaling{ 1,1,1 };
			DirectX::XMFLOAT4 rotation{ 0,0,0,1 };
			DirectX::XMFLOAT3 translation{ 0,0,0 };

			template<class T>
			void serialize(T& archive)
			{
				archive(globalTransform, scaling, rotation, translation);
			}
		};
		// �L�[�t���[���Ɋ܂܂��S�Ẵm�[�h�̍s��
		std::vector<Node> nodes;

		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	// �A�j���[�V�����P���̃f�[�^
	std::vector<Keyframe> sequence;

	template<class T>
	void serialize(T& archive)
	{
		archive(name, samplingRate, sequence);
	}
};

struct ModelScene
{
	struct Node
	{
		uint64_t uniqueID{ 0 };	// ����ID
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parentIndex{ -1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID, name, attribute, parentIndex);
		}
	};

	std::vector<Node> nodes;
	int64_t IndexOf(uint64_t uniqueID)const
	{
		int64_t index{ 0 };
		for (const Node& node : nodes)
		{
			if (node.uniqueID == uniqueID)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive(nodes);
	}
};

class Model
{
public:
	// �ő�{�[���e���l
	static const int maxBoneInfluences{ 4 };

	struct vertex
	{
		DirectX::XMFLOAT3 position;			// ���_���W
		DirectX::XMFLOAT3 normal{ 0,1,0 };	// �@��
		DirectX::XMFLOAT4 tangent;			// �ڐ��x�N�g��
		DirectX::XMFLOAT2 texcoord{ 0,0 };	// �e�N�X�`�����W
		float boneWeights[maxBoneInfluences]{ 1,0,0,0 };	// �E�F�C�g�l
		uint32_t boneIndices[maxBoneInfluences]{};		// �{�[���ԍ�

		template<class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	static const int MaxBones{ 516 };
	// �萔�o�b�t�@�t�H�[�}�b�g
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;			// ���[���h�s��
		DirectX::XMFLOAT4 materialColor;	// �}�e���A���J���[
		DirectX::XMFLOAT4X4 boneTransforms[MaxBones]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
	};

	//���b�V�����
	struct Mesh
	{
		uint64_t uniqueID{ 0 };		// ����ID
		std::string name;				// ���b�V����
		int64_t nodeIndex{ 0 };		// �m�[�hID
		std::vector<vertex> vertices;	// ���_���W
		std::vector<uint32_t> indices;	// ���_�C���f�b�N�X

		// �T�u�Z�b�g���
		struct Subset
		{
			uint64_t materialUniqueID{ 0 };	// ����ID
			std::string materialName;			// �}�e���A����

			uint32_t startIndexLocation{ 0 };	// �C���f�b�N�X�̊J�n�ʒu
			uint32_t indexCount{ 0 };			// �C���f�b�N�X�̐�(���_)

			template<class T>
			void serialize(T& archive)
			{
				archive(materialUniqueID, materialName, startIndexLocation, indexCount);
			}
		};
		std::vector<Subset> subsets;

		// ���b�V�����Ƃ̃��[���h�s��
		DirectX::XMFLOAT4X4 defaultGlobalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	
		// �o�C���h�|�[�Y(�����p��)
		Skeleton bindPose;

		DirectX::XMFLOAT3 boundingBox[2]
		{
			{D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX},
			{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
		};

		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID, name, nodeIndex, subsets, defaultGlobalTransform,
				bindPose, boundingBox, vertices, indices);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;	// ���_�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;	// �C���f�b�N�X�o�b�t�@
		friend class Model;
	};
	std::vector<Mesh> meshes;

	//�}�e���A��
	struct Material
	{
		uint64_t uniqueID{ 0 };	// ����ID
		std::string name;	// �}�e���A����

		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		// �e�N�X�`���t�@�C����
		std::string textureFilenames[4];
		// �e�N�X�`�����
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];
	
		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID, name, Ka, Kd, Ks, textureFilenames);
		}
	};
	// �ǂݍ��񂾃}�e���A��
	std::unordered_map<uint64_t, Material> materials;

	// �S�ẴA�j���[�V�����̃f�[�^
	std::vector<Animation> animationClips;
public:
	// "triangulate">true...���p�`�ō��ꂽ�|���S�����O�p�`��
	Model(ID3D11Device* device,
		const char* fbxFilename, bool triangulate = false, float samplingRate = 0);

	Model(ID3D11Device* device, std::vector<std::string>& animationFilenames,
		const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
	virtual ~Model() = default;

	// �`�揈��
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world,const DirectX::XMFLOAT4& materialColor);

	// �A�j���[�V�����̒ǉ�
	bool AppendAnimations(const char* animationFilename, float samplingRate);

	// �A�j���[�V�����̘A��
	void BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);

	void BlendAnimations(const Animation::Keyframe* keyframes[2], int nodeIndex, float rate, Animation::Keyframe& keyframe);

	// ���b�V�����̎��o��
	void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);

	// �}�e���A�����̎��o��
	void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);

	// �o�C���h�|�[�Y���̎��o��
	void FetchSkeletons(FbxMesh* fbxMesh, Skeleton& bindPose);

	// �A�j���[�V�������̎��o��
	void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
		float samplingRate);

	// �o�b�t�@�̐���
	void CreateComObjects(ID3D11Device* device, const char* fbxFilename);

	void PlayAnimation(int index,bool loop, float blendSeconds = 0.2f);

	//�A�j���[�V�����Đ������ǂ���
	bool IsPlayAnimation() const;

	const float GetTotalAnimationTime() const
	{
		return totalAnimationTime;
	}

	const float GetAnimationProgress()
	{
		animProgress = currentAnimationSeconds / totalAnimationTime;
		return animProgress;
	}

	Skeleton::Bone* FindNode(const char* node);

	Mesh* FindMesh(const char* name);
	ModelScene sceneView;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;		// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;		// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;		// �萔�o�b�t�@
public:
	// �O��̃A�j���[�V�����̔ԍ�
	int currentAnimationIndex = -1;
	float currentAnimationSeconds = 0.0f;
	float oldAnimationSeconds = 0.0f;
	bool animationLoop = false;
	bool animationEndFlag = false;
	float animationBlendTime;
	float animationBlendSeconds;
	float totalAnimationTime = 0;
	float animProgress = 0;
	Animation::Keyframe keyframe;
	FbxScene* fbxScene{};
protected:
	// ����fbx�̐e�V�[��
	//scene scene_view;
};