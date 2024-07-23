#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include"RenderState.h"

class Graphics
{
private:
	Graphics() = default;
	~Graphics() = default;

public:
	static Graphics* Instance()
	{
		static Graphics ins;
		return &ins;
	}

	// 初期化処理
	void Initialize(HWND hwnd);

	// クリア
	void Clear(float r, float g, float b, float a);

	// レンダーターゲット設定
	void SetRenderTargets();

	// 画面表示
	void Present(UINT syncInterval);

	// デバイス取得
	ID3D11Device* GetDevice() const { return device.Get(); }

	// デバイスコンテキスト取得
	ID3D11DeviceContext* GetDeviceContext() const { return immediate_context.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swap_chain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return render_target_view.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depth_stencil_view.Get(); }

	//// シェーダー取得
	//Shader* GetShader() const { return shader.get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	//// デバッグレンダラ取得
	//DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

	//// ラインレンダラ取得
	//LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	//// ImGuiレンダラ取得
	//ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }


	// レンダーステート取得
	RenderState* GetRenderState() { return renderState.get(); }

	std::mutex& GetMutex() { return mutex; }

private:
	float	screenWidth = 0;
	float	screenHeight = 0;

	// DirectX11で利用する様々なリソースの作成
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	// 様々な描画命令をGPUに伝える
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	immediate_context;

	// キャンパスに描いた画を額(ウィンドウ)に入れる
	Microsoft::WRL::ComPtr<IDXGISwapChain>	swap_chain;

	// 色を書き込みキャンパス
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	depth_stencil_buffer;

	// 奥行情報を書き込みキャンパス
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;

	D3D11_VIEWPORT viewport{};

	std::mutex mutex;

	std::unique_ptr<RenderState> renderState;
};