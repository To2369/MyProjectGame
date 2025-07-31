#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
#include "Camera.h"
#include "Collision.h"
#include "ArtsSpiritExplosion.h"
#include "ArtsSkillStraightBallet.h"
#include "NormalBallet.h"
#include "MessageData.h"
#include "Messenger.h"
#include "EnemyManager.h"
#include "PlayerStateDerived.h"
Player::Player()
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\pl\\Character1.fbx");
    const float scale_factor = 0.01f;
    scale = { scale_factor,scale_factor,scale_factor };
    radius = 0.5f;  // 幅
    height = 2.0f;  // 高さ

    stateMachine = std::make_unique<StateMachine<Player>>();

    stateMachine->RegisterState(new MovementState(this));
    stateMachine->RegisterState(new AttackState<Player>(this));
    stateMachine->RegisterState(new UseSkillState<Player>(this));
    stateMachine->RegisterState(new HitDamegeState<Player>(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new IdleState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new MoveState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new DashState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new JumpState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new LandState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_1(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_2(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_3(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_4(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_5(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_6(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_7(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState01_8(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState02_1(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new WeakAtkState02_2(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAtkState01(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAtkState02(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState03(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState04(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState05(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState06(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState07(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new StrongAttackState08(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Attack), new DashToEnemyState(this));


    stateMachine->RegisterSubState(static_cast<int>(Player::State::UseSkill), new SkillSelectState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::HitDamege), new DamegeState(this));

    // ヒットエフェクト読み込み
    hitEffect = std::make_unique<Effect>(effMap["hitEffect"].filename);

    //待機ステートへ遷移
    stateMachine->SetState(static_cast<int>(State::Movement));
    position.y = 10.0f;
    position.x=position.z = 0.0f;
    // カメラモード設定
    {
        MessageData::CAMERACHANGEFREEMODEDATA	p = { position };
        Messenger::Instance().SendData(MessageData::CAMERACHANGEFREEMODE, &p);
    }
}

Player::~Player()
{

}


void Player::Update(float elapsedTime)
{
    //ステート毎の処理
    stateMachine->Update(elapsedTime);

      // 速度処理更新
    UpdateVelocity(elapsedTime);

    // プレイヤーと敵との衝突処置
    CollisionPlayerAndEnemies();

    CollisionArtsAndEnemies();
    artsMgr.Update(elapsedTime);

    Lockon();

    InputDash(elapsedTime);
    Fly(elapsedTime);
    InputFlying(elapsedTime);
    InputJump();
    /* Mouse* mouse = InputManager::Instance()->getMouse();
     if (mouse->GetButton() & Mouse::BTN_RIGHT)
     {
         TeleportBehindEnemy();
     }*/
    UpdateStatus(elapsedTime);


    UpdateAnimation(elapsedTime);

    // ワールド行列更新
    UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, transform, { color });
    artsMgr.Render(dc);
}

void Player::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    std::string str = "";
    std::string subStr = "";
    switch (static_cast<State>(stateMachine->GetStateIndex()))
    {
    case State::Movement:
        str = "Move";
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Movement::Idle))
        {
            subStr = "Idle";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Movement::Move))
        {
            subStr = "Move";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Movement::Dash))
        {
            subStr = "Dash";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Movement::Jump))
        {
            subStr = "Jump";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Movement::Land))
        {
            subStr = "Land";
        }
        break;
    case State::Attack:
        str = "WeakAttack";
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_1))
        {
            subStr = "WeakAttack01";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_2))
        {
            subStr = "WeakAttack02";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_3))
        {
            subStr = "WeakAttack03";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_4))
        {
            subStr = "WeakAttack04";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_5))
        {
            subStr = "WeakAttack05";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::Attack::WeakAttack01_6))
        {
            subStr = "WeakAttack06";
        }

        break;
    case State::UseSkill:
        str = "UseSkill";
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::UseSkill::SkillSelect))
        {
            subStr = "SkillSelect";
        }
        break;
    case State::HitDamege:
        str = "HitDamege";
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::HitDamege::Damege))
        {
            subStr = "Damage";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::HitDamege::Death))
        {
            subStr = "Death";
        }
        break;
    }
    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &position.x);
            ImGui::SliderFloat("Position", &position.y, -10, 10);
            // 結果を保存
            ImGui::InputFloat("movespeed", &moveSpeed);
            ImGui::InputFloat4("quaternion", &quaternion.x);
            //スケール
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            ImGui::InputFloat("movespeed", &moveSpeed);
            ImGui::InputFloat("currentAnimationSeconds", &model->currentAnimationSeconds);
            ImGui::InputInt("hit", &hit);
            ImGui::Text(u8"State　%s", str.c_str());
            ImGui::Text(u8"Subtate　%s", subStr.c_str());
        }
    }
    ImGui::End();
