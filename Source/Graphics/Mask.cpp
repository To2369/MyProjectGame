#include "Mask.h"
#include "Graphics.h"
#include "Buffer.h"
Mask::Mask(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	{
		ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\LineVS.cso", vertexShader.GetAddressOf(),
			inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	// ピクセルシェーダー
	{
		ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\LinePS.cso", pixelShader.GetAddressOf());
	}

	CreateBuffer<CbMask>(device, maskConstantBuffer.GetAddressOf());
}

void Mask::Begin(const RenderContext& rc)
{
	ID3D11Buffer* constantBuffers[] =
	{
		maskConstantBuffer.Get(),
	};
	//	頂点シェーダー用の定数バッファ用の番地「0」番に転送する
	rc.deviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	//	ピクセルシェーダー用の定数バッファ用の番地「0」番に転送する
	rc.deviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	rc.deviceContext->IASetInputLayout(inputLayout.Get());

	rc.deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	rc.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
	rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
	rc.deviceContext->RSSetState(rc.renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

	// 3D 描画設定
	//ID3D11SamplerState* samplers[] =
	//{
	//	rc.renderState->GetSamplerState(SAMPLER_STATE::POINT),
	//	rc.renderState->GetSamplerState(SAMPLER_STATE::LINEAR),
	//	rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC),
	//};
	//rc.deviceContext->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
	ID3D11SamplerState* samplers[] =
	{
		rc.renderState->GetSamplerState(SAMPLER_STATE::POINT),
		rc.renderState->GetSamplerState(SAMPLER_STATE::LINEAR),
		rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC),
	};
	rc.deviceContext->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
}