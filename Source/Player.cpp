#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
Player::Player()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Jammo//Jammo.fbx");

	const float scale_factor = 0.01f;
	scale = { scale_factor,scale_factor,scale_factor };
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

    // プレイヤーと敵との衝突処置
    CollisionPlayerAndEnemies();

    // 速度処理更新
    UpdateVelocity(elapsedTime);

	// ワールド行列更新
	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
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
    debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });
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

    Move(elapsedTime, moveVec.x, moveVec.z, this->moveSpeed);

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

// 着地したときに呼び出される
void Player::onLanding()
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
        if (Collision::IntersectSphereAndSphere(
            position,
            radius,
            (enemy->GetPosition()),
            enemy->GetRadius(),
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
            enemy->SetPosition(pos);
        }
    }
}