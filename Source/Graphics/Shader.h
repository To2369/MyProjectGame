#pragma once
#include <d3d11.h>

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
	HRESULT CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	HRESULT CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	// �O���t�@�C������e�N�X�`����ǂݍ���
	HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view,
		D3D11_TEXTURE2D_DESC* texture2d_desc);

	// �S�Ẵe�N�X�`�������
	void release_all_textures(); 

	// �_�~�[�e�N�X�`�����쐬(value=�F,dimension=�T�C�Y)
	HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value,
		UINT dimension);

	// �萔�o�b�t�@�쐬
	HRESULT CreateConstantBuffer(ID3D11Device* device, UINT bufferSize, ID3D11Buffer** constantBuffer);
};