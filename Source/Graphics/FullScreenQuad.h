#pragma once
#include<d3d11.h>
#include<wrl.h>
#include<cstdint>
class FullScreenQuad
{
public:
	FullScreenQuad(ID3D11Device* device);
	virtual ~FullScreenQuad() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShader;

public:
	// �I�t�X�N���[���`�悵���e�N�X�`���̃X�N���[���ւ̍ŏI�I�ȕ`�揈��
	void Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
		uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader = nullptr);
};