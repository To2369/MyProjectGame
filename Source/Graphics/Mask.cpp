#include "Mask.h"
#include"..\Misc.h"
#include "Buffer.h"
Mask::Mask(ID3D11Device* device)
{
	CreateBuffer<dissolve_constants>(device, dissolve_constant_buffer.GetAddressOf());
	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	HRESULT hr{ S_OK };
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダーオブジェクトの生成
	{
		ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\MaskVS.cso",
			vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	}

	// ピクセルシェーダーオブジェクトの生成
	{
		ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\MaskPS.cso",
			pixelShader.GetAddressOf());
	}
}
//void Mask::Begin(ID3D11DeviceContext* dc)
//{
//	ID3D11Buffer* constantBuffers[] =
//	{
//		dissolve_constant_buffer.Get(),
//	};
//	//	頂点シェーダー用の定数バッファ用の番地「0」番に転送する
//	dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
//	//	ピクセルシェーダー用の定数バッファ用の番地「0」番に転送する
//	dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
//
//	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
//	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
//	dc->IASetInputLayout(inputLayout.Get());
//}
//void Mask::Draw(ID3D11DeviceContext* dc, Sprite* sprite)
//{
//	dissolve_constants dissolve{};
//	dissolve.parameters.x = dissolve_value;
//	dc->UpdateSubresource(dissolve_constant_buffer.Get(), 0, 0, &dissolve, 0, 0);
//
//	UINT stride = sizeof(Sprite::Vertex);
//	UINT offset = 0;
//	dc->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
//
//	dc->PSSetShaderResources(1, 1, sprite->GetShaderResourceView().GetAddressOf());
//
//	dc->Draw(4, 0);
//}
//void Mask::End(ID3D11DeviceContext* dc)
//{
//	dc->VSSetShader(nullptr, nullptr, 0);
//	dc->PSSetShader(nullptr, nullptr, 0);
//	dc->IASetInputLayout(nullptr);
//	ID3D11ShaderResourceView* srvs[] = { nullptr, nullptr };
//	dc->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
//}
void Mask::Render(ID3D11DeviceContext*dc,Sprite*sprite)
{
	dc->IASetInputLayout(inputLayout.Get());
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	dc->PSSetShaderResources(1, 1, sprite->GetShaderResourceView().GetAddressOf());
	dissolve_constants dissolve{};
	dissolve.parameters.x = dissolve_value;

	BindBuffer(dc, 0, dissolve_constant_buffer.GetAddressOf(), &dissolve);
	dc->Draw(4, 0);
}