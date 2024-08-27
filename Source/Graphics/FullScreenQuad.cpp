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

// オフスクリーン描画したテクスチャのスクリーンへの最終的な描画処理
void FullScreenQuad::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** shader_resource_view,
	uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
	// 頂点などはこちらからは送らないので統べて null
	immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	// シェーダー側で直に値を設定するのでプリミティブトポロジーは設定
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 頂点シェーダーに送る値は無いので入力レイアウトは null
	immediate_context->IASetInputLayout(nullptr);

	immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) :
		immediate_context->PSSetShader(embedded_pixel_shader.Get(), 0, 0);

	// オフスクリーンとして描かれたテクスチャを設定
	immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_view);

	immediate_context->Draw(4, 0);
}