#include "EnemyHuman.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
EnemyHuman::EnemyHuman()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\Enemy\\EnemyBoxer.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // 幅
	height = 1.0f;  // 高さ

    maxHealth = 1000;
    health = maxHealth;
    position.y = 5.0f;

    //徘徊ステートへ遷移
    TransitionWanderState();
}

void EnemyHuman::Update(float elapsedTime)
{
    //ステート毎更新処理
    switch (state)
    {
    case State::Wander:
        UpdateWanderState(elapsedTime);
        break;
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Pursuit:
        UpdatePursuitState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::IdleBattle:
        UpdateIdleBattleState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    }
	// 速度処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間の更新
	UpdateInvincibleTimer(elapsedTime);
    //Lockon();
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //Test(elapsedTime);
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
            ImGui::InputFloat3("Position", &position.x);
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

//目標地点へ移動
void EnemyHuman::MoveToTarget(float elapsedTime, float speedRate)
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


//徘徊ステートへの遷移
void EnemyHuman::TransitionWanderState()
{
    state = State::Wander;

    //目標地点設定
    SetRandomTargetPosition();

    //歩きアニメーション再生
    //model->PlayAnimation(Anim_WalkFWD, true);
}

//徘徊ステート更新処理
void EnemyHuman::UpdateWanderState(float elapsedTime)
{
    //目標地点までXZ平面での距離判定
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float distSq = vx * vx + vz * vz;
    if (distSq < radius * radius)
    {
        //待機ステートへ遷移
        TransitionIdleState();
    }

    //目標地点へ移動
    MoveToTarget(elapsedTime, 0.5f);

    //プレイヤー索敵
    if (SearchPlayer())
    {
        //見つかったら追跡ステートへ遷移
        TransitionPursuitState();
    }
}

//待機ステートへ遷移
void EnemyHuman::TransitionIdleState()
{
    state = State::Idle;

    //タイマーをランダム設定
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    //待機アニメーション再生
    //model->PlayAnimation(Anim_IdleNormal, true);
}

//待機ステート更新処理
void EnemyHuman::UpdateIdleState(float elapsedTime)
{
    //タイマー処理
    stateTimer -= elapsedTime;
    //プレイヤー索敵
    if (SearchPlayer())
    {
        //見つかったら追跡ステートへ遷移
        TransitionPursuitState();
        return;
    }
    if (stateTimer < 0.0f)
    {
        //徘徊ステートへ遷移
        TransitionWanderState();
    }
}

//追跡ステートへ遷移
void EnemyHuman::TransitionPursuitState()
{
    state = State::Pursuit;

    //数秒間追跡するタイマーをランダム設定
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    //歩きアニメーション再生
    //model->PlayAnimation(Anim_RunFWD, true);
}

//追跡ステート更新処理
void EnemyHuman::UpdatePursuitState(float elapsedTime)
{
    //目標地点をプレイヤー位置に設定
    targetPosition = Player::Instance()->GetPosition();

    //目標地点へ移動
    MoveToTarget(elapsedTime, 1.0f);

    //タイマー処理
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //待機ステートへ遷移
        TransitionIdleState();
    }

    //プレイヤーの近ずくと攻撃ステートへ遷移
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        //攻撃ステートへ遷移
        TransitionAttackState();
    }
}

//攻撃ステートへ遷移
void EnemyHuman::TransitionAttackState()
{
    state = State::Attack;

    //攻撃アニメーション再生
    //model->PlayAnimation(Anim_Attack1, false);
}

//攻撃ステート更新処理
void EnemyHuman::UpdateAttackState(float elapsedTime)
{
    //任意のアニメーション再生区間でのみ衝突判定処理をする
    //float animationTime = model->GetCurrentAnimationSeconds();
    //if (animationTime >= 0.1f && animationTime <= 0.35f)
    //{
    //    //目玉ノードとプレイヤーの衝突処理
    //    CollisionNodeVsPlayer("EyeBall", 0.3f);
    //}

    //攻撃アニメーションが終わったら戦闘待機ステートへ遷移
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
    MoveToTarget(elapsedTime, 0.0f);
}

//戦闘待機ステートへ遷移
void EnemyHuman::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    //数秒間待機するタイマーをランダム設定
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    //戦闘待機アニメーション再生
    //model->PlayAnimation(Anim_IdleBattle, true);
}

//戦闘待機ステート更新処理
void EnemyHuman::UpdateIdleBattleState(float elapsedTime)
{
    //目標地点をプレイヤー位置に設定
    targetPosition = Player::Instance()->GetPosition();

    //タイマー処理
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //プレイヤーが攻撃範囲にいた場合は攻撃ステートへ遷移
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            //攻撃ステートへ遷移
            TransitionAttackState();
        }
        else
        {
            //徘徊ステートへ遷移
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

//ダメージステートへ遷移
void EnemyHuman::TransitionDamageState()
{
    state = State::Damage;

    //ダメージアニメーション再生
    //model->PlayAnimation(Anim_GetHit, false);
}

//ダメージステート更新処理
void EnemyHuman::UpdateDamageState(float elapsedTime)
{
    //ダメージアニメーションが終わったら戦闘待機ステートへ遷移
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

//死亡ステートへ遷移
void EnemyHuman::TransitionDeathState()
{
    state = State::Death;

    //ダメージアニメーション再生
    //model->PlayAnimation(Anim_Die, false);
}

//死亡ステート更新処理
void EnemyHuman::UpdateDeathState(float elapsedTime)
{
    //ダメージアニメーションが終わったら自分を破棄
    if (!model->IsPlayAnimation())
    {
        Destroy();
    }
}