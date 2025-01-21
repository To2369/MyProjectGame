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

    // ヒットエフェクト読み込み
    //hitEffect = std::make_unique<Effect>(".//Data//Effect//01_AndrewFM01//hit_eff.efk");

    //待機ステートへ遷移
    stateMachine->SetState(static_cast<int>(State::Movement));
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

    //InputMove(elapsedTime);

    //InputFlying(elapsedTime);

    // 入力による弾発射処理
   /* InputLaunchBullet();

    InputArts();

    InputAttack();*/
    //InputArts();
    //InputMove(elapsedTime);
    // プレイヤーと敵との衝突処置
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

	// ワールド行列更新
	UpdateTransform();

    p = position;
    p.y += height / 2;
}
void Player::UpdateAnimation(float elapsedTime)
{
    //再生中でないなら処理しない
    if (!model->IsPlayAnimation())
        return;

    // ブレンド補間にかかる時間
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

    // アニメーションの番号
    int clip_index{ model->currentAnimationIndex };

    int frame_index{ 0 }, old_frame_index{ 0 };
    Animation& animation{ model->animationClips.at(clip_index) };
    frame_index = static_cast<int>(model->currentAnimationSeconds * animation.samplingRate);
    old_frame_index = static_cast<int>(model->oldAnimationSeconds * animation.samplingRate);
    if (frame_index > animation.sequence.size() - 1)
    {
        frame_index = animation.sequence.size() - 1;
    }
    if (old_frame_index > animation.sequence.size() - 1)
    {
        old_frame_index = animation.sequence.size() - 1;
    }

    Animation::Keyframe beginPose, oldPose, newPose;
    Animation::Keyframe endPose;
    if (blendRate < 1.0f)
    {
        // ブレンド再生
        const Animation::Keyframe* begin_keyframes[2]{
            &model->keyframe,
            // 今回アニメーションの最初のフレームを最後として補完
            &model->animationClips.at(clip_index).sequence.at(0),
        };
        const Animation::Keyframe* old_keyframes[2]{
            &model->keyframe,
            // 今回アニメーションの最初のフレームを最後として補完
            &model->animationClips.at(clip_index).sequence.at(old_frame_index),
        };
        const Animation::Keyframe* current_keyframes[2]{
            &model->keyframe,
            // 今回アニメーションの最初のフレームを最後として補完
            &model->animationClips.at(clip_index).sequence.at(frame_index),
        };
        const Animation::Keyframe* end_keyframes[2]{
            &model->keyframe,
            // 今回アニメーションの最初のフレームを最後として補完
            &model->animationClips.at(clip_index).sequence.at(animation.sequence.size() - 1),
        };

        // ブレンド補間
        model->BlendAnimations(begin_keyframes, blendRate, beginPose);
        model->BlendAnimations(old_keyframes, blendRate, oldPose);
        model->BlendAnimations(current_keyframes, blendRate, newPose);
        model->BlendAnimations(end_keyframes, blendRate, endPose);
    }
    else
    {
        // 通常再生
        model->keyframe = animation.sequence.at(frame_index);

        beginPose = animation.sequence.at(0);
        oldPose = animation.sequence.at(old_frame_index);
        newPose = animation.sequence.at(frame_index);
        endPose = animation.sequence.at(animation.sequence.size() - 1);
    }
#if 0
    {
        // キーフレームに存在するすべてのノードを更新する
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ローカル行列を設定
            animation::keyframe::node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // 親のグローバル行列を取得
            int64_t parent_index{ model->scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).global_transform) };

            // ローカル行列 * 親のグローバル行列
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

