#include "Character.h"
#include "StageManager.h"
#include "Mathf.h"
//行列更新
void Character::UpdateTransform()
{
    // 座標系変換用の行列
    const DirectX::XMFLOAT4X4 coordinate_system_transform[] =
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:右手系 Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:左手系 Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:右手系 Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:左手系 Z-UP
    };
    // デフォルトのスケールファクタを設定して行列に反映
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

    // スケール行列作成
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };


    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    // 位置行列作成
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };

    // 行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = C * S * R * T;

    // 計算したワールド行列をtransformに取り出す
    DirectX::XMStoreFloat4x4(&transform, W);

    right = { transform._11,transform._12,transform._13 };
    up = { transform._21,transform._22,transform._23 };
    front = { transform._31,transform._32,transform._33 };

}

void Character::UpdateAnimation(float elapsedTime)
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
        model->BlendAnimations(current_keyframes, blendRate, model->keyframe);
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

                DirectX::XMVECTOR lerpedTranslation = DirectX::XMVectorLerp(oldPos, newPos, blendRate);
                DirectX::XMStoreFloat3(&localTranslation, lerpedTranslation);
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
    model->totalAnimationTime =
        model->animationClips.at(model->currentAnimationIndex).sequence.size() *
        (1.0f / model->animationClips.at(model->currentAnimationIndex).samplingRate);
    //再生時間が終端時間を超えたら
    if (model->currentAnimationSeconds >= model->totalAnimationTime)
    {
        //再生時間を巻き戻す
        if (model->animationLoop)
        {
            model->currentAnimationSeconds -= model->totalAnimationTime;
        }
        else
        {
            model->currentAnimationSeconds = model->totalAnimationTime;
            model->animationEndFlag = true;
        }
    }
}


void Character::UpdateStatus(float elapsedTime)
{
    if (!useSpiritEnergyFlag)
    {
        //気力を回復する間隔
        energyRecoveryTimer -= elapsedTime;
        if (energyRecoveryTimer <= 0)
        {
            spiritEnergy += spiritHealSpeed;
            energyRecoveryTimer = 0.1f;
        }
    }
    else
    {
        //気力を使用した際、次に回復し始めるまでの時間
        energyNoRecoveryTimer -= elapsedTime;
        if (energyNoRecoveryTimer <= 0)
        {
            energyNoRecoveryTimer = 1.0f;
            energyRecoveryTimer = 0.1f;
            useSpiritEnergyFlag = false;
        }
    }


    //　最大値と最小値固定
    {
        if (health <= 0)
            health = 0;
        if (health >= maxHealth)
            health = maxHealth;

        if (spiritEnergy <= 0)
            spiritEnergy = 0;
        if (spiritEnergy >= maxSpritEnergy)
            spiritEnergy = maxSpritEnergy;

        if (skillEnergy <= 0)
            skillEnergy = 0;
        if (skillEnergy >= maxSkillEnergy)
            skillEnergy = maxSkillEnergy;
    }
}

// 移動方向を決定
void Character::Move(float vx, float vz, float speed)
{
    // 移動方向ベクトルを決定
    direction.x = vx;
    direction.z = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;
    //進べき進行ベクトルがゼロの場合は旋回処理なし
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f)
    {
        return;
    }

    // 進行ベクトルと単位ベクトル化
    vx /= length;
    vz /= length;

    // 前方向ベクトルのXZ成分を取得し単位ベクトル化
    float frontX = transform._31;
    float frontZ = transform._33;
    float frontLength = sqrtf(frontX * frontX + frontZ * frontZ);
    if (frontLength > 0.0f)
    {
        frontX /= frontLength;
        frontZ /= frontLength;
    }

    // 左右チェックの為の外積計算
    float cross = (frontZ * vx) - (frontX * vz);

    // 回転角を求める為、2つの単位ベクトルの内積を計算する
    // ２つのベクトルの内積値は-1.0〜1.0で表現されます
    float dot = (frontX * vx) + (frontZ * vz);

    // ２つのベクトルが重なったとき、回転速度は0.0fになる
    float rot = (1.0f - dot);

    // あまり離れすぎると回転速度が早くなりすぎるので speed 以上の回転速度にはならないよう制限
    if (rot > speed)
    {
        rot = speed;
    }
    // Y軸回りの回転角をクォータニオンに変換
    DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationAxis(
        up, // Y軸
        cross < 0.0f ? -rot : rot                     // 回転方向に応じて符号を変える
    );

    // 現在のクォータニオンに新しい回転を掛け合わせる
    DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&quaternion);
    currentQuat = DirectX::XMQuaternionMultiply(currentQuat, rotationQuat);

    // 結果を保存
    DirectX::XMStoreFloat4(&quaternion, currentQuat);
}
// ジャンプ処理
void Character::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y = speed;
}
void Character::Fly(float elapsedTime)
{
    if (flyingFlag)
    {
        groundedFlag = false;
    }
}
// 速度処理更新
void Character::UpdateVelocity(float elapsedTime)
{
    // 垂直速度更新処理
    UpdateVerticalVelocity(elapsedTime);

    // 垂直移動更新処理
    UpdateVerticalMove(elapsedTime);

    // 水平速度更新処理
    UpdateHorizontalVelocity(elapsedTime);

    // 水平移動更新処理
    UpdateHorizontalMove(elapsedTime);
}