#endif
}

void Player::DrawDebugPrimitive()
{
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawCylinder(position, radius, height, { 1,1,1,1 });
}

//入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec(const DirectX::XMFLOAT3& cameraRight, const DirectX::XMFLOAT3& cameraFront) const
{
    //入力情報を取得
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float ax = gamePad->GetAxisLX();
    float ay = gamePad->GetAxisLY();

    //移動ベクトルはXZ平面なベクトルになるようにする
    //カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //単位ベクトル化
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }
    //カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength < 0.0f)
    {
        //単位ベクトル化
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }
    //スティックの水平入力値をカメラ右方向に反映し
    //スティックの垂直入力値をカメラ前方向に反映し
    //進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
    vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
    //Y軸方向には移動しない
    vec.y = 0.0f;
    return vec;
}

//操作移動
bool Player::InputMove(float elapsedTime)
{
    Camera& camera = Camera::Instance();
    DirectX::XMFLOAT3 cameraRight = camera.GetRight();
    DirectX::XMFLOAT3 cameraFront = camera.GetFront();
    if (lockonState != LockonState::NotLocked)
    {
        cameraFront = lockDirection;
        DirectX::XMVECTOR	z = DirectX::XMLoadFloat3(&lockDirection);
        DirectX::XMVECTOR	y = DirectX::XMVectorSet(0, 1, 0, 0);
        DirectX::XMVECTOR	x = DirectX::XMVector3Cross(y, z);
        DirectX::XMStoreFloat3(&cameraRight, x);
    }

    //進行ベクトルを取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec(cameraRight, cameraFront);

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //旋回処理
    if (lockonState != LockonState::NotLocked)
    {
        //	ロックオン処理中はロックオン対象に向ける
        Turn(elapsedTime, cameraFront.x, cameraFront.z, turnSpeed);
    }
    else
    {
        Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    }

    //進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

// ジャンプ入力処理
bool Player::InputJump()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_A && !artSkillReady)
    {
        // ジャンプ回数制限
        if (jumpCount < jumpLimit)
        {
            // ジャンプ
            jumpCount++;
            Jump(jumpSpeed);
            return true;
        }
        else if (jumpCount >= jumpLimit)
        {
            if (!flyingFlag)
            {
                flyingFlag = true;
                jumpCount = 0;
            }
            else
            {
                flyingFlag = false;
            }
        }
    }
    return false;
}
void Player::InputFlying(float elapsedTime)
{
   
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButton() & GamePad::BTN_A && !artSkillReady && flyingFlag)
    {
        flySpeed = 10;
    }
    else if (gamePad->GetButton() & GamePad::BTN_B && flyingFlag)
    {
        flySpeed = -10;
    }
    else
    {
        flySpeed = 0;
    }
}

