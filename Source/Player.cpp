#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
#include "ArtsSpiritExplosion.h"
#include "ArtsSkillStraightBallet.h"
Player::Player()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\resources\\nico.fbx");
    geo= std::make_unique<GeometricCapsule>(Graphics::Instance()->GetDevice(), height/2, DirectX::XMFLOAT3{ 100,100,100 }, 6, 3, 3, DirectX::XMFLOAT3{ angle.x,angle.y,angle.z });
	const float scale_factor = 0.01f;
	scale = { scale_factor,scale_factor,scale_factor };
    height=1;
    //bulletMgr = std::make_unique<BulletManager>();

    // ヒットエフェクト読み込み
    //hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

    //待機ステートへ遷移
    TransitionIdleState();
}

Player::~Player()
{
   
}

void Player::Update(float elapsedTime)
{
    //ステート毎の処理
    switch (state)
    {
    case State::Idle:UpdateIdleState(elapsedTime); break;
    case State::Move:UpdateMoveState(elapsedTime); break;
    case State::Dash:UpdateDashState(elapsedTime); break;
    case State::DashToEnemy:UpdateDashToEnemyState(elapsedTime); break;
    case State::RecoverySkillEnergy:UpdateRecoverySkillEnergyState(elapsedTime); break;
    }
    // 速度処理更新
    UpdateVelocity(elapsedTime);

    // ジャンプ入力処理
    InputJump();

    // 入力による弾発射処理
    InputLaunchBullet();

    InputArts();

    // プレイヤーと敵との衝突処置
    CollisionPlayerAndEnemies();
    // 弾と敵の衝突処理
    CollisionBulletsAndEnemies();

    CollisionArtsAndEnemies();
    CollisionPlayerAndArts();
    // 弾更新処理
    bulletMgr.Update(elapsedTime);
    artsMgr.Update(elapsedTime);
    model->UpdateAnimation(elapsedTime);

    UpdateStatus(elapsedTime);
	// ワールド行列更新
	UpdateTransform();
}
int a = 0;
void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform,{ 1.0f,1.0f,1.0f,1.0f });
    bulletMgr.Render(dc);
    artsMgr.Render(dc);
    if (a == 1)
       geo->Render(dc, transform, { 0,0,1,1 });
    ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
    artsSkillStraightBallet->Render(dc);//Launch(dir, pos);
}

void Player::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    std::string str = "";
    std::string subStr = "";
    switch (static_cast<State>(state))
    {
    case State::Idle:
        str = "Idle";
        break;
    case State::Move:
        str = "Move";
        break;
    case State::Dash:
        str = "Dash";
        break;
    case State::DashToEnemy:
        str = "DashToEnemy";
        break;
    case State::RecoverySkillEnergy:
        str = "RecoverySkillEnergy";
        break;
    }
    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &position.x);
            ImGui::SliderFloat("Position", &position.y,-10,10);
            //回転
            DirectX::XMFLOAT4 pquater;
            pquater.x = DirectX::XMConvertToDegrees(quaternion.x);
            pquater.y = DirectX::XMConvertToDegrees(quaternion.y);
            pquater.z = DirectX::XMConvertToDegrees(quaternion.z);
            pquater.w = DirectX::XMConvertToDegrees(quaternion.w);
            ImGui::InputFloat4("Angle", &pquater.x);
   /*         quaternion.x = DirectX::XMConvertToRadians(pquater.x);
            quaternion.y = DirectX::XMConvertToRadians(pquater.y);
            quaternion.z = DirectX::XMConvertToRadians(pquater.z);
            quaternion.w = DirectX::XMConvertToRadians(pquater.w);*/
            //スケール
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            ImGui::InputFloat("movespeed", &moveSpeed);
            ImGui::InputInt("a", &a);
            ImGui::Text(u8"State　%s", str.c_str());
            //ImGui::Text(u8"Subtate　%s", subStr.c_str());
        }
    }
    ImGui::End();
#endif
}

