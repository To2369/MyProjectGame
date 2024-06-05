#pragma once
#include"Scene.h"
#include"Graphics/Sprite.h"
#include"Graphics/Sprite_batch.h"
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
    std::unique_ptr<Sprite_batch> sprite_batches[8];
    const wchar_t* filename[8] = { 
        L".\\Data\\Fonts\\font4.png",
        L".\\Data\\Sprite\\player-sprites.png",
     L".\\Data\\Sprite\\vice.png" };
};