// void型に修正する必要あり
bool Player::InputArts(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    Mouse* mouse = InputManager::Instance()->getMouse();
    artSkillReady = false;
    artUltSkillReady = false;
    // 回避技や究極技系
    if (gamePad->GetButton() & GamePad::BTN_LEFT_TRIGGER)// Rキー
    {
        artUltSkillReady = true;
        if (gamePad->GetButtonDown() & GamePad::BTN_B) // Qキー
        {
            if (spiritEnergy >= 300)
            {
                DirectX::XMFLOAT3 f;
                DirectX::XMStoreFloat3(&f, front);
                // 前方向
                DirectX::XMFLOAT3 dir;
                dir.x = f.x;
                dir.y = 0.0f;
                dir.z = f.z;


                // 発射位置（プレイヤーの腰あたり
                DirectX::XMFLOAT3 pos;
                pos.x = position.x;
                pos.y = position.y + height * 0.5f;
                pos.z = position.z;

                // 発射
                ArtsSpiritExplosion* artsSpiritExplosion = new ArtsSpiritExplosion(&artsMgr);
                artsSpiritExplosion->Launch(dir, pos);

                spiritEnergy -= artsSpiritExplosion->GetUseSpiritEnergy();
                return true;
            }
        }
    }
    // 必殺技
    else if (gamePad->GetButton() & GamePad::BTN_RIGHT_TRIGGER) // Fキー
    {
        artSkillReady = true;
        if (mouse->GetButtonDown() & Mouse::BTN_LEFT)
        {
            ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
            if (skillEnergy >= artsSkillStraightBallet->GetUseSkillEnergy())
            {
                DirectX::XMFLOAT3 f;
                DirectX::XMStoreFloat3(&f, front);
                // 前方向
                DirectX::XMFLOAT3 dir;
                dir.x = f.x;
                dir.y = 0.0f;
                dir.z = f.z;
                DirectX::XMVECTOR dirVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir));

                // 発射位置（プレイヤーの腰あたり
                DirectX::XMFLOAT3 pos;
                pos.x = position.x;
                pos.y = position.y + height * 0.5f;
                pos.z = position.z;

                float offsetDistance = 1.5f; // 少し前の距離（調整可能）
                DirectX::XMVECTOR offset = DirectX::XMVectorScale(dirVec, offsetDistance);

                // オフセットを発射位置に加算
                DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
                posVec = DirectX::XMVectorAdd(posVec, offset);
                DirectX::XMStoreFloat3(&pos, posVec);

                // 発射
                artsSkillStraightBallet->Launch(dir, pos);
                skillEnergy -= artsSkillStraightBallet->GetUseSkillEnergy();
            }
            return true;
        }
        // 技力回復
        else if (gamePad->GetButton() & GamePad::BTN_A) // Space
        {
            skillEnergyTimer -= elapsedTime;
            if (skillEnergyTimer <= 0)
            {
                skillEnergy += 1;
                skillEnergyTimer = 0.025f;
            }
            return true;
        }
    }
    //　通常弾
    else if (gamePad->GetButtonDown() & GamePad::BTN_B)
    {
        NormalBallet* normalBallet = new NormalBallet(&artsMgr);
        if (skillEnergy >= normalBallet->GetUseSkillEnergy())
        {
            DirectX::XMFLOAT3 f;
            DirectX::XMStoreFloat3(&f, front);
            // 前方向
            DirectX::XMFLOAT3 dir;
            dir.x = f.x;
            dir.y = 0.0f;
            dir.z = f.z;


            // 発射位置（プレイヤーの腰あたり
            DirectX::XMFLOAT3 pos;
            pos.x = position.x;
            pos.y = position.y + height * 0.5f;
            pos.z = position.z;

            // ターゲット
            DirectX::XMFLOAT3 target;
            target.x = pos.x + dir.x * 1000.0f;
            target.y = pos.y + dir.y * 1000.0f;
            target.z = pos.z + dir.z * 1000.0f;

            // 一番近くの敵をターゲットに設定
            float dist = FLT_MAX;
            EnemyManager& enemyMgr = EnemyManager::Instance();
            int enemyCount = enemyMgr.GetEnemyCount();
            for (int i = 0; i < enemyCount; ++i)
            {
                // 敵との距離を判定
                Enemy* enemy = enemyMgr.GetEnemy(i);
                DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
                DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(&enemy->GetPosition());
                DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
                DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
                float lengthSq;
                DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
                if (lengthSq < dist)
                {
                    dist = lengthSq;
                    target = enemy->GetPosition();
                    target.y += enemy->GetHeight() * 0.5f;

                }
            }
            // 発射
            normalBallet->Launch(dir, pos, target);
            normalBallet->LockonTarget(target);
            skillEnergy -= normalBallet->GetUseSkillEnergy();
            return true;
        }
    }
    return false;
}

