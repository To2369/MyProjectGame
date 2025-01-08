#include "ArtsSpiritExplosion.h"
#include "Graphics/Graphics.h"

ArtsSpiritExplosion::ArtsSpiritExplosion(ArtsManager* manager) : Arts(manager)
{
    useSpiritEnergy = 300;
    radius = 2;
    damage = 1;
    lifeTimer = 3.0f;
}

ArtsSpiritExplosion::~ArtsSpiritExplosion()
{
}

void ArtsSpiritExplosion::Update(float elapsedTime)
{
    // �o�ߎ��Ԃ������Ă���
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // ���Ԃ� 0 �ȉ��ɂȂ����玩����j��
        Destroy();
    }
    // ���[���h�s��̍X�V
    UpdateTransform();
}

// �`�揈��
void ArtsSpiritExplosion::Render(ID3D11DeviceContext* dc)
{
    //�Փ˔���p�̃f�o�b�O����`��
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawSphere(position, angle,radius, { 0,0,1,1 });
}

//����
void ArtsSpiritExplosion::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}