#include "HomingBullet.h"
#include "Graphics/Graphics.h"

// �R���X�g���N�^
HomingBullet::HomingBullet(BulletManager* manager)
    : Bullet(manager)
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Sword//Sword.fbx");

    const float scale_fcator = 3.0f;
    scale = { scale_fcator, scale_fcator, scale_fcator };
}

// �f�X�g���N�^
HomingBullet::~HomingBullet()
{
  
}

// �X�V����
void HomingBullet::Update(float elapsedTime)
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

    // �^�[�Q�b�g�Ƃ̋������`�F�b�N
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(targetVec, posVec);

    // �[���x�N�g���łȂ��Ȃ���񏈗�
    DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
    if (lengthSq > 0.00001f)
    {
        // �^�[�Q�b�g�܂ł̃x�N�g��������ɕϊ�
        vec = DirectX::XMVector3Normalize(vec);

        // �i�s�����x�N�g���ƃ^�[�Q�b�g�ւ̕����x�N�g������ςŊp�x���v�Z
        DirectX::XMVECTOR directVec = DirectX::XMLoadFloat3(&direction);
        DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(directVec, vec);

        float dot;
        DirectX::XMStoreFloat(&dot, dotVec);

        // �^�[�Q�b�g�ւ̃x�N�g���ɋ߂Â��قǏ���������
        float rot = 1.0f - dot;
        if (rot > turnSpeed)
        {
            rot = turnSpeed;
        }

        // ��]�p�x������Ȃ��]�������s��
        if (fabsf(rot) > 0.0001f)
        {
            // �i�s�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g�������]����ׂ������v�Z
            DirectX::XMVECTOR axisVec = DirectX::XMVector3Cross(directVec, vec);

            // ��]���Ɖ�]�ʂ����]�s����v�Z
            DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationAxis(axisVec, rot);

            // ���݂̍s��ɉ�]�𔽉f������
            DirectX::XMMATRIX transformMat = DirectX::XMLoadFloat4x4(&transform);
            transformMat = DirectX::XMMatrixMultiply(transformMat, rotationMat);

            // ��]������̑O���������o���A
            directVec = DirectX::XMVector3Normalize(transformMat.r[2]);
            // �i�s�����x�N�g���Ƃ��Đݒ肷��
            DirectX::XMStoreFloat3(&direction, directVec);
        
        }
    }

    // ���[���h�s��̍X�V
    UpdateTransform();
}


// �`�揈��
void HomingBullet::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

//����
void  HomingBullet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}