// 着地したときに呼び出される
void Player::OnLanding()
{
    jumpCount = 0;
}

// プレイヤーと敵との衝突処理
void Player::CollisionPlayerAndEnemies()
{
    EnemyManager& eneMgr = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = eneMgr.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = eneMgr.GetEnemy(i);

        // 衝突方向
        DirectX::XMFLOAT3 outVec;

        // 衝突処理
        if (Collision::IntersectCylinderVsCylinder(
            position,
            radius,
            height,
            (enemy->GetPosition()),
            enemy->GetRadius(),
            enemy->GetHeight(),
            outVec
        ))
        {

            dashTowardEnemyFlag = false;

            // 半径の合計
            float range = radius + enemy->GetRadius();

            // 衝突方向のベクトルを半径の合計の長さにスケーリング
            DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

            // そのベクトルを衝突する側の位置に足す
            vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

            // その位置ベクトルを衝突される側に設定する
            DirectX::XMFLOAT3 pos;
            DirectX::XMStoreFloat3(&pos, vec);
            pos.y = enemy->GetPosition().y;
            enemy->SetPosition(pos);
        }
        else
        {
        }
    }
}

void Player::CollisionArtsAndEnemies()
{
    EnemyManager& enemyMgr = EnemyManager::Instance();

    // 全ての弾と全ての敵を総当たりで衝突処理
    int artsCount = artsMgr.GetArtsCount();
    int enemyCount = enemyMgr.GetEnemyCount();
    for (int i = 0; i < artsCount; ++i)
    {
        Arts* arts = artsMgr.GetArts(i);

        for (int j = 0; j < enemyCount; j++)
        {
            Enemy* enemy = enemyMgr.GetEnemy(j);


            // 衝突処理
            DirectX::XMFLOAT3 outPos;
            if (arts->GetType() == BalletType::Normal || arts->GetType() == BalletType::SpiritExplosion)
            {
                if (Collision::IntersectSphereVsCylinder(
                    arts->GetPosition(),
                    arts->GetRadius(),
                    enemy->GetPosition(),
                    enemy->GetRadius(),
                    enemy->GetHeight(),
                    outPos))
                {
                    int damage = 1;
                    if (enemy->ApplyDamage(0.5f, arts->GetDamage()))
                    {
                        // 吹き飛ばし
                        float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        impulse.x = outPos.x * power;
                        impulse.y = power * 0.5f;
                        impulse.z = outPos.z * power;

                        enemy->AddImpulse(impulse);

                        // ヒットエフェクトの再生
                        DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                        enePos.y += enemy->GetHeight() * 0.5f;
                        Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        // 弾の破棄
                        arts->Destroy();
                    }
                }
            }
            else if (arts->GetType() == BalletType::SkillStraight)
            {
                IntersectionResult* result = {};
                if (Collision::IntersectCapsuleVsCapsule(
                    DirectX::XMLoadFloat3(&arts->GetPosition()),
                    DirectX::XMLoadFloat3(&arts->GetDirection()),
                    arts->GetCurrentCapsuleHeight(),
                    arts->GetRadius(),
                    DirectX::XMLoadFloat3(&enemy->GetPosition()),
                    { 0,1,0 },
                    enemy->GetHeight(),
                    enemy->GetRadius(),
                    result))
                {
                    arts->SetGrowSpeed(0);
                    if (enemy->ApplyDamage(0.5f, arts->GetDamage()))
                    {
                        
                        // ヒットエフェクトの再生
                        DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                        enePos.y += enemy->GetHeight() * 0.5f;
                        Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        
                    }
                    if (arts->GetLifeTimer() <= 0.1f)
                    {
                        // 吹き飛ばし
                        float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        impulse.x = arts->GetDirection().x * power;
                        impulse.y = 0;
                        impulse.z = arts->GetDirection().z*power;

                        enemy->AddImpulse(impulse);
                    }
                }
            }
        }
    }
}

