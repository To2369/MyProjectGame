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
// スタティックメッシュ
// スタティックメッシュとはスケルトンが入っていない変形させるようなアニメーションができないメッシュの事です。
// えば椅子や岩、壁といったメッシュはスタティックメッシュにあたります。（静的オブジェクト）
// メッシュとはポリゴンの集まりのこと。イメージとしては中身のない表面だけのデータ
class StaticMesh
{
public:
	// DCCツールによって上下成分や原点が違う場合があるのでテクスチャが反転している場合
	// flipping_v_coodinatesをtrueにする
	StaticMesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates);
	virtual ~StaticMesh() = default;

	// 描画処理
	void Render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
		PIXEL_SHADER_STATE state);

public:
	// 頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;	// 座標
		DirectX::XMFLOAT3 normal;	// 法線
		DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
	};

	// 定数バッファ用フォーマット
	struct constants
	{
		DirectX::XMFLOAT4X4 world;	// ワールド行列
		DirectX::XMFLOAT4 material_color;	// マテリアルカラー
	};

	// マテリアル名とDrawIndex()に必要な情報部分をサブセット化
	struct subset
	{
		std::wstring usemtl;		// マテリアル名
		uint32_t index_start{ 0 };	// インデックスの開始位置
		uint32_t index_count{ 0 };	// インデクスの数（頂点数）
	};
	std::vector<subset> subsets;

	// マテリアル
	struct material
	{
		// マテリアル名
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };
		// テクスチャファイル名
		std::wstring texture_filenames[static_cast<int>(STATICMESH_STATE::Enum_Max)];
		// テクスチャ情報
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view[static_cast<int>(STATICMESH_STATE::Enum_Max)];
	};
	// 読み込んだマテリアル
	std::vector<material> materials;

	DirectX::XMFLOAT3 bounding_box[2]{
		{D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX},
		{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
	};
protected:
	// 頂点バッファオブジェクトの作成
	void Create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);
private:
	// 頂点バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	// インデックスバッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	// 頂点シェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	// ピクセルシェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[static_cast<int>(PIXEL_SHADER_STATE::Enum_Max)];
	// 入力レイアウトオブジェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	// 定数バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	// 定数バッファ
	D3D11_BUFFER_DESC buffer_desc{};

	D3D11_SUBRESOURCE_DATA subresource_data{};
};