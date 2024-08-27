#include "FullScreenQuad.h"
#include "Shader.h"
#include "../misc.h"
#include<d3d11.h>
FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\FullScreenQuadVS.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\FullScreenQuadPS.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());
}

// �I�t�X�N���[���`�悵���e�N�X�`���̃X�N���[���ւ̍ŏI�I�ȕ`�揈��
void FullScreenQuad::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** shader_resource_view,
	uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
	// ���_�Ȃǂ͂����炩��͑���Ȃ��̂œ��ׂ� null
	immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	// �V�F�[�_�[���Œ��ɒl��ݒ肷��̂Ńv���~�e�B�u�g�|���W�[�͐ݒ�
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// ���_�V�F�[�_�[�ɑ���l�͖����̂œ��̓��C�A�E�g�� null
	immediate_context->IASetInputLayout(nullptr);

	immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) :
		immediate_context->PSSetShader(embedded_pixel_shader.Get(), 0, 0);

	// �I�t�X�N���[���Ƃ��ĕ`���ꂽ�e�N�X�`����ݒ�
	immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_view);

	immediate_context->Draw(4, 0);
}