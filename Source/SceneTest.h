#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/SpriteBatch.h"
#include"Camera.h"
#include"Graphics/GeometricPrimitive.h"
#include"Graphics/StaticMesh.h"
#include"CameraController.h"
#include"Graphics/Model.h"
#include"Graphics/FrameBuffer.h"
#include"Graphics/FullScreenQuad.h"
//タイトルシーン
class SceneTest :public Scene
{
public:
    SceneTest() {}
    ~SceneTest()override {}

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
        DirectX::XMFLOAT4		camera_position;
    };
    // シーン定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
private:
    std::unique_ptr<Sprite> spr[8];
    std::unique_ptr<SpriteBatch> sprite_batches[8];
    std::unique_ptr<GeometricPrimitive> geometric_primitives[8];
    std::unique_ptr<StaticMesh> static_mesh[8];
    std::unique_ptr<Model> model[8];
    std::unique_ptr<FrameBuffer> framebuffers[8];
    std::unique_ptr<FullScreenQuad> bit_block_transfer;
    const wchar_t* filename[8] = {
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };

    const wchar_t* modelfilename[8] = {
        L".\\Data\\resources\\Rock\\Rock.obj",
        L".\\Data\\resources\\cube.obj",
           L".\\Data\\resources\\Mr.Incredible\\Mr.Incredible.obj"
    };
    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;

private:
    // カメラの位置
    DirectX::XMFLOAT4 camera_position{ 0.0f,0.0f,-10.0f,1.0f };

    //ライトの方向
    DirectX::XMFLOAT4 light_direction{ 0.0f,0.0f,1.0f,0.0f };

    DirectX::XMFLOAT3 scaling{ 1,1,1 };
    DirectX::XMFLOAT3 rotation{ 0,0,0 };
    DirectX::XMFLOAT3 tramslation{ 0,0,0 };

    //マテリアルカラー
    DirectX::XMFLOAT4 material_color{ 1,1,1,1 };

    float elapsedTime_ = 0;

    float factor[8] = {1.5f,300,0.5f};
};