void Player::CollisionNodeVsEnemies(const std::string nodeName, float nodeRadius, float invTimer, int damage)
{
    atkCollisionFlag = true;
    //ノード取得
    Skeleton::Bone* bone = model->FindNode(nodeName);
    auto& node = model->keyframe.nodes[bone->nodeIndex];
    auto WorldTransform = DirectX::XMLoadFloat4x4(&node.globalTransform) * DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMFLOAT4X4 worldTransform;
    DirectX::XMStoreFloat4x4(&worldTransform, WorldTransform);
    // ノード位置取得
    DirectX::XMFLOAT3 nodePosition;
    nodePosition.x = worldTransform._41;
    nodePosition.y = worldTransform._42;
    nodePosition.z = worldTransform._43;
    EnemyManager& enemy = EnemyManager::Instance();
    int count = EnemyManager::Instance().GetEnemyCount();
    if (!isHit)
    {
        for (int i = 0; i < count; i++)
        {
            Enemy* enemys = enemy.GetEnemy(i);

            // 衝突処理
            DirectX::XMFLOAT3 outPos;
            if (Collision::IntersectSphereVsCylinder(
                nodePosition,
                nodeRadius,
                enemys->GetPosition(),
                enemys->GetRadius(),
                enemys->GetHeight(),
                outPos
            ))
            {
                if (enemys->ApplyDamage(invTimer, damage))
                { 
                    // ノックバック方向 = 敵位置 - プレイヤー位置
                    DirectX::XMFLOAT3 enemyPos = enemys->GetPosition();

                    DirectX::XMFLOAT3 dir;
                    dir.x = enemyPos.x - position.x;
                    dir.y = 0.0f;
                    dir.z = enemyPos.z - position.z;


                    float length = sqrt(dir.x * dir.x + dir.z * dir.z);
                    if (length > 0.0001f)
                    {
                        dir.x /= length;
                        dir.z /= length;
                    }

                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse = {
                        dir.x * power,
                        0.0f,
                        dir.z * power
                    };

                    // プレイヤーもその方向に少しだけ前進
                    if (!awayFlag)
                    {
                        this->AddImpulse(impulse);
                    }

                    enemys->AddImpulse(impulse);
                    {
                        DirectX::XMFLOAT3 e = enemys->GetPosition();
                        e.y += 0.5f;
                        hitEffect->Play(&e,effMap["hitEffect"].scale);
                    }
                    skillEnergy += 15;
                    isHit = true;
                }
            }
        }
    }
    {
#ifdef USE_IMGUI

        DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
        if (atkCollisionFlag)
        {
            Skeleton::Bone* bone = model->FindNode(nodeName);
            if (bone && !model->keyframe.nodes.empty())
            {
                //  外套のボーンのノードの市政情報を取得
                auto& node = model->keyframe.nodes[bone->nodeIndex];
                auto WorldTransform = DirectX::XMLoadFloat4x4(&node.globalTransform) * DirectX::XMLoadFloat4x4(&transform);
                DirectX::XMFLOAT4X4 worldTransform;
                DirectX::XMStoreFloat4x4(&worldTransform, WorldTransform);
                debugPrimitive->DrawSphere(DirectX::XMFLOAT3(
                    worldTransform._41,
                    worldTransform._42,
                    worldTransform._43),
                    nodeRadius,
                    DirectX::XMFLOAT4(0, 1, 0, 1)
                );
            }
        }
#endif //  DEBUG
    }
}

