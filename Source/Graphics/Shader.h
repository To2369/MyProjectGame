#pragma once
#include <d3d11.h>
#include "Model.h"
#include "Sprite.h"
#include "RenderContext.h"

class Sprite;
class ShaderManager
{
private:
	ShaderManager() {}
	~ShaderManager() {}
public:
	static ShaderManager* Instance()
	{
		static ShaderManager ins;
		return &ins;
	}

	// 頂点シェーダーオブジェクトの生成
	HRESULT CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	// ピクセルシェーダーオブジェクトの生成
	HRESULT CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	// 外部ファイルからテクスチャを読み込む
	HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view,
		D3D11_TEXTURE2D_DESC* texture2d_desc);

	HRESULT LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t sizse, ID3D11ShaderResourceView** shader_resource_view);

	// 全てのテクスチャを解放
	void ReleaseAllTextures(); 

	// ダミーテクスチャを作成(value=色,dimension=サイズ)
	HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value,
		UINT dimension);

	// 定数バッファ作成
	HRESULT CreateConstantBuffer(ID3D11Device* device, UINT bufferSize, ID3D11Buffer** constantBuffer);
};

class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;

	// 描画
	virtual void Draw(const RenderContext& rc, Model* model) = 0;

	// 描画終了
	virtual void End(const RenderContext& rc) = 0;
};

class SpriteShader
{
public:
	SpriteShader() {}
	virtual ~SpriteShader() {}

	// 描画開始
	virtual void Begin(const RenderContext& rc) = 0;

	// 描画
	virtual void Draw(const RenderContext& rc, const Sprite* sprite) = 0;

	// 描画終了
	virtual void End(const RenderContext& rc) = 0;
};
