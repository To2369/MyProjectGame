#pragma once
#include"Scene.h"
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
//シーンマネージャー
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}
public:
    //唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager ins;
        return ins;
    }

    //更新処理
    void Update(float elapsedTime);

    //描画更新
    void Render();

    //シーンクリア
    void Clear();

    //シーン切り替え
    void ChangeScene(Scene* scene);
private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};