#endif

    //ルートモーション
    {
        const Model::Mesh* mesh = model->FindMesh("root");
        const Skeleton::Bone* bone = model->FindNode("root");
        //const Model::mesh* mesh = model->FindMesh("pelvis");
        //const skeleton::bone* bone = model->FindNode("pelvis");
        if (bone && !model->keyframe.nodes.empty())
        {
            const int rootMotionIndex = bone->nodeIndex;
            DirectX::XMFLOAT3 localTranslation{};
            if (model->oldAnimationSeconds > model->currentAnimationSeconds)
            {
                //終端位置
                DirectX::XMVECTOR EndPos =
                    DirectX::XMLoadFloat3(&endPose.nodes[rootMotionIndex].translation);
                //前回位置
                DirectX::XMVECTOR oldPos =
                    DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);
                //今回位置
                DirectX::XMVECTOR newPos =
                    DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                //初回位置
                DirectX::XMVECTOR beginPos =
                    DirectX::XMLoadFloat3(&beginPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR endToOldPos = DirectX::XMVectorSubtract(oldPos, EndPos);
                DirectX::XMVECTOR beginToNowPos = DirectX::XMVectorSubtract(beginPos, newPos);
                DirectX::XMStoreFloat3(&localTranslation,
                    DirectX::XMVectorAdd(endToOldPos, beginToNowPos));
            }
            else
            {
                //前回位置
                DirectX::XMVECTOR oldPos = DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR newPos = DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                DirectX::XMVECTOR newPosToOldPos = DirectX::XMVectorSubtract(newPos, oldPos);
                DirectX::XMStoreFloat3(&localTranslation, newPosToOldPos);
            }
            //親ノード取得
            DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMMatrixIdentity();
            if (bone->parentIndex >= 0)
            {
                //rootmotionNodeの親ノード(parent)からグローバル行列を取得できるので
                //ローカル移動量をグローバル空間に移動させる
                const Skeleton::Bone& rootmotionNode = mesh->bindPose.bones[bone->parentIndex];
                ParentGlobalTransform = DirectX::XMLoadFloat4x4(&model->keyframe.nodes[rootmotionNode.nodeIndex].globalTransform);
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
                DirectX::XMStoreFloat3(&model->keyframe.nodes[bone->nodeIndex].translation, LocalPos);

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
        // キーフレームに存在するすべてのノードを更新する
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ローカル行列を設定
            Animation::Keyframe::Node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // 親のグローバル行列を取得
            int64_t parent_index{ model->sceneView.nodes.at(node_index).parentIndex };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).globalTransform) };

            // ローカル行列 * 親のグローバル行列
            DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
        }
    }

    //最終フレーム処理
    if (model->animationEndFlag)
    {
        model->animationEndFlag = false;
        model->currentAnimationIndex = -1;
        return;
    }
    //時間経過
    model->oldAnimationSeconds = model->currentAnimationSeconds;
    model->currentAnimationSeconds += elapsedTime;

    // アニメーション全体の長さを計算
    totalAnimationTime =
        model->animationClips.at(model->currentAnimationIndex).sequence.size() *
        (1.0f / model->animationClips.at(model->currentAnimationIndex).samplingRate);
    //再生時間が終端時間を超えたら
    if (model->currentAnimationSeconds >= totalAnimationTime)
    {
        //再生時間を巻き戻す
        if (model->animationLoop)
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
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &position.x);
            ImGui::SliderFloat("Position", &position.y,-10,10);
            //回転
            DirectX::XMVECTOR p = DirectX::XMQuaternionRotationAxis(right, c);
            DirectX::XMVECTOR orientationVec= DirectX::XMLoadFloat4(&quaternion);
            orientationVec = DirectX::XMQuaternionMultiply(orientationVec, p);
            // 結果を保存
            DirectX::XMStoreFloat4(&quaternion, orientationVec);
            //DirectX::XMFLOAT4 pquater;
            ImGui::InputFloat("movespeed", &moveSpeed);
      /*      quaternion.x = DirectX::XMConvertToRadians(pquater.x);
            quaternion.y = DirectX::XMConvertToRadians(pquater.y);
            quaternion.z = DirectX::XMConvertToRadians(pquater.z);
            quaternion.w = DirectX::XMConvertToRadians(pquater.w);*/
            ImGui::InputFloat4("quaternion", &quaternion.x);
            ImGui::SliderFloat("quaternion", &c,-1,1);
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
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 1, 1 });

    // 衝突判定用のデバッグ用円柱を描画
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
    Skeleton::Bone* bone = model->FindNode("ik_hand_l");
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
            angle,
            0.1f,
            DirectX::XMFLOAT4(0, 1, 0, 1)
        );
    }
    //artsMgr.DrawDebugPrimitive();
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
bool Player::InputJump()
{
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    if (gamePad->GetButtonDown() & GamePad::BTN_A&&!artSkillReady)
    {
        // ジャンプ回数制限
        if (jumpCount < jumpLimit)
        {
            // ジャンプ
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
    // 回避技や究極技系
    if (gamePad->GetButton() & GamePad::BTN_LEFT_TRIGGER)// Rキー
    {
        artUltSkillReady = true;
        if (gamePad->GetButtonDown()& GamePad::BTN_B) // Qキー
        {
            ArtsSpiritExplosion* artsSpiritExplosion = new ArtsSpiritExplosion(&artsMgr);
            if (spiritEnergy >= artsSpiritExplosion->GetUseSpiritEnergy())
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
        if (mouse->GetButtonDown()&Mouse::BTN_LEFT)
        {
            ArtsSkillStraightBallet* artsSkillStraightBallet = new  ArtsSkillStraightBallet(&artsMgr);
            if(skillEnergy>=artsSkillStraightBallet->GetUseSkillEnergy())
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
                        //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        // 弾の破棄
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
                        // 吹き飛ばし
                      /*  float power = 10.0f;
                        DirectX::XMFLOAT3 impulse;
                        impulse.x = result->penetration * power;
                        impulse.y = power * 0.5f;
                        impulse.z = result->penetration * power;*/

                        //enemy->AddImpulse(impulse);

                        // ヒットエフェクトの再生
                        DirectX::XMFLOAT3 enePos = enemy->GetPosition();
                        enePos.y += enemy->GetHeight() * 0.5f;
                        //Effekseer::Handle handle = hitEffect->Play(&enePos, 0.5f);

                        // 弾の破棄
                        arts->Destroy();
                    }
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

        if (arts->GetType() == BalletType::SkillStraight)
        {
            //プレイやー高さ1.5f
            // 衝突処理
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
                // プレイヤーが敵に押し出される処理
                DirectX::XMVECTOR pushVec = DirectX::XMVectorScale(result.normal, result.penetration);
                DirectX::XMVECTOR newPosition = DirectX::XMLoadFloat3(&position);
                newPosition = DirectX::XMVectorAdd(newPosition, pushVec);
                DirectX::XMStoreFloat3(&position, newPosition); // 新しい位置をpositionに反映
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
    for (int i = 0; i < count; i++)
    {
        Enemy* enemys = enemy.GetEnemy(i);

        //衝突処理
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
                //吹き飛ばす移動方向の速度ベクトル
                DirectX::XMFLOAT3 impulse;
                //吹き飛ばす力
                const float power = 1.0f;

                //敵の位置
                DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemys->GetPosition());
                //弾の位置
                DirectX::XMVECTOR projectileVec = DirectX::XMLoadFloat3(&nodePosition);
                //弾から敵への方向ベクトルを計算(敵-弾
                auto Vec = DirectX::XMVectorSubtract(enemyVec, projectileVec);
                //方向ベクトルを正規化
                Vec = DirectX::XMVector3Normalize(Vec);
                DirectX::XMFLOAT3 v;
                DirectX::XMStoreFloat3(&v, Vec);

                //吹き飛ばす移動方向の速度ベクトルに設定
                impulse.x = power * v.x;
                impulse.y = power * 0.5f;
                impulse.z = power * v.z;

                enemys->AddImpulse(impulse);
            }
            //ヒットエフェクト再生
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

            // プレイヤーが敵の方を向くようにクォータニオンを使用して回転
            DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);

            // 敵の方への方向ベクトルを計算
            DirectX::XMVECTOR toEnemy = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
            toEnemy = DirectX::XMVector3Normalize(toEnemy);

            // 方向ベクトルから回転行列を計算
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixLookToLH(playerPosVec, toEnemy, up);

            // 回転行列からクォータニオンを計算
            DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationMatrix(rotationMatrix);

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
    /// 一番近くの敵をターゲットに設定
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
        // 敵の方向を計算
        DirectX::XMFLOAT3 enemyPos = closestEnemy->GetPosition();
        DirectX::XMVECTOR enemyPosVec = DirectX::XMLoadFloat3(&enemyPos);
        DirectX::XMVECTOR playerPosVec = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR directionVec = DirectX::XMVectorSubtract(enemyPosVec, playerPosVec);
        directionVec = DirectX::XMVector3Normalize(directionVec);

        // 敵の方向を向くための回転クォータニオンを設定
        DirectX::XMVECTOR cross = DirectX::XMVector3Cross(front, directionVec);
        float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(front, directionVec));

        // ターゲット方向への回転クォータニオンを求める
        DirectX::XMVECTOR targetQuaternion;
        if (!DirectX::XMVector3Equal(cross, DirectX::XMVectorZero()))
        {
            // 外積がゼロでない場合、通常の回転軸でクォータニオンを生成
            float angleRadians = acosf(dot);
            targetQuaternion = DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Normalize(cross), angleRadians);
        }
        else
        {
            // 外積がゼロの場合、完全に同方向または反対方向
            if (dot > 0.0f)
            {
                targetQuaternion = DirectX::XMQuaternionIdentity();  // 同方向の場合、回転不要
            }
            else
            {
                targetQuaternion = DirectX::XMQuaternionRotationAxis(up, DirectX::XM_PI); // 反対方向の場合、180度回転
            }
        }

        // スムーズに補間して回転を設定する（例えば0.1の割合で補間）
        DirectX::XMVECTOR currentQuaternion = DirectX::XMLoadFloat4(&quaternion);
        DirectX::XMVECTOR smoothedQuaternion = DirectX::XMQuaternionSlerp(currentQuaternion, targetQuaternion, 0.1f);
        DirectX::XMStoreFloat4(&quaternion, smoothedQuaternion);
    }
}

