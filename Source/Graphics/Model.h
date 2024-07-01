#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<DirectXMath.h>
#include<vector>
#include<string>
#include<fbxsdk.h>
#include<unordered_map>
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
		DirectX::XMFLOAT3 position;			//頂点座標
		DirectX::XMFLOAT3 normal{ 0,1,0 };	//法線
		DirectX::XMFLOAT2 texcoord{ 0,0 };	//テクスチャ座標
	};

	//定数バッファフォーマット
	struct constants
	{
		DirectX::XMFLOAT4X4 world;			//ワールド行列
		DirectX::XMFLOAT4 material_color;	//マテリアルカラー
	};

	//メッシュ情報
	struct mesh
	{
		uint64_t unique_id{ 0 };		//識別ID
		std::string name;				//メッシュ名
		int64_t node_index{ 0 };		//ノードID
		std::vector<vertex> vertices;	//頂点座標
		std::vector<uint32_t> indices;	//頂点インデックス

		//サブセット情報
		struct subset
		{
			uint64_t material_unique_id{ 0 };	//識別ID
			std::string material_name;			//マテリアル名

			uint32_t start_index_location{ 0 };	//インデックスの開始位置
			uint32_t index_count{ 0 };			//インデックスの数(頂点)
		};
		std::vector<subset> subsets;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;	//頂点バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;	//インデックスバッファ
		friend class Model;
	};
	std::vector<mesh> meshes;

	//マテリアル
	struct material
	{
		uint64_t unique_id{ 0 };	//識別ID
		std::string name;	//マテリアル名

		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		//テクスチャファイル名
		std::string texture_filename[4];
		//テクスチャ情報
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];
	};
	//読み込んだマテリアル
	std::unordered_map<uint64_t, material> materials;
public:
	//"triangulate">true...多角形で作られたポリゴンを三角形化
	Model(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
	virtual ~Model() = default;

	//描画処理
	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& material_color);

	// メッシュ情報の取り出し
	void FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);

	//マテリアル情報の取り出し
	void FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

	//バッファの生成
	void CreateComObjects(ID3D11Device* devvice, const char* fbx_filename);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	//頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;		//ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;		//入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;		//定数バッファ
protected:
	//このfbxの親シーン
	scene scene_view;
};