// ダメージ処理
bool Character::ApplyDamage(float invincibleTime,int damage)
{
    // ダメージが０以下の場合は HP に変化なし
    if (damage <= 0)
        return false;

    // 既に HP が０以下（死亡）の場合は HP に変化なし
    if (health <= 0)
        return false;

    // 無敵時間中は HP に変化なし
    if (invincibleTimer > 0.0f)
        return false;

    // ダメージを受けた際に無敵時間を再設定
    invincibleTimer = invincibleTime;

    // ダメージ計算
    health -= damage;

    // ダメージ計算後の残り HP で処理を変化
    if (health <= 0)
    {
        // 死亡
        OnDead();
    }
    else
    {
        // ダメージを受けた
        OnDamaged();
    }

    return true;
}

// 衝撃を与える
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // 速度に力を加える
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}


// 無敵時間の更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

// 垂直速度更新処理
void Character::UpdateVerticalVelocity(float elapsedTime)
{
    
    // 重力処理（フレーム単位で計算）
    if (!flyingFlag)
    {
        velocity.y += gravity * elapsedTime * 60.0f;
    }
    // 空を飛んでいる場合
    else
    {
        velocity.y = flySpeed * elapsedTime * 60.0f;
    }
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
    float moveY = velocity.y * elapsedTime;
    slopeRate = 0.0f;

    // 姿勢制御用法線ベクトル（デフォルトは上方向）
    DirectX::XMFLOAT3 normal = { 0,1,0 };
    if (moveY < 0.0f)
    {
        // 落下中

        // レイの開始位置を設定（足もとより少し上）
        DirectX::XMFLOAT3 start = { position.x, position.y + 2.0f, position.z };
        // レイの終点位置を設定（移動後の位置）
        DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

        // レイキャストによる地面判定
        HitResult hit;
        if (StageManager::Instance().GetStage(0)->RayCast(start, end, hit))
        {
            // 地面の向きを姿勢制御用法線ベクトルに設定
            normal = hit.normal;

            // 地面に設置している（ヒット結果の y 位置を反映）
            position.y = hit.position.y;

            // 着地した
            if (!groundedFlag)
            {
                OnLanding();
            }
            groundedFlag = true;
            velocity.y = 0.0f;
        }
        else
        {
            // 空中に浮いている
            position.y += moveY;
            groundedFlag = false;
        }
    }
    else if (moveY > 0.0f)
    {
        position.y += moveY;
        groundedFlag = false;
    }

    // 垂直方向の移動量
    if (flyingFlag)
    {
        // 飛行中は自由に上下移動
        position.y += moveY;
    }
}

// 水平速度更新処理
void Character::UpdateHorizontalVelocity(float elapsedTime)
{
    // 速度に力が加わっていたら（0 じゃなかったら）減速処理を行う
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // 摩擦力（フレーム単位で計算）
        float friction = this->friction * elapsedTime * 60.0f;

        // 空中にいるときは摩擦力を減少
        if (!groundedFlag)
        {
            friction *= airControl;
        }

        if (length > friction)
        {
            // 摩擦による横方向の減速処理
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            // 横方向の速度が摩擦力以下になったので速度を無効化
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // 最大速度以下なら加速処理を行う
    if (length <= maxMoveSpeed)
    {
        // 方向ベクトルがゼロでないなら加速処理を行う
        // ゼロの場合は入力されていない
        float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
        if (directionLength > 0.0f)
        {
            // 加速度（フレーム単位で計算）
            float acceleration = this->acceleration * elapsedTime * 60.0f;

            // 空中にいるときは加速度を減少
            if (!groundedFlag)
            {
                acceleration *= airControl;
            }

            // 加速処理
            // 方向ベクトルに加速度をスケーリングした値を速度ベクトルに加算
            velocity.x += direction.x * acceleration;
            velocity.z += direction.z * acceleration;

            // 最大速度制限
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                // 方向ベクトルに最大移動速度をスケーリングした値を速度ベクトルに代入
                velocity.x = direction.x * maxMoveSpeed;
                velocity.z = direction.z * maxMoveSpeed;

            }
            
            // 傾斜率が高い場合は落ちていくように計算
            if (groundedFlag && slopeRate > 0.0f)
            {
                velocity.y -= length * slopeRate * elapsedTime * 60.0f;
            }
        }
    }
    direction.x = 0.0f;
    direction.z = 0.0f;
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // 水平速度量計算
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // 計算用の移動後の速度
        float moveX = velocity.x * elapsedTime;
        float moveZ = velocity.z * elapsedTime;

        // レイの始点位置と終点位置
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveX, position.y + 1.0f, position.z + moveZ };

        // レイキャスト
        HitResult hit;
        if (StageManager::Instance().GetStage(0)->SphereCast(start, end, radius, hit))
        {
            DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

            // 壁の法線ベクトル
            DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを逆ベクトルに変換
            vec = DirectX::XMVectorNegate(vec);

            // 入射ベクトルを法線で射影（移動後の位置から壁までの距離）
            DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
            float length;
            DirectX::XMStoreFloat(&length, lengthVec);

            // 補正位置へのベクトルを計算
            DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

            // 最終的な補正位置ベクトルを計算
            DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
            DirectX::XMFLOAT3 correctPos;
            DirectX::XMStoreFloat3(&correctPos, correctPosVec);
            //壁ずり方向へレイキャスト
            HitResult hit2;
            if (StageManager::Instance().GetStage(0)->SphereCast(start, end, radius, hit))
            {
                //壁ずる方向で壁に当たらなかったら補正位置に移動
                position.x = correctPos.x;
                position.z = correctPos.z;
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // 壁にあたっていない場合は通常の移動
            position.x += moveX;
            position.z += moveZ;
        }
    }
}