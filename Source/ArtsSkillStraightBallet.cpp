#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"
ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    height = 1;
    geoPrimitive =
        std::make_unique<CapsuleOneWay>(Graphics::Instance()->GetDevice(), height, radius, 12, 6, 6);
    damage = 1;
    useSkillEnergy = 100;
    direction = { 0,0,0 };
}

ArtsSkillStraightBallet::~ArtsSkillStraightBallet()
{
}

void ArtsSkillStraightBallet::Update(float elapsedTime)
{
    // �o�ߎ��Ԃ������Ă���
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // ���Ԃ� 0 �ȉ��ɂȂ����玩����j��
        Destroy();
    }
    currentCapsuleHeight = geoPrimitive->GetCurrentHeight();
    geoPrimitive->SetGrowthRate(growSpeed);
    geoPrimitive->Update(elapsedTime);
    // ���[���h�s��̍X�V
    UpdateTransform();
}

// �`�揈��
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    geoPrimitive->Render(dc, transform, { 1,0,0,1 });
}

//����
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}