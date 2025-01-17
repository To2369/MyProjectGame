#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ArtsSpiritExplosion.h"
#include "ArtsSkillStraightBallet.h"
#include "StateDerived.h"
#include "NormalBallet.h"
Player::Player()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\pl\\Character1.fbx");
    geo= std::make_unique<GeometricCapsule>(Graphics::Instance()->GetDevice(), height, radius, 12, 6, 6);
	const float scale_factor = 0.01f;
	scale = { scale_factor,scale_factor,scale_factor };
    height=1.5f;
    stateMachine = std::make_unique<StateMachine>();

    stateMachine->RegisterState(new MovementState(this));
    stateMachine->RegisterState(new WeakAttackState(this));
    stateMachine->RegisterState(new UseSkillState(this));
    stateMachine->RegisterState(new HitDamegeState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new IdleState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new MoveState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new DashState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new JumpState(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::Movement), new LandState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState01(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState02(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState03(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState04(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState05(this));
    stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new WeakAttackState06(this));
    //stateMachine->RegisterSubState(static_cast<int>(Player::State::WeakAttack), new DashToEnemyState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::UseSkill), new SkillSelectState(this));

    stateMachine->RegisterSubState(static_cast<int>(Player::State::HitDamege), new DamegeState(this));
    //bulletMgr = std::make_unique<BulletManager>();

    // �q�b�g�G�t�F�N�g�ǂݍ���
    //hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

    //�ҋ@�X�e�[�g�֑J��
    stateMachine->SetState(static_cast<int>(State::Movement));
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

    //InputMove(elapsedTime);

    //InputFlying(elapsedTime);

    // ���͂ɂ��e���ˏ���
   /* InputLaunchBullet();

    InputArts();

    InputAttack();*/
    //InputArts();
    //InputMove(elapsedTime);
    // �v���C���[�ƓG�Ƃ̏Փˏ��u
    CollisionPlayerAndEnemies();

    CollisionArtsAndEnemies();
    //CollisionPlayerAndArts();
    artsMgr.Update(elapsedTime);
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    {
        
        
    }

    //Lock();

    UpdateStatus(elapsedTime);


    UpdateAnimation(elapsedTime);

	// ���[���h�s��X�V
	UpdateTransform();

    p = position;
    p.y += height / 2;
}
void Player::UpdateAnimation(float elapsedTime)
{
    //�Đ����łȂ��Ȃ珈�����Ȃ�
    if (!model->IsPlayAnimation())
        return;

    // �u�����h��Ԃɂ����鎞��
    float blendRate = 1.0f;
    if (model->animationBlendTime < model->animationBlendSeconds)
    {
        model->animationBlendTime += elapsedTime;
        if (model->animationBlendTime >= model->animationBlendSeconds)
        {
            model->animationBlendTime = model->animationBlendSeconds;
        }
        blendRate = model->animationBlendTime / model->animationBlendSeconds;
        blendRate *= blendRate;
    }

    // �A�j���[�V�����̔ԍ�
    int clip_index{ model->currentAnimationIndex };

    int frame_index{ 0 }, old_frame_index{ 0 };
    animation& animation{ model->animation_clips.at(clip_index) };
    frame_index = static_cast<int>(model->currentAnimationSeconds * animation.sampling_rate);
    old_frame_index = static_cast<int>(model->oldAnimationSeconds * animation.sampling_rate);
    if (frame_index > animation.sequence.size() - 1)
    {
        frame_index = animation.sequence.size() - 1;
    }
    if (old_frame_index > animation.sequence.size() - 1)
    {
        old_frame_index = animation.sequence.size() - 1;
    }

    animation::keyframe beginPose, oldPose, newPose;
    animation::keyframe endPose;
    if (blendRate < 1.0f)
    {
        // �u�����h�Đ�
        const animation::keyframe* begin_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animation_clips.at(clip_index).sequence.at(0),
        };
        const animation::keyframe* old_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animation_clips.at(clip_index).sequence.at(old_frame_index),
        };
        const animation::keyframe* current_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animation_clips.at(clip_index).sequence.at(frame_index),
        };
        const animation::keyframe* end_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animation_clips.at(clip_index).sequence.at(animation.sequence.size() - 1),
        };

        // �u�����h���
        model->BlendAnimations(begin_keyframes, blendRate, beginPose);
        model->BlendAnimations(old_keyframes, blendRate, oldPose);
        model->BlendAnimations(current_keyframes, blendRate, newPose);
        model->BlendAnimations(end_keyframes, blendRate, endPose);
    }
    else
    {
        // �ʏ�Đ�
        model->keyframe = animation.sequence.at(frame_index);

        beginPose = animation.sequence.at(0);
        oldPose = animation.sequence.at(old_frame_index);
        newPose = animation.sequence.at(frame_index);
        endPose = animation.sequence.at(animation.sequence.size() - 1);
    }
