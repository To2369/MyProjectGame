#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"

ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    position = { 3,0,3 };
    damage = 1;
    useSkillEnergy = 100;
    direction = { 0,1,0 };
    height =1;
    //angle.x = DirectX::XMConvertToRadians(90);
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
    height += 5 * elapsedTime;
    //angle.x = DirectX::XMConvertToRadians(90);
    // �ړ�
    //float speed = this->speed * elapsedTime;
    //// �ʒu += ���� * ����
    //position.x += direction.x * speed;
    //position.y += direction.y * speed;
    //position.z += direction.z * speed;
    // ���[���h�s��̍X�V
    UpdateTransform();
}

// �`�揈��
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    //�Փ˔���p�̃f�o�b�O����`��
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawCapsule(position, angle,radius,height, { 1, 0, 0, 1 });
}

//����
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}