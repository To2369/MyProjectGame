#pragma once
#include <d3d11.h>
#include "Model.h"
#include "Sprite.h"
#include "RenderContext.h"
enum SPRITE_SHADER_STATE
{
	DEFAULT,
	MASK,

	EnumMax
};

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
	HRESULT CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);

	// ピクセルシェーダーオブジェクトの生成
	HRESULT CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);

	// 外部ファイルからテクスチャを読み込む
	HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc);

	HRESULT LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView);

	// 全てのテクスチャを解放
	void ReleaseAllTextures(); 

	// ダミーテクスチャを作成(value=色,dimension=サイズ)
	HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value,
		UINT dimension);
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
