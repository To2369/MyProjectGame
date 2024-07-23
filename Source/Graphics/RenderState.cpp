#include"RenderState.h"
#include"..\misc.h"
RenderState::RenderState(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	// サンプラーステートを生成するための構造体
	{
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のauテクスチャ座標を解決するために使用するメソッド
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のavテクスチャ座標を解決するために使用する方法
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のawテクスチャ座標を解決するために使用する方法
		sampler_desc.MipLODBias = 0;							// 計算されたミップマップレベルからのオフセット。
		sampler_desc.MaxAnisotropy = 16;						// クランプ値
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// サンプリングされたデータを既存のサンプリングされたデータと比較する関数
		// AddressU,V,W にD3D11_TEXTURE_ADDRESS_BORDER が指定されている場合に使用する境界線の色
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;								// アクセスをクランプするミップマップの範囲の下限
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;				// アクセスをクランプするミップマップの範囲の上限

		// ポイントサンプラーステート(粗い)
		{
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// テクスチャをサンプリングするときに使用するフィルタリング方法
			// ポイントフィルタとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::POINT)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// ポイントサンプラーステート(滑らか)
		{
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// テクスチャをサンプリングするときに使用するフィルタリング方法
			// リニアフィルタとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::LINEAR)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// アニソトロピックサンプラーステート(滑らか3D)
		{
			sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;			// テクスチャをサンプリングするときに使用するフィルタリング方法
			// アニソトロピックとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	{
		// 深度テストを ON 深度書き込みを ON
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		{
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::ON_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを ON 深度書き込みを OFF
		{
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::ON_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを OFF 深度書き込みを ON
		{
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::OFF_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを OFF 深度書き込みを OFF
		{
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::OFF_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC blend_desc{};

		// 合成無し
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = FALSE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::NONE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 通常合成
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::ALPHABLENDING)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 加算合成
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::ADD)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 減算合成
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::SUB)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 乗算合成
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::MULTIPLE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// ラスタライザステートを作成するための構造体
	D3D11_RASTERIZER_DESC rasterizer_desc{};

	// ラスタライザステート構造体共通項
	rasterizer_desc.FrontCounterClockwise = FALSE;	// false...時計回りを表面とする、反時計回りを裏面
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;

	// 「ソリッド」で「裏面非表示」
	{
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::SOLID_CULLBACK)].GetAddressOf());
	}

	// 「ワイヤーフレーム」で「裏面非表示」
	{
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::WIRE_CULLBACK)].GetAddressOf());
	}

	// 「ソリッド」で「両面表示」
	{
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::SOLID_CULLNONE)].GetAddressOf());
	}

	// 「ワイヤーフレーム」で「両面表示」
	{
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::WIRE_CULLNONE)].GetAddressOf());
	}
}