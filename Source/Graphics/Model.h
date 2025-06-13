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

// スケルトン
// 複数のボーンを管理
struct Skeleton
{
	// メッシュのボーン情報
	struct Bone
	{
		uint64_t uniqueID{ 0 };	// 識別ID
		std::string name;
		// 親ボーンの位置を参照するインデックス	-1...親無し
		int64_t parentIndex{ -1 };
		// シーンのノード配列を参照するインデックス
		int64_t nodeIndex{ 0 };

		// モデル(メッシュ)空間からボーン(ノード)に変換するために使用
		DirectX::XMFLOAT4X4 offsetTransform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		// true...親無しのボーン
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

// アニメーション
struct Animation
{
	// アニメーションの名前
	std::string name;
	// サンプリングレート
	float samplingRate{ 0 };

	// キーフレーム
	struct Keyframe
	{
		struct Node
		{
			// キーフレームに含まれるノードの行列
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
		// キーフレームに含まれる全てのノードの行列
		std::vector<Node> nodes;

		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	// アニメーション１つ分のデータ
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
		uint64_t uniqueID{ 0 };	// 識別ID
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
	// 最大ボーン影響値
	static const int maxBoneInfluences{ 4 };

	struct vertex
	{
		DirectX::XMFLOAT3 position;			// 頂点座標
		DirectX::XMFLOAT3 normal{ 0,1,0 };	// 法線
		DirectX::XMFLOAT4 tangent;			// 接線ベクトル
		DirectX::XMFLOAT2 texcoord{ 0,0 };	// テクスチャ座標
		float boneWeights[maxBoneInfluences]{ 1,0,0,0 };	// ウェイト値
		uint32_t boneIndices[maxBoneInfluences]{};		// ボーン番号

		template<class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};

	static const int MaxBones{ 516 };
	// 定数バッファフォーマット
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;			// ワールド行列
		DirectX::XMFLOAT4 materialColor;	// マテリアルカラー
		DirectX::XMFLOAT4X4 boneTransforms[MaxBones]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
	};

	//メッシュ情報
	struct Mesh
	{
		uint64_t uniqueID{ 0 };		// 識別ID
		std::string name;				// メッシュ名
		int64_t nodeIndex{ 0 };		// ノードID
		std::vector<vertex> vertices;	// 頂点座標
		std::vector<uint32_t> indices;	// 頂点インデックス

		// サブセット情報
		struct Subset
		{
			uint64_t materialUniqueID{ 0 };	// 識別ID
			std::string materialName;			// マテリアル名

			uint32_t startIndexLocation{ 0 };	// インデックスの開始位置
			uint32_t indexCount{ 0 };			// インデックスの数(頂点)

			template<class T>
			void serialize(T& archive)
			{
				archive(materialUniqueID, materialName, startIndexLocation, indexCount);
			}
		};
		std::vector<Subset> subsets;

		// メッシュごとのワールド行列
		DirectX::XMFLOAT4X4 defaultGlobalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	
		// バインドポーズ(初期姿勢)
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
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;	// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;	// インデックスバッファ
		friend class Model;
	};
	std::vector<Mesh> meshes;

	//マテリアル
	struct Material
	{
		uint64_t uniqueID{ 0 };	// 識別ID
		std::string name;	// マテリアル名

		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		// テクスチャファイル名
		std::string textureFilenames[4];
		// テクスチャ情報
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];
	
		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID, name, Ka, Kd, Ks, textureFilenames);
		}
	};
	// 読み込んだマテリアル
	std::unordered_map<uint64_t, Material> materials;

	// 全てのアニメーションのデータ
	std::vector<Animation> animationClips;
public:
	// "triangulate">true...多角形で作られたポリゴンを三角形化
	Model(ID3D11Device* device,
		const char* fbxFilename, bool triangulate = false, float samplingRate = 0);

	Model(ID3D11Device* device, std::vector<std::string>& animationFilenames,
		const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
	virtual ~Model() = default;

	// 描画処理
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world,const DirectX::XMFLOAT4& materialColor);

	// アニメーションの追加
	bool AppendAnimations(const char* animationFilename, float samplingRate);

	// アニメーションの連結
	void BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe);

	void BlendAnimations(const Animation::Keyframe* keyframes[2], int nodeIndex, float rate, Animation::Keyframe& keyframe);

	// メッシュ情報の取り出し
	void FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes);

	// マテリアル情報の取り出し
	void FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);

	// バインドポーズ情報の取り出し
	void FetchSkeletons(FbxMesh* fbxMesh, Skeleton& bindPose);

	// アニメーション情報の取り出し
	void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
		float samplingRate);

	// バッファの生成
	void CreateComObjects(ID3D11Device* device, const char* fbxFilename);

	void PlayAnimation(int index,bool loop, float blendSeconds = 0.2f);

	//アニメーション再生中かどうか
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

	Skeleton::Bone* FindNode(std::string node);

	Mesh* FindMesh(const char* name);
	ModelScene sceneView;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;		// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;		// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;		// 定数バッファ
public:
	// 前回のアニメーションの番号
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
	// このfbxの親シーン
	//scene scene_view;
};