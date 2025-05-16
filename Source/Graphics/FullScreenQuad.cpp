#include "FullScreenQuad.h"
#include "Shader.h"
#include "../Misc.h"
#include<d3d11.h>
FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\FullScreenQuadVS.cso", embeddedVertexShader.ReleaseAndGetAddressOf(),
		nullptr, nullptr, 0);
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\FullScreenQuadPS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}

// オフスクリーン描画したテクスチャのスクリーンへの最終的な描画処理
void FullScreenQuad::Blit(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
{
	// 頂点などはこちらからは送らないので統べて null
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	// シェーダー側で直に値を設定するのでプリミティブトポロジーは設定
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 頂点シェーダーに送る値は無いので入力レイアウトは null
	dc->IASetInputLayout(nullptr);

	dc->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ? dc->PSSetShader(replacedPixelShader, 0, 0) :
		dc->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	// オフスクリーンとして描かれたテクスチャを設定
	dc->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	dc->Draw(4, 0);
}