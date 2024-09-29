#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>
#include<vector>
#include<string>
#include<fbxsdk.h>
#include<unordered_map>
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
struct skeleton
{
	// メッシュのボーン情報
	struct bone
	{
		uint64_t unique_id{ 0 };	// 識別ID
		std::string name;
		// 親ボーンの位置を参照するインデックス	-1...親無し
		int64_t parent_index{ -1 };
		// シーンのノード配列を参照するインデックス
		int64_t node_index{ 0 };

		// モデル(メッシュ)空間からボーン(ノード)に変換するために使用
		DirectX::XMFLOAT4X4 offset_transform{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

		// true...親無しのボーン
		bool IsOrphans() const { return parent_index < 0; };

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, parent_index, node_index, offset_transform);
		}
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

	template<class T>
	void serialize(T& archive)
	{
		archive(bones);
	}
};

// アニメーション
struct animation
{
	// アニメーションの名前
	std::string name;
	// サンプリングレート
	float sampling_rate{ 0 };

	// キーフレーム
	struct keyframe
	{
		struct node
		{
			// キーフレームに含まれるノードの行列
			DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		
			DirectX::XMFLOAT3 scaling{ 1,1,1 };
			DirectX::XMFLOAT4 rotation{ 0,0,0,1 };
			DirectX::XMFLOAT3 translation{ 0,0,0 };

			template<class T>
			void serialize(T& archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}
		};
		// キーフレームに含まれる全てのノードの行列
		std::vector<node> nodes;

		template<class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	// アニメーション１つ分のデータ
	std::vector<keyframe> sequence;

	template<class T>
	void serialize(T& archive)
	{
		archive(name, sampling_rate, sequence);
	}
};

struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };	// 識別ID
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, attribute, parent_index);
		}
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
	static const int MAX_BONE_INFLUENCES{ 4 };

	struct vertex
	{
		DirectX::XMFLOAT3 position;			// 頂点座標
		DirectX::XMFLOAT3 normal{ 0,1,0 };	// 法線
		DirectX::XMFLOAT4 tangent;			// 接線ベクトル
		DirectX::XMFLOAT2 texcoord{ 0,0 };	// テクスチャ座標
		float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };	// ウェイト値
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};		// ボーン番号

		template<class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
		}
	};

	static const int MAX_BONES{ 256 };
	// 定数バッファフォーマット
	struct constants
	{
		DirectX::XMFLOAT4X4 world;			// ワールド行列
		DirectX::XMFLOAT4 material_color;	// マテリアルカラー
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1} };
	};

	//メッシュ情報
	struct mesh
	{
		uint64_t unique_id{ 0 };		// 識別ID
		std::string name;				// メッシュ名
		int64_t node_index{ 0 };		// ノードID
		std::vector<vertex> vertices;	// 頂点座標
		std::vector<uint32_t> indices;	// 頂点インデックス

		// サブセット情報
		struct subset
		{
			uint64_t material_unique_id{ 0 };	// 識別ID
			std::string material_name;			// マテリアル名

			uint32_t start_index_location{ 0 };	// インデックスの開始位置
			uint32_t index_count{ 0 };			// インデックスの数(頂点)

			template<class T>
			void serialize(T& archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}
		};
		std::vector<subset> subsets;

		// メッシュごとのワールド行列
		DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	
		// バインドポーズ(初期姿勢)
		skeleton bind_pose;

		DirectX::XMFLOAT3 bounding_box[2]
		{
			{D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX},
			{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
		};

		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, node_index, subsets, default_global_transform,
				bind_pose, bounding_box, vertices, indices);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;	// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;	// インデックスバッファ
		friend class Model;
	};
	std::vector<mesh> meshes;

	//マテリアル
	struct material
	{
		uint64_t unique_id{ 0 };	// 識別ID
		std::string name;	// マテリアル名

		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		// テクスチャファイル名
		std::string texture_filenames[4];
		// テクスチャ情報
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
	
		template<class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
		}
	};
	// 読み込んだマテリアル
	std::unordered_map<uint64_t, material> materials;

	// 全てのアニメーションのデータ
	std::vector<animation> animation_clips;
public:
	// "triangulate">true...多角形で作られたポリゴンを三角形化
	Model(ID3D11Device* device,
		const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);

	Model(ID3D11Device* device, std::vector<std::string>& animation_filenames,
		const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
	virtual ~Model() = default;

	// 描画処理
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& material_color, const animation::keyframe* keyframe);

	//	アニメーション更新
	void UpdateAnimation(float elapsedTime);

	// アニメーションの追加
	bool AppendAnimations(const char* animation_filename, float sampling_rate);

	// アニメーションの連結
	void BlendAnimations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);

	// メッシュ情報の取り出し
	void FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);

	// マテリアル情報の取り出し
	void FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

	// バインドポーズ情報の取り出し
	void FetchSkeletons(FbxMesh* fbx_mesh, skeleton& bind_pose);

	// アニメーション情報の取り出し
	void FetchAnimations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
		float sampling_rate);

	// バッファの生成
	void CreateComObjects(ID3D11Device* devvice, const char* fbx_filename);

	void PlayAnimation(int index,bool loop, float blendSeconds = 0.2f);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;		// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;		// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;		// 定数バッファ

	int animIndex = 0;
	bool animLoop = false;
	animation::keyframe* keyframe = nullptr;
protected:
	// このfbxの親シーン
	scene scene_view;
};