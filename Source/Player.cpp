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

    // �q�b�g�G�t�F�N�g�ǂݍ���
    //hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

}

Player::~Player()
{
   
}

void Player::Update(float elapsedTime)
{

    // ���x�����X�V
    UpdateVelocity(elapsedTime);

    //�ړ����͏���
    InputMove(elapsedTime);

    // �W�����v���͏���
    InputJump();

    // ���͂ɂ��e���ˏ���
    InputLaunchBullet();

    InputTeleportBehindEnemy();

    InputDashTowardsEnemy(elapsedTime);
    // �v���C���[�ƓG�Ƃ̏Փˏ��u
    CollisionPlayerAndEnemies();

    // �e�ƓG�̏Փˏ���
    CollisionBulletsAndEnemies();

    // �e�X�V����
    bulletMgr.Update(elapsedTime);

    model->UpdateAnimation(elapsedTime);


	// ���[���h�s��X�V
	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);

    bulletMgr.Render(dc);
}

void Player::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        //�g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //�ʒu
            ImGui::InputFloat3("Position", &position.x);
            //��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
#endif
}

void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });

    // �Փ˔���p�̃f�o�b�O�p�~����`��
    debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });

    // �e�f�o�b�O�v���~�e�B�u�`��
    bulletMgr.DrawDebugPrimitive();
}

//���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    //���͏����擾
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float ax = gamePad->GetAxisLX();
    float ay = gamePad->GetAxisLY();

    //�J�����������擾
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    //�ړ��x�N�g����XZ���ʂȃx�N�g���ɂȂ�悤�ɂ���
    //�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }
    //�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength < 0.0f)
    {
        //�P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }
    //�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f��
    //�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f��
    //�i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
    vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
    //Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;
    return vec;
}

//����ړ�
void Player::InputMove(float elapsedTime)
{
    //�i�s�x�N�g�����擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, this->turnSpeed);
}

// �W�����v���͏���
void Player::InputJump()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_A)
    {
        // �W�����v�񐔐���
        if (jumpCount < jumpLimit)
        {
            // �W�����v
            jumpCount++;
            Jump(jumpSpeed);
        }
    }
}

// ���͂ɂ��e���ˏ���
void Player::InputLaunchBullet()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    std::unique_ptr<StraightBullet> bullet;
    // �X�g���[�g�e����
    //if (gamePad->GetButtonDown() & GamePad::BTN_X)
    //{
    //    // �O����
    //    DirectX::XMFLOAT3 dir;
    //    dir.x = sinf(angle.y);
    //    dir.y = 0.0f;
    //    dir.z = cosf(angle.y);

    //    // ���ˈʒu�i�v���C���[�̍�������
    //    DirectX::XMFLOAT3 pos;
    //    pos.x = position.x;
    //    pos.y = position.y + height * 0.5f;
    //    pos.z = position.z;

    //    // ����
    //    StraightBullet* bullet = new StraightBullet(&bulletMgr);
    //    bullet->Launch(dir, pos);
    //    //bulletMgr->Regist(bullet.get());
    //}

    // �z�[�~���O�e����
    if (gamePad->GetButtonDown() & GamePad::BTN_Y)
    {
        // �O����
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // ���ˈʒu�i�v���C���[�̍�������
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // �^�[�Q�b�g
        DirectX::XMFLOAT3 target;
        target.x = pos.x + dir.x * 1000.0f;
        target.y = pos.y + dir.y * 1000.0f;
        target.z = pos.z + dir.z * 1000.0f;

        // ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
        float dist = FLT_MAX;
        EnemyManager& enemyMgr = EnemyManager::Instance();
        int enemyCount = enemyMgr.GetEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // �G�Ƃ̋����𔻒�
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

        // ����
        HomingBullet* bullet = new HomingBullet(&bulletMgr);
        bullet->Launch(dir, pos, target);
        bullet->LockonTarget(target);
    }
}

// ���n�����Ƃ��ɌĂяo�����
void Player::OnLanding()
{
    jumpCount = 0;
}

// �v���C���[�ƓG�Ƃ̏Փˏ���
void Player::CollisionPlayerAndEnemies()
{
    EnemyManager& eneMgr = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = eneMgr.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = eneMgr.GetEnemy(i);

        // �Փ˕���
        DirectX::XMFLOAT3 outVec;

        // �Փˏ���
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
            //OutputDebugStringA("�Փ�\n");

            dash = false;

            // ���a�̍��v
            float range = radius + enemy->GetRadius();

            // �Փ˕����̃x�N�g���𔼌a�̍��v�̒����ɃX�P�[�����O
            DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

            // ���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
            vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

            // ���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
            DirectX::XMFLOAT3 pos;
            DirectX::XMStoreFloat3(&pos, vec);
            pos.y = enemy->GetPosition().y;
            enemy->SetPosition(pos);
        }
    }
}