////待機ステート更新処理
//void Player::UpdateIdleState(float elapsedTime)
//{
//    //移動入力処理
//    InputMove(elapsedTime);
//    if (InputMove(elapsedTime))
//    {
//        //移動ステートへの遷移
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
//    ////ジャンプ入力処理
//    //InputJump();
//    //if (InputJump() == true)
//    //{
//    //    //ジャンプステートへの遷移
//    //    TransitionJumpState();
//    //}
//
//    //攻撃入力処理
//    InputAttack();
//    if (InputAttack())
//    {
//        TransitionAttackState();
//    }
//    ////弾丸入力処理
//    //InputProjectile();
//}

//void IdleState<Player>::Enter()
//{
//    this->owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
//}
//void IdleState<Player>::Execute(float elapsedTime)
//{
//}
////移動ステート
//void Player::TransitionMoveState()
//{
//    state = State::Move;
//
//    //アニメーション再生
//    model->PlayAnimation(0, true);
//}
//
////移動ステート更新処理
//void Player::UpdateMoveState(float elapsedTime)
//{
//    //移動入力処理
//    if (!InputMove(elapsedTime))
//    {
//        //移動ステートへの遷移
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
//    ////ジャンプ入力処理
//    //InputJump();
//    //if (InputJump() == true)
//    //{
//    //    //ジャンプステートへの遷移
//    //    TransitionJumpState();
//    //}
//
//    ////攻撃入力処理
//    //InputAttack();
//    //if (InputAttack())
//    //{
//    //    TransitionAttackState();
//    //}
//    ////弾丸入力処理
//    //InputProjectile();
//}
//
////ダッシュステート
//void Player::TransitionDashState()
//{
//    state = State::Dash;
//}
//
////ダッシュステート更新処理
//void Player::UpdateDashState(float elapsedTime)
//{
//    InputDash(elapsedTime);
//    if (!InputDash(elapsedTime))
//    {
//        TransitionIdleState();
//    }
//}
//
////敵へダッシュステート
//void Player::TransitionDashToEnemyState()
//{
//    state = State::DashToEnemy;
//
//    //アニメーション再生
//    //model->PlayAnimation(0, true);
//}
//
////敵へダッシュステート更新処理
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
//// 技力回復
//void Player::TransitionRecoverySkillEnergy()
//{
//    state = State::RecoverySkillEnergy;
//}
//
//// 技力回復更新処理
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
