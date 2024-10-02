#include "ArtsSpiritExplosion.h"
#include "Graphics/Graphics.h"

ArtsSpiritExplosion::ArtsSpiritExplosion(ArtsManager* manager) : Arts(manager)
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Sword//Sword.fbx");
    const float scale_fcator = 0.0f;	// ���f�����傫���̂ŃX�P�[������
    scale = { scale_fcator, scale_fcator, scale_fcator };
    useSpiritEnergy = 300;
    radius = 2;
    damage = 1;
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
    model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//����
void ArtsSpiritExplosion::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}