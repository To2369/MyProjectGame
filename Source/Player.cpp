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
    geo = std::make_unique<GeometricCapsule>(Graphics::Instance()->GetDevice(), height, radius, 12, 6, 6);
    const float scale_factor = 0.01f;
    scale = { scale_factor,scale_factor,scale_factor };
    height = 1.5f;
    stateMachine = std::make_unique<StateMachine<Player>>();

    stateMachine->RegisterState(new MovementState(this));
    stateMachine->RegisterState(new WeakAttackState<Player>(this));
    stateMachine->RegisterState(new StrongAttackState<Player>(this));
    stateMachine->RegisterState(new UseSkillState<Player>(this));
    stateMachine->RegisterState(new HitDamegeState<Player>(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new IdleState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new MoveState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new DashState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new JumpState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new LandState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState01(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState02(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState03(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState04(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState05(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAtkState06(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAtkState01(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAtkState02(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState03(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState04(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState05(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState06(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState07(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::StrongAttack), new StrongAttackState08(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new DashToEnemyState(this));


    stateMachine->RegisterSubState(static_cast<int>(Player::State::UseSkill), new SkillSelectState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::HitDamege), new DamegeState(this));

    // �q�b�g�G�t�F�N�g�ǂݍ���
    hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

    //�ҋ@�X�e�[�g�֑J��
    stateMachine->SetState(static_cast<int>(State::Movement));
    position.y = 5.0f;
    // �J�������[�h�ݒ�
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
    //�X�e�[�g���̏���
    stateMachine->Update(elapsedTime);
    // ���x�����X�V
    UpdateVelocity(elapsedTime);

    //InputFlying(elapsedTime);

    // �v���C���[�ƓG�Ƃ̏Փˏ��u
    CollisionPlayerAndEnemies();

    CollisionArtsAndEnemies();
    //CollisionPlayerAndArts();
    artsMgr.Update(elapsedTime);

    Lockon();

    InputDash(elapsedTime);

   /* Mouse* mouse = InputManager::Instance()->getMouse();
    if (mouse->GetButton() & Mouse::BTN_RIGHT)
    {
        TeleportBehindEnemy();
    }*/

    UpdateStatus(elapsedTime);


    UpdateAnimation(elapsedTime);

	// ���[���h�s��X�V
	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform,{ color });
    artsMgr.Render(dc);
}
float c = 0;
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
    case State::WeakAttack:
        str = "WeakAttack";
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack01))
        {
            subStr = "WeakAttack01";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack02))
        {
            subStr = "WeakAttack02";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack03))
        {
            subStr = "WeakAttack03";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack04))
        {
            subStr = "WeakAttack04";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack05))
        {
            subStr = "WeakAttack05";
        }
        if (stateMachine->GetState()->GetSubStateIndex() == static_cast<int>(Player::WeakAttack::WeakAttack06))
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
        //�g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //�ʒu
            ImGui::InputFloat3("Position", &position.x);
            ImGui::SliderFloat("Position", &position.y,-10,10);
            //��]
            DirectX::XMVECTOR p = DirectX::XMQuaternionRotationAxis(right, c);
            DirectX::XMVECTOR orientationVec= DirectX::XMLoadFloat4(&quaternion);
            orientationVec = DirectX::XMQuaternionMultiply(orientationVec, p);
            // ���ʂ�ۑ�
            DirectX::XMStoreFloat4(&quaternion, orientationVec);
            //DirectX::XMFLOAT4 pquater;
            ImGui::InputFloat("movespeed", &moveSpeed);
      /*      quaternion.x = DirectX::XMConvertToRadians(pquater.x);
            quaternion.y = DirectX::XMConvertToRadians(pquater.y);
            quaternion.z = DirectX::XMConvertToRadians(pquater.z);
            quaternion.w = DirectX::XMConvertToRadians(pquater.w);*/
            ImGui::InputFloat4("quaternion", &quaternion.x);
            ImGui::SliderFloat("quaternion", &c,-1,1);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            ImGui::InputFloat("movespeed", &moveSpeed);
            ImGui::InputFloat("currentAnimationSeconds", &model->currentAnimationSeconds);
            ImGui::InputInt("hit", &hit);
            ImGui::Text(u8"State�@%s", str.c_str());
            ImGui::Text(u8"Subtate�@%s", subStr.c_str());
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

//���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec(const DirectX::XMFLOAT3& cameraRight, const DirectX::XMFLOAT3& cameraFront) const
{
    //���͏����擾
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float ax = gamePad->GetAxisLX();
    float ay = gamePad->GetAxisLY();

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

    //�i�s�x�N�g�����擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec(cameraRight, cameraFront);

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //���񏈗�
    if (lockonState != LockonState::NotLocked)
    {
        //	���b�N�I���������̓��b�N�I���ΏۂɌ�����
        Turn(elapsedTime, cameraFront.x, cameraFront.z, turnSpeed);
    }
    else
    {
        Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    }

    //�i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
    return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

// �W�����v���͏���
bool Player::InputJump()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_A&&!artSkillReady)
    {
        // �W�����v�񐔐���
        if (jumpCount < jumpLimit)
        {
            // �W�����v
            jumpCount++;
            Jump(jumpSpeed);
            return true;
        }
    }
    return false;
}
void Player::InputFlying(float elapsedTime)
{

}

bool Player::InputArts(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    Mouse* mouse = InputManager::Instance()->getMouse();
    artSkillReady = false;
    artUltSkillReady = false;
    // ����Z�⋆�ɋZ�n
    if (gamePad->GetButton() & GamePad::BTN_LEFT_TRIGGER)// R�L�[
    {
        artUltSkillReady = true;
        if (gamePad->GetButtonDown()& GamePad::BTN_B) // Q�L�[
        {
            if (spiritEnergy >= 300)
            {
                DirectX::XMFLOAT3 f;
                DirectX::XMStoreFloat3(&f, front);
                // �O����
                    DirectX::XMFLOAT3 dir;
                    dir.x = f.x;
                    dir.y = 0.0f;
                    dir.z = f.z;


                    // ���ˈʒu�i�v���C���[�̍�������
                    DirectX::XMFLOAT3 pos;
                pos.x = position.x;
                pos.y = position.y + height * 0.5f;
                pos.z = position.z;

                // ����
                ArtsSpiritExplosion* artsSpiritExplosion = new ArtsSpiritExplosion(&artsMgr);
                artsSpiritExplosion->Launch(dir, pos);

                spiritEnergy -= artsSpiritExplosion->GetUseSpiritEnergy();
                return true;
            }
        }
    }
    // �K�E�Z
    else if (gamePad->GetButton() & GamePad::BTN_RIGHT_TRIGGER) // F�L�[
    {
        artSkillReady = true;
        if (mouse->GetButtonDown()&Mouse::BTN_LEFT)
        {
            ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
            if(skillEnergy>=artsSkillStraightBallet->GetUseSkillEnergy())
            {
                DirectX::XMFLOAT3 f;
                DirectX::XMStoreFloat3(&f, front);
                // �O����
                DirectX::XMFLOAT3 dir;
                dir.x = f.x;
                dir.y = 0.0f;
                dir.z = f.z;
                DirectX::XMVECTOR dirVec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir));

                // ���ˈʒu�i�v���C���[�̍�������
                DirectX::XMFLOAT3 pos;
                pos.x = position.x;
                pos.y = position.y + height * 0.5f;
                pos.z = position.z;

                float offsetDistance = 1.5f; // �����O�̋����i�����\�j
                DirectX::XMVECTOR offset = DirectX::XMVectorScale(dirVec, offsetDistance);

                // �I�t�Z�b�g�𔭎ˈʒu�ɉ��Z
                DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
                posVec = DirectX::XMVectorAdd(posVec, offset);
                DirectX::XMStoreFloat3(&pos, posVec);

                // ����
                artsSkillStraightBallet->Launch(dir, pos);
                skillEnergy -= artsSkillStraightBallet->GetUseSkillEnergy();
            }
            return true;
        }
        // �Z�͉�
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
    //�@�ʏ�e
    else if (gamePad->GetButtonDown() & GamePad::BTN_B)
    {
        NormalBallet* normalBallet = new NormalBallet(&artsMgr);
        if (skillEnergy >= normalBallet->GetUseSkillEnergy())
        {
            DirectX::XMFLOAT3 f;
            DirectX::XMStoreFloat3(&f, front);
            // �O����
            DirectX::XMFLOAT3 dir;
            dir.x = f.x;
            dir.y = 0.0f;
            dir.z = f.z;


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
            normalBallet->Launch(dir, pos, target);
            normalBallet->LockonTarget(target);
            skillEnergy -= normalBallet->GetUseSkillEnergy();
            return true;
        }
    }
    return false;
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
            //OutputDebugStringA("�Փ�\n");

            dashTowardEnemyFlag = false;

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
        else
        {
        }
    }
}

void Player::CollisionArtsAndEnemies()
{
    EnemyManager& enemyMgr = EnemyManager::Instance();

    // �S�Ă̒e�ƑS�Ă̓G�𑍓�����ŏՓˏ���
    int artsCount = artsMgr.GetArtsCount();
    int enemyCount = enemyMgr.GetEnemyCount();
    for (int i = 0; i < artsCount; ++i)
    {
        Arts* arts = artsMgr.GetArts(i);

        for (int j = 0; j < enemyCount; j++)
        {
            Enemy* enemy = enemyMgr.GetEnemy(j);


            // �Փˏ���
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
                        // ������΂�
                        float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        impulse.x = outPos.x * power;
                        impulse.y = power * 0.5f;
                        impulse.z = outPos.z * power;

                        enemy->AddImpulse(impulse);

                        // �q�b�g�G�t�F�N�g�̍Đ�
                        DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                        enePos.y += enemy->GetHeight() * 0.5f;
                        //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        // �e�̔j��
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
                    {0,1,0},
                    enemy->GetHeight(),
                    enemy->GetRadius(),
                    result))
                {
                    if (enemy->ApplyDamage(0.5f, arts->GetDamage()))
                    {
                        // ������΂�
                      /*  float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        impulse.x = result->penetration * power;
                        impulse.y = power * 0.5f;
                        impulse.z = result->penetration * power;*/

                        //enemy->AddImpulse(impulse);

                        // �q�b�g�G�t�F�N�g�̍Đ�
                        DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                        enePos.y += enemy->GetHeight() * 0.5f;
                        //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        // �e�̔j��
                        arts->Destroy();
                    }
                }
            }
        }
    }
}

