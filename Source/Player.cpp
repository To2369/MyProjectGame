#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
Player::Player()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Jammo//Jammo.fbx");

	const float scale_factor = 0.01f;
	scale = { scale_factor,scale_factor,scale_factor };

    //bulletMgr = std::make_unique<BulletManager>();

    // ヒットエフェクト読み込み
    //hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

}

Player::~Player()
{
   
}

void Player::Update(float elapsedTime)
{
    //移動入力処理
    InputMove(elapsedTime);

    // ジャンプ入力処理
    InputJump();

    // 入力による弾発射処理
    InputLaunchBullet();

    // プレイヤーと敵との衝突処置
    CollisionPlayerAndEnemies();

    // 弾と敵の衝突処理
    CollisionBulletsAndEnemies();

    // 速度処理更新
    UpdateVelocity(elapsedTime);

    // 弾更新処理
    bulletMgr.Update(elapsedTime);

	// ワールド行列更新
	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);

    bulletMgr.Render(dc);
}

void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &position.x);
            //回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //スケール
            ImGui::InputFloat3("Scale", &scale.x);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
}

void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });

    // 衝突判定用のデバッグ用円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });

    // 弾デバッグプリミティブ描画
    bulletMgr.DrawDebugPrimitive();
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
void Player::InputMove(float elapsedTime)
{
    //進行ベクトルを取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, this->turnSpeed);
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
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    std::unique_ptr<StraightBullet> bullet;
    // ストレート弾発射
    if (gamePad->GetButtonDown() & GamePad::BTN_X)
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
        StraightBullet* bullet = new StraightBullet(&bulletMgr);
        bullet->Launch(dir, pos);
        //bulletMgr->Regist(bullet.get());
    }

    // ホーミング弾発射
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