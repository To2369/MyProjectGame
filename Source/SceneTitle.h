#pragma once
#include"Scene.h"
#include"DeviceManager.h"
#include"Graphics/Sprite.h"
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
private:
    std::unique_ptr<Sprite> spr[8];
    const wchar_t* filename[8] = { L".\\Data\\Sprite\\cyberpunk.jpg", L".\\Data\\Sprite\\player-sprites.png" };
};