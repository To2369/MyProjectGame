#include "EnemyHuman.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
EnemyHuman::EnemyHuman()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\pl\\Character1.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // 幅
	height = 1.0f;  // 高さ

    maxHealth = 1000;
    health = maxHealth;
    position.y = 5.0f;
}

void EnemyHuman::Update(float elapsedTime)
{
    //Moving(elapsedTime, 0.5f);
	// 速度処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間の更新
	UpdateInvincibleTimer(elapsedTime);
    //Lockon();
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        Test(elapsedTime);
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
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &ppos.x);
            //スケール
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

// 死亡したときに呼ばれる
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
    //ターゲット方向への進行ベクトルを算出
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx = vx / dist;
    vz = vz / dist;

    //移動処理
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
    
}

//ターゲット位置をランダム設定
void EnemyHuman::SetRandomTargetPosition()
{
    float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Mathf::RandomRange(0.0f, territoryRange);
    targetPosition.x = territoryOrigin.x + sinf(theta) + range;
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

//プレイヤー索敵
bool EnemyHuman::SearchPlayer()
{
    //プレイヤーとの高低差を考慮して3Dでの距離判定をする
    const DirectX::XMFLOAT3& playerPosition = Player::Instance()->GetPosition();
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);
        //単位ベクトル化
        vx /= distXZ;
        vz /= distXZ;
        //前方ベクトル
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, front);
        float frontX = sinf(f.y);
        float frontZ = cosf(f.y);
        //2つのベクトルの内積値で前後判定
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

//縄張り設定
void EnemyHuman::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

void EnemyHuman::Test(float elapsedTime)
{
    targetPosition = Player::Instance()->GetPosition();

    Moving(elapsedTime,1);
}