void Player::TeleportBehindEnemy()
{
    int useEnergy = 200;
    if (spiritEnergy >= useEnergy)
    {
        // 一番近くの敵をターゲットに設定
        float minDistance = FLT_MAX;
        Enemy* closestEnemy = nullptr;
        EnemyManager& enemyMgr = EnemyManager::Instance();
        int enemyCount = enemyMgr.GetEnemyCount();

        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyMgr.GetEnemy(i);

            // 敵との距離を計算
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR vecToEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vecToEnemy);

            float distance;
            DirectX::XMStoreFloat(&distance, lengthSqVec);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestEnemy = enemy;
            }
        }

        if (closestEnemy)
        {
            spiritEnergy -= useEnergy;
            useSpiritEnergyFlag = true;

            // 敵の後ろに飛ぶ
            DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();

            // 敵の前方方向を計算 (敵の向いている方向を仮定)
            DirectX::XMFLOAT3 enemyForward = {};
            DirectX::XMStoreFloat3(&enemyForward, closestEnemy->GetFront());
            enemyForward.y = 0.0f;

            // 敵の後ろの位置 = 敵の位置 - 前方方向 * 2.0f (距離を調整)
            const float distanceBehindEnemy = 2.0f; // プレイヤーが敵から後ろに飛ぶ距離
            DirectX::XMFLOAT3 teleportPos;
            teleportPos.x = enemyPos.x - (enemyForward.x * distanceBehindEnemy);
            teleportPos.y = enemyPos.y;
            teleportPos.z = enemyPos.z - (enemyForward.z * distanceBehindEnemy);

            // プレイヤーの位置を更新
            position = teleportPos;

            // プレイヤーが敵の方を向くようにクォータニオンを使用して回転
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);

            // 敵の方への方向ベクトルを計算
            DirectX::XMVECTOR toEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            toEnemy = DirectX::XMVector3Normalize(toEnemy);

            // 方向ベクトルから回転行列を計算
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixLookAtLH(playerPosVec, toEnemy, up);

            // 回転行列からクォータニオンを計算
            DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationMatrix(-rotationMatrix);


            // プレイヤーの回転にクォータニオンを適用
            DirectX::XMStoreFloat4(&quaternion, rotationQuat);
        }
    }
}

bool Player::InputDashTowardsEnemy(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float useEnergy = 5;
    // Xボタンでダッシュ
    if (gamePad->GetButtonDown() & GamePad::BTN_X)
    {
        // Xボタンでダッシュ
        if (skillEnergy >= useEnergy)
        {
            if (dashTowardEnemyFlag)
            {
                dashTowardEnemyFlag = false;
            }
            dashTowardEnemyFlag = true;
        }
        else
        {
            dashTowardEnemyFlag = false;
        }
    }

    float useEnergyTimer = 0.3f;
    if (dashTowardEnemyFlag)
    {
        // 一番近くの敵をターゲットに設定
        float minDistance = FLT_MAX;
        Enemy* closestEnemy = nullptr;
        EnemyManager& enemyMgr = EnemyManager::Instance();
        int enemyCount = enemyMgr.GetEnemyCount();

        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyMgr.GetEnemy(i);

            // 敵との距離を計算
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR vecToEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vecToEnemy);

            float distance;
            DirectX::XMStoreFloat(&distance, lengthSqVec);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestEnemy = enemy;
            }
        }

        if (closestEnemy)
        {
            skillEnergyTimer -= elapsedTime;
            if (skillEnergyTimer <= 0)
            {
                skillEnergy -= useEnergy;
                skillEnergyTimer = useEnergyTimer;
            }

            // 敵の方向を計算
            DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR directionVec = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            directionVec = DirectX::XMVector3Normalize(directionVec);

            // ダッシュ速度を設定 (徐々に移動する速度)
            float dashSpeed = 10.0f; // ダッシュの速さ（フレーム単位）
            DirectX::XMVECTOR dashVec = DirectX::XMVectorScale(directionVec, dashSpeed * elapsedTime);

            // プレイヤーの位置を更新 (徐々に移動)
            DirectX::XMVECTOR newPositionVec = DirectX::XMVectorAdd(playerPosVec, dashVec);
            DirectX::XMStoreFloat3(&position, newPositionVec);

            // 敵の方向を向くための回転クォータニオンを設定
            DirectX::XMVECTOR cross = DirectX::XMVector3Cross(front, directionVec); // 前方とターゲット方向の外積
            float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(front, directionVec)); // 前方とターゲット方向の内積

            if (!DirectX::XMVector3Equal(cross, DirectX::XMVectorZero()))
            {
                // 外積がゼロでない場合、通常の回転軸でクォータニオンを生成
                float angleRadians = acosf(dot); // 内積から角度を求める
                DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Normalize(cross), angleRadians);

                // プレイヤーの回転を更新（クォータニオンとして設定）
                DirectX::XMStoreFloat4(&quaternion, rotationQuaternion);
            }
            else
            {
                // 外積がゼロの場合（完全に同方向または反対方向）
                if (dot > 0.0f)
                {
                    // 同方向の場合、回転不要（単位クォータニオン）
                    quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
                }
                else
                {
                    // 反対方向の場合、180度回転
                    DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(
                        up, DirectX::XM_PI); // Y軸周りに180度回転
                    DirectX::XMStoreFloat4(&quaternion, rotationQuaternion);
                }
            }
        }
        return true;
    }
    else
    {
        skillEnergyTimer = useEnergyTimer;
    }
    return false;
}

