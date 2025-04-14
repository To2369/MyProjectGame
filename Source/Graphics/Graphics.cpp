#include "../misc.h"
#include"Graphics.h"
#include "Buffer.h"

#include "MaskShader.h"
// 初期化処理
void Graphics::Initialize(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };
	{
		UINT createDeviceFlags{ 0 };
#ifdef _DEBUG
		createDeviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
		// Device,Contextの能力レベルの設定
		// DirectX11の基準を完全に満たしたGPU上で動作するレベルに設定
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			{D3D_FEATURE_LEVEL_11_0}
		};

		// IDXGISwapChain の設定を行うための構造体。SwapChain 作成するために必要な設定を行う
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		{
			swapChainDesc.BufferCount = 1;								// スワップチェインのバッファ数
			swapChainDesc.BufferDesc.Width = screenWidth;					// バッファの幅
			swapChainDesc.BufferDesc.Height = screenHeight;				// バッファの高さ
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バッファのカラーフォーマットの指定
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレートの分母
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分子
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用方法の指定
			swapChainDesc.OutputWindow = hwnd;//出力先ウィンドウバンドル
			// バックバッファのマルチサンプリングに関する設定
			// マルチサンプリングを使用しない場合は Count に１、Quality に０を指定。
			swapChainDesc.SampleDesc.Count = 1;							// 1ピクセルの色を決めるサンプリング数(未使用は１)
			swapChainDesc.SampleDesc.Quality = 0;							// サンプリングの品質(精度)（未使用は０）
			swapChainDesc.Windowed = !FALSE;								// ウィンドウモード　or　フルスクリーンモード
		}

		D3D_FEATURE_LEVEL featureLevel;

		// デバイス生成とスワップチェーン作成を行う
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,									// ビデオアダプタの指定(nullは規定)
			D3D_DRIVER_TYPE_HARDWARE,				// ドライバタイプ
			NULL,									// D3D_DRIVER_TYPE_SOFTWARE指定時に使用
			createDeviceFlags,					// フラグ指定
			featureLevels,							// D3D_FEATURE_LEVEL指定で自分で定義した配列
			1,										// 上のD3D_FEATURE_LEVEL配列の要素数
			D3D11_SDK_VERSION,						// SDKバージョン
			&swapChainDesc,						// DXGI_SWAP_CHAIN_DESC
			swapChain.GetAddressOf(),				// 関数成功時のSwapChainの出力先
			device.GetAddressOf(),					// 関数成功時のDeviceの出力先
			&featureLevel,							// 成功したD3D_FEATURE_LEVELの出力先
			dc.GetAddressOf());		// 関数成功時のContextの出力先
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// レンダーターゲットビューの作成
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapChain->GetBuffer(
			0,											// バッファのインデックス(基本は０)
			__uuidof(ID3D11Texture2D),					// 取得するバッファのインターフェースID
			reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));	// バッファの取得先、voidポインタでアドレスで取得する
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		// バックバッファを設定してレンダーターゲットビューを作成
		hr = device->CreateRenderTargetView(
			backBuffer.Get(),						// 作成するバッファのリソース
			NULL,									// 作成するViewの設定内容データの指定(nullptrでデフォルト設定)
			&renderTargetView);					// 作成されたRenderTargetViewを格納するためのポインタのアドレス
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// 深度ステンシルビューの作成
	{
		// ID3D11Texture2D の設定を行うための構造体。ID3D11Texture2D に深度ステンシル用の設定を行う
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = screenWidth;						// テクスチャの幅
		texture2dDesc.Height = screenHeight;					// テクスチャの高さ
		texture2dDesc.MipLevels = 1;							// ミップマップのレベル指定
		texture2dDesc.ArraySize = 1;							// テクスチャ配列のサイズ指定
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// テクスチャのフォーマット(DXGI_FORMAT_D24_UNORM_S8_UINTを使うとDepth24bit,Stencil8bitとなる)
		// マルチサンプリングを使用しない場合は Count に１、Quality に０を指定。
		texture2dDesc.SampleDesc.Count = 1;					// 1ピクセルの色を決めるサンプリング数(未使用は１)
		texture2dDesc.SampleDesc.Quality = 0;					// サンプリングの品質(精度)（未使用は０）
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;				// テクスチャの使用方法
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Bind設定,D3D11_BIND_DEPTH_STENCILを指定
		texture2dDesc.CPUAccessFlags = 0;						// CPUからの読み書きを不可
		texture2dDesc.MiscFlags = 0;							// リソースオプションのフラグ
		hr = device->CreateTexture2D(
			&texture2dDesc,							// 作成するテクスチャの詳細情報
			NULL,										// テクスチャの情報に付与するデータを指定する、読み込んだ画像データをせていしたりする
			depthStencilBuffer.GetAddressOf());		// 作成されたTextureを格納するためのポインタのアドレス
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		// ID3D11DepthStencilView の設定を行うための構造体。深度ステンシルビューを作成するために必要な設定を行う
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		// リソーステータの形式
		depthStencilViewDesc.Format = texture2dDesc.Format;					// Viewのフォーマット
		// 深度ステンシルへのアクセス方法
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// テクスチャの種類(ここで設定した値によってD3D11_TEX2D_DSVが決める)
		depthStencilViewDesc.Texture2D.MipSlice = 0;							// 最初に使用するミップマップのレベルを指定 				
		// リソースデータにアクセスするための深度ステンシルビューを作成			
		hr = device->CreateDepthStencilView(
			depthStencilBuffer.Get(),					// View作成に必要なリソース
			&depthStencilViewDesc,					// DSVに関する設定を行ったデータ
			depthStencilView.GetAddressOf());			// ID3D11DepthStencilViewの出力先
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ビューポートの設定
	{
		viewport.TopLeftX = 0;									// ビューポート左側のXの位置
		viewport.TopLeftY = 0;									// ビューポート上部のY位置
		viewport.Width = static_cast<float>(screenWidth);		// ビューポートの幅
		viewport.Height = static_cast<float>(screenHeight);		// ビューポートの高さ
		viewport.MinDepth = 0.0f;								// ビューポートの最小深度。0~1の範囲
		viewport.MaxDepth = 1.0f;								// ビューポートの最大深度。0~1の範囲
		dc->RSSetViewports(1, &viewport);
	}

	// レンダーステート生成
	renderState = std::make_unique<RenderState>(device.Get());

	{
		light = std::make_unique<Light>(device.Get());
	}

	{
		spriteShaders[static_cast<int>(SpriteShaderId::Mask)] = std::make_unique<MaskShader>(device.Get());
	}
	// レンダラ
	{
		debugPrimitive = std::make_unique<DebugPrimitive>(device.Get());
		lineRenderer= std::make_unique<LineRenderer>(device.Get(), 1024);
	}
}

// クリア
void Graphics::Clear(float r, float g, float b, float a)
{
	float color[4]{ r, g, b, a };
	dc->ClearRenderTargetView(renderTargetView.Get(), color);
	dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// レンダーターゲット設定
void Graphics::SetRenderTargets()
{
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

// 画面表示
void Graphics::Present(UINT syncInterval)
{
	swapChain->Present(syncInterval, 0);
}