void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });

    // 衝突判定用のデバッグ用円柱を描画
    //debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });

    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    //debugPrimitive->DrawSphere(position, radius, { 0,0,1,1 });
    //debugPrimitive->DrawCube(position, {1,1,1}, { 1,1,1,1 });
    debugPrimitive->DrawCylinder(position, radius, height, { 1,1,1,1 });
    {
        //if(a==1)
        //debugPrimitive->DrawCapsule(position, { radius,radius,radius }, height, { 1,1,1,1 });
    }
    // 弾デバッグプリミティブ描画
    bulletMgr.DrawDebugPrimitive();
    artsMgr.DrawDebugPrimitive();
}

//入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    //入力情報を取得
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float ax = gamePad->GetAxisLX();
    float ay = gamePad->GetAxisLY();

    //カメラ方向を取得
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

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
    //進行ベクトルを取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, this->turnSpeed);


    //進行ベクトルがゼロベクトルでない場合は入力された
    return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

// ジャンプ入力処理
void Player::InputJump()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_A)
    {
        // ジャンプ回数制限
        if (jumpCount < jumpLimit)
        {
            // ジャンプ
            jumpCount++;
            Jump(jumpSpeed);
        }
    }
}

// 入力による弾発射処理
void Player::InputLaunchBullet()
{
    float useEnergy = 2;
    if (skillEnergy >= useEnergy)
    {
        GamePad* gamePad = InputManager::Instance()->getGamePad();
        std::unique_ptr<StraightBullet> bullet;
        // ストレート弾発射
        //if (gamePad->GetButtonDown() & GamePad::BTN_X)
        //{
        //    // 前方向
        //    DirectX::XMFLOAT3 dir;
        //    dir.x = sinf(angle.y);
        //    dir.y = 0.0f;
        //    dir.z = cosf(angle.y);

        //    // 発射位置（プレイヤーの腰あたり
        //    DirectX::XMFLOAT3 pos;
        //    pos.x = position.x;
        //    pos.y = position.y + height * 0.5f;
        //    pos.z = position.z;

        //    // 発射
        //    StraightBullet* bullet = new StraightBullet(&bulletMgr);
        //    bullet->Launch(dir, pos);
        //    //bulletMgr->Regist(bullet.get());
        //}

        // ホーミング弾発射
        if (gamePad->GetButtonDown() & GamePad::BTN_Y)
        {
            skillEnergy -= useEnergy;
            // 前方向
            DirectX::XMFLOAT3 dir;
            dir.x = sinf(angle.y);
            dir.y = 0.0f;
            dir.z = cosf(angle.y);

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
            HomingBullet* bullet = new HomingBullet(&bulletMgr);
            bullet->Launch(dir, pos, target);
            bullet->LockonTarget(target);
        }
    }
}