bool Player::InputDash(float elapsedTime)
{
    float useEnergy = 1;
    float useEnergyTimer = 0.3f;
    if (skillEnergy >= useEnergy)
    {
        Mouse* mouse = InputManager::Instance()->getMouse();
        if (mouse->GetButton() & Mouse::BTN_RIGHT)
        {
            this->moveSpeed = 10.0f;
            skillEnergyTimer -= elapsedTime;
            if (skillEnergyTimer <= 0)
            {
                skillEnergy -= useEnergy;
                skillEnergyTimer = useEnergyTimer;
            }
            InputMove(elapsedTime);
            return true;
        }
        else
        {
            moveSpeed = 5.0f;
            skillEnergyTimer = useEnergyTimer;
            return false;
        }
    }
    else
    {
        moveSpeed = 5.0f;
        skillEnergyTimer = useEnergyTimer;
        return false;
    }
}

bool Player::InputWeekAttack()
{
    Mouse* mouse = InputManager::Instance()->getMouse();
    if (mouse->GetButtonDown() & Mouse::BTN_LEFT && !artSkillReady)
    {
        return true;
    }
    return false;
}

bool Player::InputStrongAttack()
{
    Mouse* mouse = InputManager::Instance()->getMouse();
    if (mouse->GetButtonDown() & Mouse::BTN_RIGHT && !artSkillReady)
    {
        return true;
    }
    return false;
}

