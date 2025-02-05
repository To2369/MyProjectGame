#pragma once

#include "Graphics/Sprite.h"
#include "Graphics/SpriteBatch.h"
#include "Scene.h"
#include "CameraController.h"
// ���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	// ���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

	// �V�[���萔
	struct Scene_constants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};
	// �V�[���萔�o�b�t�@
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
