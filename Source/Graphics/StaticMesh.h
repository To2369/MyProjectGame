#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include<string>
//スタティックメッシュ
//スタティックメッシュとはスケルトンが入っていない変形させるようなアニメーションができないメッシュの事です。
//例えば椅子や岩、壁といったメッシュはスタティックメッシュにあたります。（静的オブジェクト）
//メッシュとはポリゴンの集まりのこと。イメージとしては中身のない表面だけのデータ
class StaticMesh
{
public:
	//DCCツールに予って上下成分や原点が違う場合があるのでテクスチャが反転している場合
	//flipping_v_coodinatesをtrueにする
	StaticMesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates);
	virtual ~StaticMesh() = default;

	//描画処理
	void Render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

public:
	//頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;	//座標
		DirectX::XMFLOAT3 normal;	//法線
		DirectX::XMFLOAT2 texcoord;	//テクスチャ座標
	};

	//定数バッファ用フォーマット
	struct constants
	{
		DirectX::XMFLOAT4X4 world;	//ワールド行列
		DirectX::XMFLOAT4 material_color;	//マテリアルカラー
	};

	//テクスチャファイル名
	std::wstring texture_filename;
	//テクスチャ情報
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
protected:
	//頂点バッファオブジェクトの作成
	void Create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
		uint32_t* indices, size_t index_count);
private:
	//頂点バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//インデックスバッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	//頂点シェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//ピクセルシェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	//入力レイアウトオブジェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	//定数バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	//定数バッファ
	D3D11_BUFFER_DESC buffer_desc{};

	D3D11_SUBRESOURCE_DATA subresource_data{};
};