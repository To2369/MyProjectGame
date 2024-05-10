#include"GraphicsManager.h"
#include"..\misc.h"
bool GraphicsManager::Initialize(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	//サンプラーステートを生成するための構造体
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//０から１の範囲外のauテクスチャ座標を解決するために使用するメソッド
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//０から１の範囲外のavテクスチャ座標を解決するために使用する方法
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//０から１の範囲外のawテクスチャ座標を解決するために使用する方法
	sampler_desc.MipLODBias = 0;							//計算されたミップマップレベルからのオフセット。
	sampler_desc.MaxAnisotropy = 16;						//クランプ値
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	//サンプリングされたデータを既存のサンプリングされたデータと比較する関数
	// AddressU,V,W にD3D11_TEXTURE_ADDRESS_BORDER が指定されている場合に使用する境界線の色
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;								//アクセスをクランプするミップマップの範囲の下限
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;				//アクセスをクランプするミップマップの範囲の上限

	//ポイントサンプラーステート(粗い)
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	//テクスチャをサンプリングするときに使用するフィルタリング方法
	//ポイントフィルタとしてサンプラーステートを生成
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ポイントサンプラーステート(滑らか)
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//テクスチャをサンプリングするときに使用するフィルタリング方法
	//リニアフィルタとしてサンプラーステートを生成
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//アニソトロピックサンプラーステート(滑らか3D)
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;			//テクスチャをサンプリングするときに使用するフィルタリング方法
	//アニソトロピックとしてサンプラーステートを生成
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

void GraphicsManager::SettingRenderContext(std::function<void(RenderContext&)> SettingCallback)
{
	SettingCallback(renderContext);
}