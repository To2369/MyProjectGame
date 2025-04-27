#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// �G�t�F�N�g�Ǘ�
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

    // ����������
    void Initialize();

    // �I������
    void Finalize() {};

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection);

    // Effekseer �Ǘ��̎擾
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerMgr; }

private:
    // Effekseer �Ǘ�
    Effekseer::ManagerRef effekseerMgr;
    // Effekseer �`�����
    EffekseerRenderer::RendererRef effekseerRenderer;
};