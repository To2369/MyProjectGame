#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクト管理
class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}

public:
    static EffectManager& Instance()
    {
        static EffectManager ins;
        return ins;
    }

    // 初期化処理
    void Initialize();

    // 終了処理
    void Finalize() {};

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection);

    // Effekseer 管理の取得
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerMgr; }

private:
    // Effekseer 管理
    Effekseer::ManagerRef effekseerMgr;
    // Effekseer 描画周り
    EffekseerRenderer::RendererRef effekseerRenderer;
};