#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include"RenderState.h"
#include "DebugRenderer.h"
#include "LineRenderer.h"
#include "DebugPrimitive.h"
#include "Shader.h"
#include "Light.h"

enum class ModelShaderId
{
	Default,
	Phong,
	Toon,
	ShadowmapCaster,

	Max
};

enum class SpriteShaderId
{
	Default,
	UVScroll,
	Mask,
	ColorGrading,
	GaussianBlur,
	LuminanceExtraction,
	Finalpass,

	Max
};

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

	// モデルシェーダー取得
	ModelShader* GetShader(ModelShaderId id) const { return modelShaders[static_cast<int>(id)].get(); }

	// スプライトシェーダー取得
	SpriteShader* GetShader(SpriteShaderId id) const { return spriteShaders[static_cast<int>(id)].get(); }

	// スクリーン幅取得
	float GetScreenWidth() const { return screenWidth; }

	// スクリーン高さ取得
	float GetScreenHeight() const { return screenHeight; }

	//// デバッグレンダラ取得
	DebugRenderer* GetDebugRenderer() const { return debug_renderer.get(); }

	// デバッグプリミティブ取得
	DebugPrimitive* GetDebugPrimitive() const { return debugPrimitive.get(); }

	// ラインレンダラ取得
	LineRenderer* GetLineRenderer() const { return line_renderer.get(); }

	//Light* GetLight()const { return light.get(); }
	//// ImGuiレンダラ取得
	//ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }


	// レンダーステート取得
	RenderState* GetRenderState() { return render_state.get(); }

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

	std::unique_ptr<RenderState> render_state;

	std::unique_ptr<DebugRenderer>					debug_renderer;
	
	std::unique_ptr<DebugPrimitive>					debugPrimitive;

	std::unique_ptr<LineRenderer>					line_renderer;

	std::unique_ptr<Light> light;
	std::unique_ptr<ModelShader>					modelShaders[static_cast<int>(ModelShaderId::Max)];
	std::unique_ptr<SpriteShader>					spriteShaders[static_cast<int>(SpriteShaderId::Max)];
};