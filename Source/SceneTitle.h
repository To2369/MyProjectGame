#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/SpriteBatch.h"
#include"Camera.h"
#include"Graphics/GeometricPrimitive.h"
#include"CameraController.h"
//タイトルシーン
class SceneTitle :public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle()override {}

    //初期化
    void Initialize()override;

    //終了化
    void Finalize()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画更新
    void Render()override;
public:
    // シーン定数
    struct Scene_constants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
    };
    // シーン定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
    std::unique_ptr<Sprite> sprite;
    std::unique_ptr<Sprite> push;
    std::unique_ptr<SpriteBatch> sprite_batches[8];
    const wchar_t* filename[8] = { 
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };
    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;
};