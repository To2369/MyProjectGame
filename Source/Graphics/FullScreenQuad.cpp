#include "FullScreenQuad.h"
#include "Shader.h"
#include "../misc.h"
#include<d3d11.h>
FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\FullScreenQuadVS.cso", embeddedVertexShader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\FullScreenQuadPS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}

// �I�t�X�N���[���`�悵���e�N�X�`���̃X�N���[���ւ̍ŏI�I�ȕ`�揈��
void FullScreenQuad::Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
{
	// ���_�Ȃǂ͂����炩��͑���Ȃ��̂œ��ׂ� null
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	// �V�F�[�_�[���Œ��ɒl��ݒ肷��̂Ńv���~�e�B�u�g�|���W�[�͐ݒ�
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// ���_�V�F�[�_�[�ɑ���l�͖����̂œ��̓��C�A�E�g�� null
	dc->IASetInputLayout(nullptr);

	dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? dc->PSSetShader(replacedPixelShader, 0, 0) :
		dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	// �I�t�X�N���[���Ƃ��ĕ`���ꂽ�e�N�X�`����ݒ�
	dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	dc->Draw(4, 0);
}