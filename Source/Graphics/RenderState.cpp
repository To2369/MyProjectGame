#include"RenderState.h"
#include"..\misc.h"
RenderState::RenderState(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	// サンプラーステートを生成するための構造体
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のauテクスチャ座標を解決するために使用するメソッド
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のavテクスチャ座標を解決するために使用する方法
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// ０から１の範囲外のawテクスチャ座標を解決するために使用する方法
		samplerDesc.MipLODBias = 0;							// 計算されたミップマップレベルからのオフセット。
		samplerDesc.MaxAnisotropy = 16;						// クランプ値
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// サンプリングされたデータを既存のサンプリングされたデータと比較する関数
		// AddressU,V,W にD3D11_TEXTURE_ADDRESS_BORDER が指定されている場合に使用する境界線の色
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;								// アクセスをクランプするミップマップの範囲の下限
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;				// アクセスをクランプするミップマップの範囲の上限

		// ポイントサンプラーステート(粗い)
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// テクスチャをサンプリングするときに使用するフィルタリング方法
			// ポイントフィルタとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::POINT)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// ポイントサンプラーステート(滑らか)
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// テクスチャをサンプリングするときに使用するフィルタリング方法
			// リニアフィルタとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::LINEAR)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// アニソトロピックサンプラーステート(滑らか3D)
		{
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;			// テクスチャをサンプリングするときに使用するフィルタリング方法
			// アニソトロピックとしてサンプラーステートを生成
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	{
		// 深度テストを ON 深度書き込みを ON
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
		{
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::ON_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを ON 深度書き込みを OFF
		{
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::ON_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを OFF 深度書き込みを ON
		{
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::OFF_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 深度テストを OFF 深度書き込みを OFF
		{
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::OFF_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC blendDesc{};

		// 合成無し
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::NONE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 通常合成
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::ALPHABLENDING)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 加算合成
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::ADD)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 減算合成
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::SUB)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// 乗算合成
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::MULTIPLE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// ラスタライザステートを作成するための構造体
	D3D11_RASTERIZER_DESC rasterizerDesc{};

	// ラスタライザステート構造体共通項
	rasterizerDesc.FrontCounterClockwise = FALSE;	// false...時計回りを表面とする、反時計回りを裏面
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;

	// 「ソリッド」で「裏面非表示」
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::SOLID_CULLBACK)].GetAddressOf());
	}

	// 「ワイヤーフレーム」で「裏面非表示」
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIRE_CULLBACK)].GetAddressOf());
	}

	// 「ソリッド」で「両面表示」
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::SOLID_CULLNONE)].GetAddressOf());
	}

	// 「ワイヤーフレーム」で「両面表示」
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIRE_CULLNONE)].GetAddressOf());
	}
}