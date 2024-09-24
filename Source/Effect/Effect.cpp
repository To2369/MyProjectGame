#include "Effect.h"
#include "EffectManager.h"

// �R���X�g���N�^
Effect::Effect(const char* filename)
{
    // �t�@�C������ UTF-16 �����Ή����Ă��Ȃ��̂ŕ����R�[�h��ϊ�
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    // Effekseer �̃G�t�F�N�g��ǂݍ���
    effekseerEffect = Effekseer::Effect::Create(effekseerMgr, (EFK_CHAR*)utf16Filename);
}

// �Đ�
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3* position, float scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    // �Đ��ƈʒu�ݒ�
    Effekseer::Handle handle = effekseerMgr->Play(effekseerEffect, position->x, position->y, position->z);

    // �g�嗦�̕ύX
    effekseerMgr->SetScale(handle, scale, scale, scale);

    return handle;
}

// ��~
void Effect::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->StopEffect(handle);
}

// �ʒu�ݒ�
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->SetLocation(handle, position->x, position->y, position->z);
}

// �g��k���ݒ�
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::Instance().GetEffekseer();

    effekseerMgr->SetScale(handle, scale->x, scale->y, scale->z);
}