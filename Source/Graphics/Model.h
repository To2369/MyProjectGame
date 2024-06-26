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
public:
	//"triangulate">true...多角形で作られたポリゴンを三角形化
	Model(ID3D11Device* device, const char* fbx_filename, bool triangulate = false);
	virtual ~Model() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;	//頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixsl_shader;		//ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;		//入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;		//定数バッファ
protected:
	//このfbxの親シーン
	scene scene_view;
};