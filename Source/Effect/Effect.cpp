#include "Effect.h"
#include "EffectManager.h"

// コンストラクタ
Effect::Effect(const char* filename)
{
    // ファイル名は UTF-16 しか対応していないので文字コードを変換
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    // Effekseer のエフェクトを読み込み
    effekseerEffect = Effekseer::Effect::Create(effekseerMgr, (EFK_CHAR*)utf16Filename);
}

// 再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3* position, float scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    // 再生と位置設定
    Effekseer::Handle handle = effekseerMgr->Play(effekseerEffect, position->x, position->y, position->z);

    // 拡大率の変更
    effekseerMgr->SetScale(handle, scale, scale, scale);

    return handle;
}

// 停止
void Effect::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->StopEffect(handle);
}

// 位置設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->SetLocation(handle, position->x, position->y, position->z);
}

// 拡大縮小設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->SetScale(handle, scale->x, scale->y, scale->z);
}