void Player::CollisionPlayerAndArts()
{
    // �S�Ă̒e�ƑS�Ă̓G�𑍓�����ŏՓˏ���
    int artsCount = artsMgr.GetArtsCount();
    IntersectionResult result;
    for (int i = 0; i < artsCount; ++i)
    {
        Arts* arts = artsMgr.GetArts(i);

        if (arts->GetType() == BalletType::SkillStraight)
        {
            //�v���C��[����1.5f
            // �Փˏ���
            DirectX::XMFLOAT3 outVec;
            DirectX::XMFLOAT3 dir = { 0,1,0 };
            DirectX::XMFLOAT3 plPos = position;
            //plPos.y += height / 2;
            //float h = height / 2;
            if (Collision::IntersectCapsuleVsCapsule(
                DirectX::XMLoadFloat3(&plPos),
                DirectX::XMLoadFloat3(&dir),
                height,
                radius,
                DirectX::XMLoadFloat3(&arts->GetPosition()),
                DirectX::XMLoadFloat3(&arts->GetDirection()),
                arts->GetHeight(),
                arts->GetRadius(),
                &result))
            {
                // �v���C���[���G�ɉ����o����鏈��
                DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(result.normal, result.penetration);
                DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&position);
                newPosition = DirectX::XMVectorAdd(newPosition, pushVec);
                DirectX::XMStoreFloat3(&position, newPosition); // �V�����ʒu��position�ɔ��f
                hit = 1;
            }
            else
            {
                hit = 0;
            }
        }
    }
}

