#include "framework.h"

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

bool framework::initialize()
{
	//デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags != D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
	//Device,Contextの能力レベルの設定
	// DirectX11の基準を完全に満たしたGPU上で動作するレベルに設定
	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

	// IDXGISwapChain の設定を行うための構造体。SwapChain 作成するために必要な設定を行う
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;								//スワップチェインのバッファ数
	swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;				//バッファの幅
	swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;				//バッファの高さ
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//バッファのカラーフォーマットの指定
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;			//リフレッシュレートの分母
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;			//リフレッシュレートの分子
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バッファの使用方法の指定
	swap_chain_desc.OutputWindow = hwnd;//出力先ウィンドウバンドル
	// バックバッファのマルチサンプリングに関する設定
	// マルチサンプリングを使用しない場合は Count に１、Quality に０を指定。
	swap_chain_desc.SampleDesc.Count = 1;							//1ピクセルの色を決めるサンプリング数(未使用は１)
	swap_chain_desc.SampleDesc.Quality = 0;							//サンプリングの品質(精度)（未使用は０）
	swap_chain_desc.Windowed = !FULLSCREEN;							//ウィンドウモード　or　フルスクリーンモード

	//デバイス生成とスワップチェーン作成を行う
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//ビデオアダプタの指定(nullは規定)
		D3D_DRIVER_TYPE_HARDWARE,	//ドライバタイプ
		NULL,						//D3D_DRIVER_TYPE_SOFTWARE指定時に使用
		create_device_flags,		//フラグ指定
		&feature_levels,			//D3D_FEATURE_LEVEL指定で自分で定義した配列
		1,							//上のD3D_FEATURE_LEVEL配列の要素数
		D3D11_SDK_VERSION,			//SDKバージョン
		&swap_chain_desc,			//DXGI_SWAP_CHAIN_DESC
		&swap_chain,				//関数成功時のSwapChainの出力先
		&device,					//関数成功時のDeviceの出力先
		NULL,						//成功したD3D_FEATURE_LEVELの出力先
		&immediate_context);		//関数成功時のContextの出力先
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//レンダーターゲットビューの作成
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(
		0,											//バッファのインデックス(基本は０)
		__uuidof(ID3D11Texture2D),					//取得するバッファのインターフェースID
		reinterpret_cast<LPVOID*>(&back_buffer));	//バッファの取得先、voidポインタでアドレスで取得する
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//バックバッファを設定してレンダーターゲットビューを作成
	hr = device->CreateRenderTargetView(
		back_buffer,							//作成するバッファのリソース
		NULL,									//作成するViewの設定内容データの指定(nullptrでデフォルト設定)
		&render_target_view);					//作成されたRenderTargetViewを格納するためのポインタのアドレス
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();

	//深度ステンシルビューの作成
	// ID3D11Texture2D の設定を行うための構造体。ID3D11Texture2D に深度ステンシル用の設定を行う
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = SCREEN_WIDTH;					//テクスチャの幅
	texture2d_desc.Height = SCREEN_HEIGHT;					//テクスチャの高さ
	texture2d_desc.MipLevels = 1;							//ミップマップのレベル指定
	texture2d_desc.ArraySize = 1;							//テクスチャ配列のサイズ指定
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//テクスチャのフォーマット(DXGI_FORMAT_D24_UNORM_S8_UINTを使うとDepth24bit,Stencil8bitとなる)
	// マルチサンプリングを使用しない場合は Count に１、Quality に０を指定。
	texture2d_desc.SampleDesc.Count = 1;					//1ピクセルの色を決めるサンプリング数(未使用は１)
	texture2d_desc.SampleDesc.Quality = 0;					//サンプリングの品質(精度)（未使用は０）
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;				//テクスチャの使用方法
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	//Bind設定,D3D11_BIND_DEPTH_STENCILを指定
	texture2d_desc.CPUAccessFlags = 0;						//CPUからの読み書きを不可
	texture2d_desc.MiscFlags = 0;							//リソースオプションのフラグ
	hr = device->CreateTexture2D(
		&texture2d_desc,			//作成するテクスチャの詳細情報
		NULL,						//テクスチャの情報に付与するデータを指定する、読み込んだ画像データをせていしたりする
		&depth_stencil_buffer);		//作成されたTextureを格納するためのポインタのアドレス
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ID3D11DepthStencilView の設定を行うための構造体。深度ステンシルビューを作成するために必要な設定を行う
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	//リソーステータの形式
	depth_stencil_view_desc.Format = texture2d_desc.Format;					//Viewのフォーマット
	// 深度ステンシルへのアクセス方法
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	//テクスチャの種類(ここで設定した値によってD3D11_TEX2D_DSVが決める)
	depth_stencil_view_desc.Texture2D.MipSlice = 0;							// 最初に使用するミップマップのレベルを指定 				
	// リソースデータにアクセスするための深度ステンシルビューを作成			
	hr = device->CreateDepthStencilView(
		depth_stencil_buffer,		//View作成に必要なリソース
		&depth_stencil_view_desc,	//DSVに関する設定を行ったデータ
		&depth_stencil_view);		//ID3D11DepthStencilViewの出力先
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ビューポートの設定
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;									//ビューポート左側のXの位置
	viewport.TopLeftY = 0;									//ビューポート上部のY位置
	viewport.Width = static_cast<float>(SCREEN_WIDTH);		//ビューポートの幅
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);	//ビューポートの高さ
	viewport.MinDepth = 0.0f;								//ビューポートの最小深度。0~1の範囲
	viewport.MaxDepth = 1.0f;								//ビューポートの最大深度。0~1の範囲
	immediate_context->RSSetViewports(1, &viewport);
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");


	ImGui::End();
#endif
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,0.2f };
	immediate_context->ClearRenderTargetView(render_target_view, color);
	immediate_context->ClearDepthStencilView(depth_stencil_view,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);



#ifdef USE_IMGUI
		ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_intervel{ 0 };
	swap_chain->Present(sync_intervel, 0);

}

bool framework::uninitialize()
{
	device->Release();
	immediate_context->Release();
	render_target_view->Release();
	swap_chain->Release();
	render_target_view->Release();
	depth_stencil_view->Release();
	return true;
}

framework::~framework()
{

}