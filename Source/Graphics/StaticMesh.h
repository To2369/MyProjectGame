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
class StaticMesh
{
public:
	// DCCツールによって上下成分や原点が違う場合があるのでテクスチャが反転している場合
	// flippingVcoodinatesをtrueにする
	StaticMesh(ID3D11Device* device, const wchar_t* objFilename, bool flippingVcoordinates);
	virtual ~StaticMesh() = default;

	// 描画処理
	void Render(ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
		PIXEL_SHADER_STATE state);

public:
	// 頂点フォーマット
	struct Vertex
	{
		DirectX::XMFLOAT3 position;	// 座標
		DirectX::XMFLOAT3 normal;	// 法線
		DirectX::XMFLOAT2 texcoord;	// テクスチャ座標
	};

	// 定数バッファ用フォーマット
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;	// ワールド行列
		DirectX::XMFLOAT4 Ka;
		DirectX::XMFLOAT4 Kd;
		DirectX::XMFLOAT4 Ks;
		DirectX::XMFLOAT4 materialColor;
	};

	// マテリアル名とDrawIndex()に必要な情報部分をサブセット化
	struct Subset
	{
		std::wstring usemtl;		// マテリアル名
		uint32_t indexStart{ 0 };	// インデックスの開始位置
		uint32_t indexCount{ 0 };	// インデクスの数（頂点数）
	};
	std::vector<Subset> subsets;

	// マテリアル
	struct Material
	{
		// マテリアル名
		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };
		// テクスチャファイル名
		std::wstring textureFilenames[static_cast<int>(STATICMESH_STATE::Enum_Max)];
		// テクスチャ情報
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView[static_cast<int>(STATICMESH_STATE::Enum_Max)];
	};
	// 読み込んだマテリアル
	std::vector<Material> materials;

	DirectX::XMFLOAT3 boundingBox[2]{
		{D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX},
		{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
	};
protected:
	// 頂点バッファオブジェクトの作成
	void CreateComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount,
		uint32_t* indices, size_t indexCount);
private:
	// 頂点バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	// インデックスバッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// 頂点シェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	// ピクセルシェーダーオブジェクト
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::Enum_Max)];
	// 入力レイアウトオブジェクト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	// 定数バッファオブジェクト
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	// 定数バッファ
	D3D11_BUFFER_DESC bufferDesc{};

	D3D11_SUBRESOURCE_DATA subresourceData{};
};