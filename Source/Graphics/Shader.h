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

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	HRESULT CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	HRESULT CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);

	// �O���t�@�C������e�N�X�`����ǂݍ���
	HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView,
		D3D11_TEXTURE2D_DESC* texture2dDesc);

	HRESULT LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView);

	// �S�Ẵe�N�X�`�������
	void ReleaseAllTextures(); 

	// �_�~�[�e�N�X�`�����쐬(value=�F,dimension=�T�C�Y)
	HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value,
		UINT dimension);
};

class ModelShader
{
public:
	ModelShader() {}
	virtual ~ModelShader() {}

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(const RenderContext& rc, Model* model) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc) = 0;
};

class SpriteShader
{
public:
	SpriteShader() {}
	virtual ~SpriteShader() {}

	// �`��J�n
	virtual void Begin(const RenderContext& rc) = 0;

	// �`��
	virtual void Draw(const RenderContext& rc, const Sprite* sprite) = 0;

	// �`��I��
	virtual void End(const RenderContext& rc) = 0;
};
