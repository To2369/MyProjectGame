#include "StraightBullet.h"
#include "Graphics/Graphics.h"
StraightBullet::StraightBullet(BulletManager* manager) : Bullet(manager)
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//SpikeBall//SpikeBall.fbx");
    const float scale_fcator = 1.0f;	// ���f�����傫���̂ŃX�P�[������
    scale = { scale_fcator, scale_fcator, scale_fcator };
}

StraightBullet::~StraightBullet()
{
}

void StraightBullet::Update(float elapsedTime)
{
    // �o�ߎ��Ԃ������Ă���
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // ���Ԃ� 0 �ȉ��ɂȂ����玩����j��
        Destroy();
    }
    // �ړ�
    float speed = this->speed * elapsedTime;

    // �ʒu += ���� * ����
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    // ���[���h�s��̍X�V
    UpdateTransform();
}

// �`�揈��
void StraightBullet::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//����
void StraightBullet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}