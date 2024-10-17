#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"

ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    height = 1.0f;

    prim = std::make_unique<GeometricCapsule>(Graphics::Instance()->GetDevice(), height, DirectX::XMFLOAT3{ radius,radius,radius }, 6, 3, 3, DirectX::XMFLOAT3{ angle.x,angle.y,angle.z });
    const float scale_fcator = 1.0f;	// ���f�����傫���̂ŃX�P�[������
    scale = { scale_fcator, scale_fcator, scale_fcator };
    position = { 1,5,1 };
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
    //angle.x = DirectX::XMConvertToRadians(90);
    // �ړ�
    float speed = this->speed * elapsedTime;
    // �ʒu += ���� * ����
    /*position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;*/
    // ���[���h�s��̍X�V
    UpdateTransform();
}

// �`�揈��
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    prim->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//����
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}