#if 0
    {
        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            animation::keyframe::node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ model->scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).global_transform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

#endif

    //���[�g���[�V����
    {
        const Model::mesh* mesh = model->FindMesh("root");
        const skeleton::bone* bone = model->FindNode("root");
        //const Model::mesh* mesh = model->FindMesh("pelvis");
        //const skeleton::bone* bone = model->FindNode("pelvis");
        if (bone && !model->keyframe.nodes.empty())
        {
            const int rootMotionIndex = bone->node_index;
            DirectX::XMFLOAT3 localTranslation{};
            if (model->oldAnimationSeconds > model->currentAnimationSeconds)
            {
                //�I�[�ʒu
                DirectX::XMVECTOR EndPos =
                    DirectX::XMLoadFloat3(&endPose.nodes[rootMotionIndex].translation);
                //�O��ʒu
                DirectX::XMVECTOR oldPos =
                    DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);
                //����ʒu
                DirectX::XMVECTOR newPos =
                    DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                //����ʒu
                DirectX::XMVECTOR beginPos =
                    DirectX::XMLoadFloat3(&beginPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR endToOldPos = DirectX::XMVectorSubtract(oldPos, EndPos);
                DirectX::XMVECTOR beginToNowPos = DirectX::XMVectorSubtract(beginPos, newPos);
                DirectX::XMStoreFloat3(&localTranslation,
                    DirectX::XMVectorAdd(endToOldPos, beginToNowPos));
            }
            else
            {
                //�O��ʒu
                DirectX::XMVECTOR oldPos = DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR newPos = DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                DirectX::XMVECTOR newPosToOldPos = DirectX::XMVectorSubtract(newPos, oldPos);
                DirectX::XMStoreFloat3(&localTranslation, newPosToOldPos);
            }
            //�e�m�[�h�擾
            DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMMatrixIdentity();
            if (bone->parent_index >= 0)
            {
                //rootmotionNode�̐e�m�[�h(parent)����O���[�o���s����擾�ł���̂�
                //���[�J���ړ��ʂ��O���[�o����ԂɈړ�������
                const skeleton::bone& rootmotionNode = mesh->bind_pose.bones[bone->parent_index];
                ParentGlobalTransform = DirectX::XMLoadFloat4x4(&model->keyframe.nodes[rootmotionNode.node_index].global_transform);
            }

            DirectX::XMVECTOR GlobalTranslation;
            GlobalTranslation = DirectX::XMVector3TransformNormal(XMLoadFloat3(&localTranslation), ParentGlobalTransform);

            {
                GlobalTranslation = DirectX::XMVectorSetY(GlobalTranslation, 0);
                DirectX::XMVECTOR LocalPos, GlobalPos;
                LocalPos = DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                GlobalPos = DirectX::XMVector3Transform(LocalPos, ParentGlobalTransform);
                GlobalPos = DirectX::XMVectorSetX(GlobalPos, 0);
                GlobalPos = DirectX::XMVectorSetZ(GlobalPos, 0);
                DirectX::XMMATRIX InverceParentGlobalTransform;
                InverceParentGlobalTransform = XMMatrixInverse(nullptr, ParentGlobalTransform);
                LocalPos = XMVector3Transform(GlobalPos, InverceParentGlobalTransform);
                DirectX::XMStoreFloat3(&model->keyframe.nodes[bone->node_index].translation, LocalPos);

                DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&transform);
                DirectX::XMFLOAT3 worldTranslation;
                DirectX::XMVECTOR ConvertWordtransform;
                ConvertWordtransform = DirectX::XMVector3TransformNormal(GlobalTranslation, WorldTransform);
                DirectX::XMStoreFloat3(&worldTranslation, ConvertWordtransform);

                position.x += worldTranslation.x;
                position.y += worldTranslation.y;
                position.z += worldTranslation.z;
            }
        }
    }
    {
        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            animation::keyframe::node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ model->scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).global_transform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

    //�ŏI�t���[������
    if (model->animationEndFlag)
    {
        model->animationEndFlag = false;
        model->currentAnimationIndex = -1;
        return;
    }
    //���Ԍo��
    model->oldAnimationSeconds = model->currentAnimationSeconds;
    model->currentAnimationSeconds += elapsedTime;

    // �A�j���[�V�����S�̂̒������v�Z
    totalAnimationTime =
        model->animation_clips.at(model->currentAnimationIndex).sequence.size() *
        (1.0f / model->animation_clips.at(model->currentAnimationIndex).sampling_rate);
    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (model->currentAnimationSeconds >= totalAnimationTime)
    {
        //�Đ����Ԃ������߂�
        if (model->animLoop)
        {
            model->currentAnimationSeconds -= totalAnimationTime;
        }
        else
        {
            model->currentAnimationSeconds = totalAnimationTime;
            model->animationEndFlag = true;
        }
    }
}


