#include "EnemyHuman.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
EnemyHuman::EnemyHuman()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\pl\\Character1.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // ��
	height = 1.0f;  // ����

    maxHealth = 1000;
    health = maxHealth;
    position.y = 5.0f;
}

void EnemyHuman::Update(float elapsedTime)
{
    //Moving(elapsedTime, 0.5f);
	// ���x�����X�V
	UpdateVelocity(elapsedTime);

	// ���G���Ԃ̍X�V
	UpdateInvincibleTimer(elapsedTime);
    //Lockon();
    
    {
       
    }
    
    UpdateAnimation(elapsedTime);
    UpdateStatus(elapsedTime);
	UpdateTransform();
}

void EnemyHuman::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, color);
}

void EnemyHuman::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Slime", nullptr, ImGuiWindowFlags_None))
    {
        //�g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //�ʒu
            ImGui::InputFloat3("Position", &ppos.x);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
#endif

}

// ���S�����Ƃ��ɌĂ΂��
void EnemyHuman::OnDead()
{
	Destroy();
}

void EnemyHuman::Lockon()
{

    Player* oldLockonPl = lockonPlayer;
    lockonPlayer = nullptr;
    DirectX::XMVECTOR p, t, v;
    Player*pl = Player::Instance();
    {
        p = DirectX::XMLoadFloat3(&position);
        t = DirectX::XMLoadFloat3(&pl->GetPosition());
        v = DirectX::XMVectorSubtract(t, p);

        lockonPlayer = pl;
        DirectX::XMStoreFloat3(&lockDirection, DirectX::XMVector3Normalize(v));
    }
}

void EnemyHuman::Moving(float elapsedTime, float speedRate)
{
    //�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx = vx / dist;
    vz = vz / dist;

    //�ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
    
}

//�^�[�Q�b�g�ʒu�������_���ݒ�
void EnemyHuman::SetRandomTargetPosition()
{
    float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Mathf::RandomRange(0.0f, territoryRange);
    targetPosition.x = territoryOrigin.x + sinf(theta) + range;
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

//�v���C���[���G
bool EnemyHuman::SearchPlayer()
{
    //�v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    const DirectX::XMFLOAT3& playerPosition = Player::Instance()->GetPosition();
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);
        //�P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;
        //�O���x�N�g��
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, front);
        float frontX = sinf(f.y);
        float frontZ = cosf(f.y);
        //2�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}