#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include"RenderState.h"
#include "LineRenderer.h"
#include "DebugPrimitive.h"
#include "Shader.h"
#include "Light.h"

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
	ID3D11DeviceContext* GetDeviceContext() const { return dc.Get(); }

	// スワップチェーン取得
	IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

	// レンダーターゲットビュー取得
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

	// デプスステンシルビュー取得
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	// デバッグプリミティブ取得
	DebugPrimitive* GetDebugPrimitive() const { return debugPrimitive.get(); }

	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

	Light* GetLight()const { return light.get(); }
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
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	dc;

	// キャンパスに描いた画を額(ウィンドウ)に入れる
	Microsoft::WRL::ComPtr<IDXGISwapChain>	swapChain;

	// 色を書き込みキャンパス
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	depthStencilBuffer;

	// 奥行情報を書き込みキャンパス
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	D3D11_VIEWPORT viewport{};

	std::mutex mutex;

	std::unique_ptr<RenderState> renderState;
	
	std::unique_ptr<DebugPrimitive>					debugPrimitive;

	std::unique_ptr<LineRenderer>					lineRenderer;

	std::unique_ptr<Light> light;
};