void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform,{ 1.0f,1.0f,1.0f,1.0f });
    artsMgr.Render(dc);
    //ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
    //artsSkillStraightBallet->Render(dc);//Launch(dir, pos);
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
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });

    // �Փ˔���p�̃f�o�b�O�p�~����`��
    //debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });

    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    //debugPrimitive->DrawSphere(position, radius, { 0,0,1,1 });
    //debugPrimitive->DrawCube(position, {1,1,1}, { 1,1,1,1 });
    debugPrimitive->DrawCylinder(position, angle,radius, height, { 1,1,1,1 });
    {
        //debugPrimitive->DrawCapsule(p, angle,radius, height, { 1,1,1,1 });
       /* if(a==1)
        debugPrimitive->DrawCapsule(position, { radius,radius,radius }, height, { 1,1,1,1 });*/
    }
    skeleton::bone* bone = model->FindNode("ik_hand_l");
    if (bone && !model->keyframe.nodes.empty())
    {
        //  �O���̃{�[���̃m�[�h�̎s�������擾
        auto& node = model->keyframe.nodes[bone->node_index];
        auto WorldTransform = DirectX::XMLoadFloat4x4(&node.global_transform) * DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMFLOAT4X4 worldTransform;
        DirectX::XMStoreFloat4x4(&worldTransform, WorldTransform);
        debugPrimitive->DrawSphere(DirectX::XMFLOAT3(
            worldTransform._41,
            worldTransform._42,
            worldTransform._43),
            angle,
            0.1f,
            DirectX::XMFLOAT4(0, 1, 0, 1)
        );
    }
    //artsMgr.DrawDebugPrimitive();
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
bool Player::InputMove(float elapsedTime)
{
    //�i�s�x�N�g�����擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x, moveVec.z, this->moveSpeed);

    //���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, this->turnSpeed);


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
    // ����Z�⋆�ɋZ�n
    if (gamePad->GetButton() & GamePad::BTN_LEFT_TRIGGER)// R�L�[
    {
        artSkillReady = true;
        if (gamePad->GetButtonDown()& GamePad::BTN_B) // Q�L�[
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
    // �K�E�Z
    else if (gamePad->GetButton() & GamePad::BTN_RIGHT_TRIGGER) // F�L�[
    {
        artSkillReady = true;
        if (mouse->GetButtonDown()&Mouse::BTN_LEFT)
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
            ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
            artsSkillStraightBallet->Launch(dir, pos);
            skillEnergy -= artsSkillStraightBallet->GetUseSkillEnergy();
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
        NormalBallet* normalBallet = new NormalBallet(&artsMgr);
        normalBallet->Launch(dir, pos,target);
        normalBallet->LockonTarget(target);
        skillEnergy -= normalBallet->GetUseSkillEnergy();
        return true;
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
                if (Collision::IntersectSphereAndCylinder(
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
                if (Collision::IntersectCapsuleAndCapsule(
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
            if (Collision::IntersectCapsuleAndCapsule(
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

void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
    //�m�[�h�擾
    skeleton::bone* bone = model->FindNode(nodeName);
    auto& node = model->keyframe.nodes[bone->node_index];
    auto WorldTransform = DirectX::XMLoadFloat4x4(&node.global_transform) * DirectX::XMLoadFloat4x4(&transform);
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

        //�Փˏ���
        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectSphereVsCylinder(
            nodePosition,
            nodeRadius,
            enemys->GetPosition(),
            enemys->GetRadius(),
            enemys->GetHeight(),
            outPosition
        ))
        {
            {
                //������΂��ړ������̑��x�x�N�g��
                DirectX::XMFLOAT3 impulse;
                //������΂���
                const float power = 1.0f;

                //�G�̈ʒu
                DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemys->GetPosition());
                //�e�̈ʒu
                DirectX::XMVECTOR projectileVec = DirectX::XMLoadFloat3(&nodePosition);
                //�e����G�ւ̕����x�N�g�����v�Z(�G-�e
                auto Vec = DirectX::XMVectorSubtract(enemyVec, projectileVec);
                //�����x�N�g���𐳋K��
                Vec = DirectX::XMVector3Normalize(Vec);
                DirectX::XMFLOAT3 v;
                DirectX::XMStoreFloat3(&v, Vec);

                //������΂��ړ������̑��x�x�N�g���ɐݒ�
                impulse.x = power * v.x;
                impulse.y = power * 0.5f;
                impulse.z = power * v.z;

                enemys->AddImpulse(impulse);
            }
            //�q�b�g�G�t�F�N�g�Đ�
            if (enemys->ApplyDamage(1, 1))
            {
                DirectX::XMFLOAT3 e = enemys->GetPosition();
                e.y += enemys->GetHeight() * 0.5f;
                //hitEffect->Play(e);
            }
        }
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

            // �v���C���[���G�̕��������悤�ɃN�H�[�^�j�I�����g�p���ĉ�]
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);

            // �G�̕��ւ̕����x�N�g�����v�Z
            DirectX::XMVECTOR toEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            toEnemy = DirectX::XMVector3Normalize(toEnemy);

            // �����x�N�g�������]�s����v�Z
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixLookToLH(playerPosVec, toEnemy, up);

            // ��]�s�񂩂�N�H�[�^�j�I�����v�Z
            DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationMatrix(rotationMatrix);

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

bool Player::InputAttack()
{
    Mouse* mouse = InputManager::Instance()->getMouse();
    if (mouse->GetButtonDown() & Mouse::BTN_LEFT&&!artSkillReady)
    {
        return true;
    }
    return false;
}

void Player::Lock()
{
    /// ��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
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

        // �G�̕������������߂̉�]�N�H�[�^�j�I����ݒ�
        DirectX::XMVECTOR cross = DirectX::XMVector3Cross(front, directionVec);
        float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(front, directionVec));

        // �^�[�Q�b�g�����ւ̉�]�N�H�[�^�j�I�������߂�
        DirectX::XMVECTOR targetQuaternion;
        if (!DirectX::XMVector3Equal(cross, DirectX::XMVectorZero()))
        {
            // �O�ς��[���łȂ��ꍇ�A�ʏ�̉�]���ŃN�H�[�^�j�I���𐶐�
            float angleRadians = acosf(dot);
            targetQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Normalize(cross), angleRadians);
        }
        else
        {
            // �O�ς��[���̏ꍇ�A���S�ɓ������܂��͔��Ε���
            if (dot > 0.0f)
            {
                targetQuaternion = DirectX::XMQuaternionIdentity();  // �������̏ꍇ�A��]�s�v
            }
            else
            {
                targetQuaternion = DirectX::XMQuaternionRotationAxis(up, DirectX::XM_PI); // ���Ε����̏ꍇ�A180�x��]
            }
        }

        // �X���[�Y�ɕ�Ԃ��ĉ�]��ݒ肷��i�Ⴆ��0.1�̊����ŕ�ԁj
        DirectX::XMVECTOR currentQuaternion = DirectX::XMLoadFloat4(&quaternion);
        DirectX::XMVECTOR smoothedQuaternion = DirectX::XMQuaternionSlerp(currentQuaternion, targetQuaternion, 0.1f);
        DirectX::XMStoreFloat4(&quaternion, smoothedQuaternion);
    }
}

////�ҋ@�X�e�[�g�X�V����
//void Player::UpdateIdleState(float elapsedTime)
//{
//    //�ړ����͏���
//    InputMove(elapsedTime);
//    if (InputMove(elapsedTime))
//    {
//        //�ړ��X�e�[�g�ւ̑J��
//        TransitionMoveState();
//    }
//
//    GamePad* gamePad = InputManager::Instance()->getGamePad();
//    if (gamePad->GetButtonDown() & GamePad::BTN_B)//x
//    {
//        TeleportBehindEnemy();
//    }
//
//    if (InputDashTowardsEnemy(elapsedTime))
//    {
//        TransitionDashToEnemyState();
//    }
//    if (InputRecoverySkillEnergy(elapsedTime))
//    {
//        TransitionRecoverySkillEnergy();
//    }
//    ////�W�����v���͏���
//    //InputJump();
//    //if (InputJump() == true)
//    //{
//    //    //�W�����v�X�e�[�g�ւ̑J��
//    //    TransitionJumpState();
//    //}
//
//    //�U�����͏���
//    InputAttack();
//    if (InputAttack())
//    {
//        TransitionAttackState();
//    }
//    ////�e�ۓ��͏���
//    //InputProjectile();
//}

//void IdleState<Player>::Enter()
//{
//    this->owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
//}
//void IdleState<Player>::Execute(float elapsedTime)
//{
//}
////�ړ��X�e�[�g
//void Player::TransitionMoveState()
//{
//    state = State::Move;
//
//    //�A�j���[�V�����Đ�
//    model->PlayAnimation(0, true);
//}
//
////�ړ��X�e�[�g�X�V����
//void Player::UpdateMoveState(float elapsedTime)
//{
//    //�ړ����͏���
//    if (!InputMove(elapsedTime))
//    {
//        //�ړ��X�e�[�g�ւ̑J��
//        TransitionIdleState();
//    }
//
//    GamePad* gamePad = InputManager::Instance()->getGamePad();
//    if (gamePad->GetButtonDown() & GamePad::BTN_B)//x
//    {
//        TeleportBehindEnemy();
//    }
//
//    if (InputDash(elapsedTime))
//    {
//        TransitionDashState();
//    }
//
//    if (InputDashTowardsEnemy(elapsedTime))
//    {
//        TransitionDashToEnemyState();
//    }
//
//    if (InputRecoverySkillEnergy(elapsedTime))
//    {
//        TransitionRecoverySkillEnergy();
//    }
//    ////�W�����v���͏���
//    //InputJump();
//    //if (InputJump() == true)
//    //{
//    //    //�W�����v�X�e�[�g�ւ̑J��
//    //    TransitionJumpState();
//    //}
//
//    ////�U�����͏���
//    //InputAttack();
//    //if (InputAttack())
//    //{
//    //    TransitionAttackState();
//    //}
//    ////�e�ۓ��͏���
//    //InputProjectile();
//}
//
////�_�b�V���X�e�[�g
//void Player::TransitionDashState()
//{
//    state = State::Dash;
//}
//
////�_�b�V���X�e�[�g�X�V����
//void Player::UpdateDashState(float elapsedTime)
//{
//    InputDash(elapsedTime);
//    if (!InputDash(elapsedTime))
//    {
//        TransitionIdleState();
//    }
//}
//
////�G�փ_�b�V���X�e�[�g
//void Player::TransitionDashToEnemyState()
//{
//    state = State::DashToEnemy;
//
//    //�A�j���[�V�����Đ�
//    //model->PlayAnimation(0, true);
//}
//
////�G�փ_�b�V���X�e�[�g�X�V����
//void Player::UpdateDashToEnemyState(float elapsedTime)
//{
//
//    InputDashTowardsEnemy(elapsedTime);
//    if (!InputDashTowardsEnemy(elapsedTime))
//    {
//        TransitionMoveState();
//    }
//
//}
//
//// �Z�͉�
//void Player::TransitionRecoverySkillEnergy()
//{
//    state = State::RecoverySkillEnergy;
//}
//
//// �Z�͉񕜍X�V����
//void Player::UpdateRecoverySkillEnergyState(float elapsedTime)
//{
//    InputRecoverySkillEnergy(elapsedTime);
//    if (!InputRecoverySkillEnergy(elapsedTime))
//    {
//        TransitionIdleState();
//    }
//}
//
//void Player::TransitionAttackState()
//{
//    state = State::Attack;
//    model->PlayAnimation(FullAttack, false);
//}
//
//void Player::UpdateAttackState(float elapsedTime)
//{
//
//    if (!model->IsPlayAnimation())
//    {
//        TransitionIdleState();
//    }
//    CollisionNodeVsEnemies("ik_hand_l", 5.0f);
//}
