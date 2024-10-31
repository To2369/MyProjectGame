#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/FrameBuffer.h"
#include "Graphics/FullScreenQuad.h"
#include "Graphics/FrameBuffer.h"
#include <memory>

struct SceneViewData
{
	std::shared_ptr<FrameBuffer>	fb;
};

// ポストプロセス用のレンダラー
class PostprocessingRenderer
{
public:
	PostprocessingRenderer();
	~PostprocessingRenderer();

	//	描画
	void	Render(ID3D11DeviceContext* deviceContext);

	//	シーン情報の設定
	void	SetSceneData(SceneViewData sceneViewData);

	//	デバッグ情報の表示
	void	DrawDebugGUI();

private:
	std::unique_ptr<FrameBuffer> framebuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
};