void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius, float Timer, int damage)
{
    atkCollisionFlag = true;
    //�m�[�h�擾
    Skeleton::Bone* bone = model->FindNode(nodeName);
    auto& node = model->keyframe.nodes[bone->nodeIndex];
    auto WorldTransform = DirectX::XMLoadFloat4x4(&node.globalTransform) * DirectX::XMLoadFloat4x4(&transform);
    DirectX::XMFLOAT4X4 worldTransform;
    DirectX::XMStoreFloat4x4(&worldTransform, WorldTransform);
    // �m�[�h�ʒu�擾
    DirectX::XMFLOAT3 nodePosition;
    nodePosition.x = worldTransform._41;
    nodePosition.y = worldTransform._42;
    nodePosition.z = worldTransform._43;
    EnemyManager& enemy = EnemyManager::Instance();
    int count = EnemyManager::Instance().GetEnemyCount();
    for (int i = 0; i < count; i++)
    {
        Enemy* enemys = enemy.GetEnemy(i);

        // �Փˏ���
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
            if (enemys->ApplyDamage(Timer, damage))
            {  //hitEffect->Play(e);
                // �m�b�N�o�b�N���� = �G�ʒu - �v���C���[�ʒu
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

                // �v���C���[�����̔��Ε����ɏ��������O�i
                this->AddImpulse(impulse);

                enemys->AddImpulse(impulse);
                {
                    DirectX::XMFLOAT3 e = enemys->GetPosition();
                    hitEffect->Play(&e);
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
                //  �O���̃{�[���̃m�[�h�̎s�������擾
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
            spiritEnergy -= useEnergy;
            useSpiritEnergyFlag = true;

            // �G�̌��ɔ��
            DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();

            // �G�̑O���������v�Z (�G�̌����Ă������������)
            DirectX::XMFLOAT3 enemyForward = {};
            DirectX::XMStoreFloat3(&enemyForward,closestEnemy->GetFront());
            enemyForward.y = 0.0f;

            // �G�̌��̈ʒu = �G�̈ʒu - �O������ * 2.0f (�����𒲐�)
            const float distanceBehindEnemy = 2.0f; // �v���C���[���G������ɔ�ԋ���
            DirectX::XMFLOAT3 teleportPos;
            teleportPos.x = enemyPos.x - (enemyForward.x * distanceBehindEnemy);
            teleportPos.y = enemyPos.y;
            teleportPos.z = enemyPos.z - (enemyForward.z * distanceBehindEnemy);

            // �v���C���[�̈ʒu���X�V
            position = teleportPos;

            // �v���C���[���G�̕��������悤�ɃN�H�[�^�j�I�����g�p���ĉ�]
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);

            // �G�̕��ւ̕����x�N�g�����v�Z
            DirectX::XMVECTOR toEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            toEnemy = DirectX::XMVector3Normalize(toEnemy);

            // �����x�N�g�������]�s����v�Z
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixLookAtLH(playerPosVec, toEnemy, up);

            // ��]�s�񂩂�N�H�[�^�j�I�����v�Z
            DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationMatrix(-rotationMatrix);
            

            // �v���C���[�̉�]�ɃN�H�[�^�j�I����K�p
            DirectX::XMStoreFloat4(&quaternion, rotationQuat);
        }
    }
}

