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
    // シーン定数
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
    };
    // シーン定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};