void Player::Lockon()
{
    LockonState oldLockonState = lockonState;
    Enemy* oldLockonenemy = lockonEnemy;
    lockonState = LockonState::NotLocked;
    lockonEnemy = nullptr;
    EnemyManager& enemyMgr = EnemyManager::Instance();
    int enemyCount = enemyMgr.GetEnemyCount();
    Mouse* mouse = InputManager::Instance()->getMouse();
    if (mouse->GetButtonDown() & Mouse::BTN_MIDDLE)
    {
        if (lockon)
        {
            lockon = false;
        }
        else
        {
            lockon = true;
        }
    }

    if (lockon)
    {
        DirectX::XMVECTOR p, t, v;
        switch (oldLockonState)
        {
        case Player::LockonState::NotLocked:
            float	length1, length2;


            for (int i = 0; i < enemyCount; ++i)
            {
                Enemy* enemy = enemyMgr.GetEnemy(i);

                if (lockonState != LockonState::NotLocked)
                {
                    p = DirectX::XMLoadFloat3(&position);
                    t = DirectX::XMLoadFloat3(&lockonEnemy->GetPosition());
                    v = DirectX::XMVectorSubtract(t, p);
                    DirectX::XMStoreFloat(&length2, DirectX::XMVector3LengthSq(v));
                    p = DirectX::XMLoadFloat3(&position);
                    t = DirectX::XMLoadFloat3(&enemy->GetPosition());
                    v = DirectX::XMVectorSubtract(t, p);
                    DirectX::XMStoreFloat(&length1, DirectX::XMVector3LengthSq(v));
                    if (length1 < length2)
                    {
                        lockonEnemy = enemy;
                        DirectX::XMStoreFloat3(&lockDirection, DirectX::XMVector3Normalize(v));
                    }
                }
                else
                {
                    p = DirectX::XMLoadFloat3(&position);
                    t = DirectX::XMLoadFloat3(&enemy->GetPosition());
                    v = DirectX::XMVectorSubtract(t, p);
                    DirectX::XMStoreFloat(&length1, DirectX::XMVector3LengthSq(v));

                    lockonEnemy = enemy;
                    DirectX::XMStoreFloat3(&lockDirection, DirectX::XMVector3Normalize(v));
                    lockonState = LockonState::Locked;
                }
            }
            break;
        case Player::LockonState::Locked:
            for (int i = 0; i < enemyCount; ++i)
            {
                Enemy* enemy = enemyMgr.GetEnemy(i);
                if (enemy == oldLockonenemy)
                {
                    lockonEnemy = enemy;
                    lockonState = LockonState::Locked;
                    p = DirectX::XMLoadFloat3(&position);
                    t = DirectX::XMLoadFloat3(&enemy->GetPosition());
                    v = DirectX::XMVectorSubtract(t, p);

                    lockonEnemy = enemy;
                    DirectX::XMStoreFloat3(&lockDirection, DirectX::XMVector3Normalize(v));
                }
            }
            break;
        }
        if (lockonState == LockonState::Locked)
        {
            MessageData::CAMERACHANGELOCKONMODEDATA	p = { position, lockonEnemy->GetPosition() };
            Messenger::Instance().SendData(MessageData::CAMERACHANGELOCKONMODE, &p);
        }

    }
    else
    {
        MessageData::CAMERACHANGEFREEMODEDATA	p = { position };
        Messenger::Instance().SendData(MessageData::CAMERACHANGEFREEMODE, &p);
    }
}

void Player::InputAttackNext(float currentAnimaSeconds, AttackData attackData)
{
    if (currentAnimaSeconds >= attackData.inputAcceptStartTime
        && currentAnimaSeconds <= attackData.inputAcceptEndTime)
    {
        if (InputWeekAttack())
        {
            weekAtkNextFlag = true;
        }
        else if (InputStrongAttack())
        {
            strongAtkNextFlag = true;
        }
    }
}

void Player::ActiveAttackCollider(AttackData attackData)
{
    if (model->currentAnimationSeconds >= attackData.hitStartTime
        && model->currentAnimationSeconds <= attackData.hitEndTime)
    {
        CollisionNodeVsEnemies(
            attackData.hitBoneName,
            attackData.hitRadius,
            invincibleTimer,
            attackData.damage);
    }
    else
    {
        isHit = false;
    }

    if (attackData.secondHitBoneName!="")
    {
        if (model->currentAnimationSeconds >= attackData.secondHitStartTime
            && model->currentAnimationSeconds <= attackData.secondHitEndTime)
        {
            CollisionNodeVsEnemies(
                attackData.secondHitBoneName,
                attackData.secondHitRadius,
                invincibleTimer,
                attackData.damage);
        }
        else
        {
            isHit = false;
        }
    }
}

void Player::AttackMove(float startMoveTimer,float endMoveTime,float speed)
{
    // 前方向ベクトル
    DirectX::XMVECTOR forward = front;
    if (model->currentAnimationSeconds >= startMoveTimer
        && model->currentAnimationSeconds <= endMoveTime)
    {

        // 前方向への移動距離ベクトル = front * speed
        DirectX::XMVECTOR movement = DirectX::XMVectorScale(forward, speed);

        // 現在の position（XMFLOAT3）を XMVECTOR に変換
        DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&position);

        // 移動ベクトルを加算
        DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(currentPos, movement);

        // 新しい位置を XMFLOAT3 に戻して保存
        DirectX::XMStoreFloat3(&position, newPos);
    }
}