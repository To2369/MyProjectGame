#pragma once

#include "Graphics/Sprite.h"
#include "Graphics/SpriteBatch.h"
#include "Scene.h"
#include "CameraController.h"
// ローディングシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

	// シーン定数
	struct Scene_constants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};
	// シーン定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
	Scene* nextScene = nullptr;
	float	angle = 0.0f;
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<SpriteBatch> sprite_batches[8];
	const wchar_t* filename[8] = {
		L".\\Data\\Fonts\\font4.png",
		L".\\Data\\Sprite\\player-sprites.png",
	 L".\\Data\\Sprite\\vice.png" };
	Camera* camera;
	std::unique_ptr<CameraController> cameraCtrl;


};