void Player::InputArts()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    // ストレート弾発射
    if (gamePad->GetButtonDown() & GamePad::BTN_Y)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // 発射位置（プレイヤーの腰あたり
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // 発射
        ArtsSpiritExplosion* artsSpiritExplosion  = new ArtsSpiritExplosion(&artsMgr);
        artsSpiritExplosion->Launch(dir, pos);
     /*   ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
        artsSkillStraightBallet->Launch(dir,pos);*///Launch(dir, pos);
        //spiritEnergy -= artsSpiritExplosion->GetUseSpiritEnergy();
        //bulletMgr->Regist(bullet.get());
    }

    // ストレート弾発射
    //if (gamePad->GetButtonDown() & GamePad::BTN_Y)
    //{
    //    // 前方向
    //    DirectX::XMFLOAT3 dir;
    //    dir.x = sinf(angle.y);
    //    dir.y = 0.0f;
    //    dir.z = cosf(angle.y);

    //    // 発射位置（プレイヤーの腰あたり
    //    DirectX::XMFLOAT3 pos;
    //    pos.x = position.x;
    //    pos.y = position.y + height * 0.5f;
    //    pos.z = position.z;

    //    // 発射
    //    StraightBullet* bullet = new StraightBullet(&bulletMgr);
    //    bullet->Launch(dir, pos);
    //    //bulletMgr->Regist(bullet.get());
    //}
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
        if (Collision::IntersectCylinderAndCylinder(
            position,
            radius,
            height,
            (enemy->GetPosition()),
            enemy->GetRadius(),
            enemy->GetHeight(),
            outVec
        ))
        {
            //OutputDebugStringA("衝突\n");

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

// 弾と敵の衝突処理
void Player::CollisionBulletsAndEnemies()
{
    EnemyManager& enemyMgr = EnemyManager::Instance();

    // 全ての弾と全ての敵を総当たりで衝突処理
    int bulletCount = bulletMgr.GetBulletCount();
    int enemyCount = enemyMgr.GetEnemyCount();
    for (int i = 0; i < bulletCount; ++i)
    {
        Bullet* bullet = bulletMgr.GetBullet(i);

        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyMgr.GetEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outVec;
            if (Collision::IntersectSphereAndCylinder(
                bullet->GetPosition(),
                bullet->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outVec))
            {
                int damage = 1;
                if (enemy->ApplyDamage(0.5f,damage))
                {
                    // 吹き飛ばし
                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse;
                    impulse.x = outVec.x * power;
                    impulse.y = power * 0.5f;
                    impulse.z = outVec.z * power;

                    enemy->AddImpulse(impulse);

                    // ヒットエフェクトの再生
                    DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                    enePos.y += enemy->GetHeight() * 0.5f;
                    //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                    // 弾の破棄
                    bullet->Destroy();
                }
            }
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

        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyMgr.GetEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outVec;
            if (Collision::IntersectSphereAndCylinder(
                arts->GetPosition(),
                arts->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outVec))
            {
                int damage = 1;
                if (enemy->ApplyDamage(0.5f, arts->GetDamage()))
                {
                    // 吹き飛ばし
                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse;
                    impulse.x = outVec.x * power;
                    impulse.y = power * 0.5f;
                    impulse.z = outVec.z * power;

                    enemy->AddImpulse(impulse);

                    // ヒットエフェクトの再生
                    DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                    enePos.y += enemy->GetHeight() * 0.5f;
                    //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                    // 弾の破棄
                    //arts->Destroy();
                }
            }
        }
    }
}

void Player::CollisionPlayerAndArts()
{
    // 全ての弾と全ての敵を総当たりで衝突処理
    int artsCount = artsMgr.GetArtsCount();
    IntersectionResult result;
    for (int i = 0; i < artsCount; ++i)
    {
        Arts* arts = artsMgr.GetArts(i);

        // 衝突処理
        DirectX::XMFLOAT3 outVec;
        direction.y = 1;
        DirectX::XMFLOAT3 plPos = position;
        plPos.y +=0.5f;
        if (Collision::IntersectCapsuleAndCapsule(
            DirectX::XMLoadFloat3(&plPos),
            DirectX::XMLoadFloat3(&direction),
            height/2,
            radius,
            DirectX::XMLoadFloat3(&arts->GetPosition()),
            DirectX::XMLoadFloat3(&arts->GetDirection()),
            arts->GetHeight(),
            arts->GetRadius(),
            &result))
        {
            a = 1;
            //DirectX::XMStoreFloat3(&position,result.pointB);
        }
        else
        {
            a = 0;
        }
        
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
            DirectX::XMFLOAT3 enemyForward;
            enemyForward.x = sinf(closestEnemy->GetAngle().y);
            enemyForward.y = 0.0f;
            enemyForward.z = cosf(closestEnemy->GetAngle().y);

            // 敵の後ろの位置 = 敵の位置 - 前方方向 * 2.0f (距離を調整)
            const float distanceBehindEnemy = 2.0f; // プレイヤーが敵から後ろに飛ぶ距離
            DirectX::XMFLOAT3 teleportPos;
            teleportPos.x = enemyPos.x - (enemyForward.x * distanceBehindEnemy);
            teleportPos.y = enemyPos.y;
            teleportPos.z = enemyPos.z - (enemyForward.z * distanceBehindEnemy);

            // プレイヤーの位置を更新
            position = teleportPos;

            // プレイヤーが敵の方を向くように回転
            angle.y = closestEnemy->GetAngle().y;
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

            // 常に敵の方向を向くように回転
            DirectX::XMVECTOR forwardVec = DirectX::XMVectorSet(0, 0, 1, 0);  // プレイヤーの現在の前方ベクトル
            DirectX::XMVECTOR rightVec = DirectX::XMVectorSet(1, 0, 0, 0);    // プレイヤーの右方向ベクトル
            DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0, 1, 0, 0);       // 上方向ベクトル

            // 敵への方向ベクトルのY軸成分のみを使用して回転を計算（Y軸回転）
            DirectX::XMVECTOR projectedDirectionVec = DirectX::XMVector3Normalize(
                DirectX::XMVectorSet(DirectX::XMVectorGetX(directionVec), 0, DirectX::XMVectorGetZ(directionVec), 0)
            );

            // Y軸周りの回転角度を計算
            float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(forwardVec, projectedDirectionVec));
            float det = DirectX::XMVectorGetX(DirectX::XMVector3Dot(rightVec, projectedDirectionVec));
            float angleRadians = atan2(det, dot);

            // Y軸回転を更新
            angle.y = angleRadians;
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