// �e�ƓG�̏Փˏ���
void Player::CollisionBulletsAndEnemies()
{
    EnemyManager& enemyMgr = EnemyManager::Instance();

    // �S�Ă̒e�ƑS�Ă̓G�𑍓�����ŏՓˏ���
    int bulletCount = bulletMgr.GetBulletCount();
    int enemyCount = enemyMgr.GetEnemyCount();
    for (int i = 0; i < bulletCount; ++i)
    {
        Bullet* bullet = bulletMgr.GetBullet(i);

        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyMgr.GetEnemy(j);

            // �Փˏ���
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
                    // ������΂�
                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse;
                    impulse.x = outVec.x * power;
                    impulse.y = power * 0.5f;
                    impulse.z = outVec.z * power;

                    enemy->AddImpulse(impulse);

                    // �q�b�g�G�t�F�N�g�̍Đ�
                    DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                    enePos.y += enemy->GetHeight() * 0.5f;
                    //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                    // �e�̔j��
                    bullet->Destroy();
                }
            }
        }
    }
}

void Player::InputTeleportBehindEnemy()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_B)//x
    {
        // ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
        float minDistance = FLT_MAX;
        Enemy* closestEnemy = nullptr;
        EnemyManager& enemyMgr = EnemyManager::Instance();
        int enemyCount = enemyMgr.GetEnemyCount();

        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyMgr.GetEnemy(i);

            // �G�Ƃ̋������v�Z
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
            // �G�̌��ɔ��
            DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();

            // �G�̑O���������v�Z (�G�̌����Ă������������)
            DirectX::XMFLOAT3 enemyForward;
            enemyForward.x = sinf(closestEnemy->GetAngle().y);
            enemyForward.y = 0.0f;
            enemyForward.z = cosf(closestEnemy->GetAngle().y);

            // �G�̌��̈ʒu = �G�̈ʒu - �O������ * 2.0f (�����𒲐�)
            const float distanceBehindEnemy = 2.0f; // �v���C���[���G������ɔ�ԋ���
            DirectX::XMFLOAT3 teleportPos;
            teleportPos.x = enemyPos.x - (enemyForward.x * distanceBehindEnemy);
            teleportPos.y = enemyPos.y;
            teleportPos.z = enemyPos.z - (enemyForward.z * distanceBehindEnemy);

            // �v���C���[�̈ʒu���X�V
            position = teleportPos;

            // �v���C���[���G�̕��������悤�ɉ�]
            angle.y = closestEnemy->GetAngle().y;
        }
    }
}

void Player::InputDashTowardsEnemy(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();

    // X�{�^���Ń_�b�V��
    if (gamePad->GetButtonDown() & GamePad::BTN_X)
    {
        if (dash)
        {
            dash = false;
            return;
        }
        dash = true;
    }

    if (dash)
    {
        // ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
        float minDistance = FLT_MAX;
        Enemy* closestEnemy = nullptr;
        EnemyManager& enemyMgr = EnemyManager::Instance();
        int enemyCount = enemyMgr.GetEnemyCount();

        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = enemyMgr.GetEnemy(i);

            // �G�Ƃ̋������v�Z
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
            // �G�̕������v�Z
            DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR directionVec = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            directionVec = DirectX::XMVector3Normalize(directionVec);

            // �_�b�V�����x��ݒ� (���X�Ɉړ����鑬�x)
            float dashSpeed = 10.0f; // �_�b�V���̑����i�t���[���P�ʁj
            DirectX::XMVECTOR dashVec = DirectX::XMVectorScale(directionVec, dashSpeed * elapsedTime);

            // �v���C���[�̈ʒu���X�V (���X�Ɉړ�)
            DirectX::XMVECTOR newPositionVec = DirectX::XMVectorAdd(playerPosVec, dashVec);
            DirectX::XMStoreFloat3(&position, newPositionVec);

            // ��ɓG�̕����������悤�ɉ�]
            DirectX::XMVECTOR forwardVec = DirectX::XMVectorSet(0, 0, 1, 0);  // �v���C���[�̌��݂̑O���x�N�g��
            DirectX::XMVECTOR rightVec = DirectX::XMVectorSet(1, 0, 0, 0);    // �v���C���[�̉E�����x�N�g��
            DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0, 1, 0, 0);       // ������x�N�g��

            // �G�ւ̕����x�N�g����Y�������݂̂��g�p���ĉ�]���v�Z�iY����]�j
            DirectX::XMVECTOR projectedDirectionVec = DirectX::XMVector3Normalize(
                DirectX::XMVectorSet(DirectX::XMVectorGetX(directionVec), 0, DirectX::XMVectorGetZ(directionVec), 0)
            );

            // Y������̉�]�p�x���v�Z
            float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(forwardVec, projectedDirectionVec));
            float det = DirectX::XMVectorGetX(DirectX::XMVector3Dot(rightVec, projectedDirectionVec));
            float angleRadians = atan2(det, dot);

            // Y����]���X�V
            angle.y = angleRadians;
        }
    }
}