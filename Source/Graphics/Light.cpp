#include"..\misc.h"
#include "Graphics.h"
#include "Buffer.h"
#include "Light.h"
Light::Light(ID3D11Device* device)
{
	HRESULT hr{ S_OK };
	CreateBuffer<Light::lightConstants>(device, lightConstantBuffer.GetAddressOf());

	// ブレンドステート
	{
		// アルファブレンド
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
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

		HRESULT hr = device->CreateSamplerState(&desc, sampler_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\PhongShaderVS.cso",
		meshVertexShader.GetAddressOf(), meshInputLayout.GetAddressOf(),
		inputElementDesc, ARRAYSIZE(inputElementDesc));
	ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\PhongShaderPS.cso",
		meshPixelShader.GetAddressOf());
}

void Light::Update(float elapsedTIme)
{
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::Separator();
	ImGui::ColorEdit3("ambient_color", &ambientColor.x);
	ImGui::SliderFloat3("directional_light_direction", &directionalLightDirection.x, -1.0f, +1.0f);
	ImGui::ColorEdit3("directional_light_color", &directionalLightColor.x);

	ImGui::End();
#endif
}

void Light::Render(ID3D11DeviceContext* dc)
{
	//描画コンテキスト
	RenderContext rc;
	rc.deviceContext = dc;
	dc->VSSetShader(meshVertexShader.Get(), nullptr, 0);
	dc->PSSetShader(meshPixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(meshInputLayout.Get());
	//immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	// 3D 描画設定
	ID3D11SamplerState* samplers[] =
	{
		rc.renderState->GetSamplerState(SAMPLER_STATE::POINT),
		rc.renderState->GetSamplerState(SAMPLER_STATE::LINEAR),
		rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC),
	};
	dc->PSSetSamplers(0, ARRAYSIZE(samplers), samplers);
	const float blend_factor[4] = { 1,1,1,1 };
	dc->OMSetBlendState(blend_state.Get(), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	dc->RSSetState(rasterizer_state.Get());

	lightConstants lights{};
	lights.ambientColor = ambientColor;
	lights.directionalLightDirection = directionalLightDirection;
	lights.directionalLightColor = directionalLightColor;
	BindBuffer(dc, 2, lightConstantBuffer.GetAddressOf(), &lights);

}