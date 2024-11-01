#include"FrameBuffer.h"
#include"..\misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr{ S_OK };
	// オフスクリーンレンダリング用の描画先のテクスチャを作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	{
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// オフスクリーンレンダリング用の描画先のレンダーターゲットビューを生成
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
	{
		// 描画先のテクスチャとして上記で作成したテクスチャを設定
		render_target_view_desc.Format = texture2d_desc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc,
			render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 上記で作成したレンダーターゲットビュー用のテクスチャのシェーダーリソースビューを作成
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	{
		shader_resource_view_desc.Format = texture2d_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc,
			shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// オフスクリーンレンダリング用の描画先の深度ステンシル用テクスチャを作成
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
	{
		texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// オフスクリーンレンダリング用の描画先の深度ステンシルビューを作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	{
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc,
			depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 上記で作成した深度ステンシルビュー用のテクスチャのシェーダーリソースビューを作成
	{
		shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc,
			shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::ShaderResourceView)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// オフスクリーンレンダリング用のビューポートのサイズを設定
	{
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

// クリア処理
void FrameBuffer::Clear(ID3D11DeviceContext* immediate_context,
	float r, float g, float b, float a, float depth)
{
	float color[4]{ r,g,b,a };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

// オフスクリーンレンダリングの設定に連結
// 今までの描画情報をキャッシュに退避させオフスクリーン書き込み用にターゲットを設定
void FrameBuffer::Activate(ID3D11DeviceContext* immediate_context)
{
	// 現在指定中のビューポートとレンダーターゲットと深度ステンシルの情報を退避
	viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediate_context->RSGetViewports(&viewport_count, cached_viewports);
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(),
		cached_depth_stencil_view.ReleaseAndGetAddressOf());

	// オフスクリーン用にこのクラスで作成したビューポートとレンダーターゲットと深度ステンシルを設定
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(),
		depth_stencil_view.Get());
}

// オフスクリーンの設定から連結を解除
void FrameBuffer::Deactivate(ID3D11DeviceContext* immediate_context)
{
	// 退避させていたビューポートとレンダーターゲットと深度ステンシルの情報を再設定
	immediate_context->RSSetViewports(viewport_count, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(),
		cached_depth_stencil_view.Get());
}