bool Player::InputDashTowardsEnemy(float elapsedTime)
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float useEnergy = 5;
    // X�{�^���Ń_�b�V��
    if (gamePad->GetButtonDown() & GamePad::BTN_X)
    {
        // X�{�^���Ń_�b�V��
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
            skillEnergyTimer -= elapsedTime;
            if (skillEnergyTimer <= 0)
            {
                skillEnergy -= useEnergy;
                skillEnergyTimer = useEnergyTimer;
            }

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

            // �G�̕������������߂̉�]�N�H�[�^�j�I����ݒ�
            DirectX::XMVECTOR cross = DirectX::XMVector3Cross(front, directionVec); // �O���ƃ^�[�Q�b�g�����̊O��
            float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(front, directionVec)); // �O���ƃ^�[�Q�b�g�����̓���

            if (!DirectX::XMVector3Equal(cross, DirectX::XMVectorZero()))
            {
                // �O�ς��[���łȂ��ꍇ�A�ʏ�̉�]���ŃN�H�[�^�j�I���𐶐�
                float angleRadians = acosf(dot); // ���ς���p�x�����߂�
                DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Normalize(cross), angleRadians);

                // �v���C���[�̉�]���X�V�i�N�H�[�^�j�I���Ƃ��Đݒ�j
                DirectX::XMStoreFloat4(&quaternion, rotationQuaternion);
            }
            else
            {
                // �O�ς��[���̏ꍇ�i���S�ɓ������܂��͔��Ε����j
                if (dot > 0.0f)
                {
                    // �������̏ꍇ�A��]�s�v�i�P�ʃN�H�[�^�j�I���j
                    quaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
                }
                else
                {
                    // ���Ε����̏ꍇ�A180�x��]
                    DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(
                        up, DirectX::XM_PI); // Y�������180�x��]
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
    if (mouse->GetButtonDown() & Mouse::BTN_LEFT&&!artSkillReady)
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

void Player::InputAttackNext(float currentAnimaSeconds, float inputAcceptStartTime, float inputAcceptEndTime)
{
    if (currentAnimaSeconds >= inputAcceptStartTime
        && currentAnimaSeconds <= inputAcceptEndTime)
    {
        if (InputWeekAttack())
        {
            SetWeekAtkNextFlag(true);
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
        atkCollisionFlag = false;
    }

    if (attackData.secondHitBoneName != nullptr)
    {
        if (model->currentAnimationSeconds >= attackData.secondHitStartTime
            && model->currentAnimationSeconds <= attackData.secondHitEndTime)
        {
            CollisionNodeVsEnemies(
                attackData.secondHitBoneName,
                attackData.hitRadius,
                invincibleTimer,
                attackData.damage);
        }
        else
        {
            atkCollisionFlag = false;
        }
    }
}

void Player::ActiveSecondAttackCollider(AttackData attackData)
{
    if (model->currentAnimationSeconds >= attackData.secondHitStartTime
        && model->currentAnimationSeconds <= attackData.secondHitEndTime)
    {
        CollisionNodeVsEnemies(
            attackData.secondHitBoneName,
            attackData.hitRadius,
            invincibleTimer,
            attackData.damage);
    }
    else
    {
        atkCollisionFlag = false;
    }
}