bool Player::InputRecoverySkillEnergy(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButton() & GamePad::BTN_A)//Z
    {
        skillEnergyTimer -= elapsedTime;
        if (skillEnergyTimer <= 0)
        {
            skillEnergy += 1;
            skillEnergyTimer = 0.1f;
        }
        return true;
    }
    return false;
}

//待機ステートへ遷移
void Player::TransitionIdleState()
{
    state = State::Idle;

    //待機アニメーション再生
    model->PlayAnimation(0, true);
}

//待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
    //移動入力処理
    InputMove(elapsedTime);
    if (InputMove(elapsedTime))
    {
        //移動ステートへの遷移
        TransitionMoveState();
    }

    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_B)//x
    {
        TeleportBehindEnemy();
    }

    if (InputDashTowardsEnemy(elapsedTime))
    {
        TransitionDashToEnemyState();
    }
    if (InputRecoverySkillEnergy(elapsedTime))
    {
        TransitionRecoverySkillEnergy();
    }
    ////ジャンプ入力処理
    //InputJump();
    //if (InputJump() == true)
    //{
    //    //ジャンプステートへの遷移
    //    TransitionJumpState();
    //}

    ////攻撃入力処理
    //InputAttack();
    //if (InputAttack())
    //{
    //    TransitionAttackState();
    //}
    ////弾丸入力処理
    //InputProjectile();
}

//移動ステート
void Player::TransitionMoveState()
{
    state = State::Move;

    //アニメーション再生
    //model->PlayAnimation(0, true);
}

//移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
    //移動入力処理
    if (!InputMove(elapsedTime))
    {
        //移動ステートへの遷移
        TransitionIdleState();
    }

    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_B)//x
    {
        TeleportBehindEnemy();
    }

    if (InputDash(elapsedTime))
    {
        TransitionDashState();
    }

    if (InputDashTowardsEnemy(elapsedTime))
    {
        TransitionDashToEnemyState();
    }

    if (InputRecoverySkillEnergy(elapsedTime))
    {
        TransitionRecoverySkillEnergy();
    }
    ////ジャンプ入力処理
    //InputJump();
    //if (InputJump() == true)
    //{
    //    //ジャンプステートへの遷移
    //    TransitionJumpState();
    //}

    ////攻撃入力処理
    //InputAttack();
    //if (InputAttack())
    //{
    //    TransitionAttackState();
    //}
    ////弾丸入力処理
    //InputProjectile();
}

//ダッシュステート
void Player::TransitionDashState()
{
    state = State::Dash;
}

//ダッシュステート更新処理
void Player::UpdateDashState(float elapsedTime)
{
    InputDash(elapsedTime);
    if (!InputDash(elapsedTime))
    {
        TransitionIdleState();
    }
}

//敵へダッシュステート
void Player::TransitionDashToEnemyState()
{
    state = State::DashToEnemy;

    //アニメーション再生
    //model->PlayAnimation(0, true);
}

//敵へダッシュステート更新処理
void Player::UpdateDashToEnemyState(float elapsedTime)
{

    InputDashTowardsEnemy(elapsedTime);
    if (!InputDashTowardsEnemy(elapsedTime))
    {
        TransitionMoveState();
    }

}

// 技力回復
void Player::TransitionRecoverySkillEnergy()
{
    state = State::RecoverySkillEnergy;
}

// 技力回復更新処理
void Player::UpdateRecoverySkillEnergyState(float elapsedTime)
{
    InputRecoverySkillEnergy(elapsedTime);
    if (!InputRecoverySkillEnergy(elapsedTime))
    {
        TransitionIdleState();
    }
}