#include "Character.h"

//行列更新
void Character::UpdateTransform()
{
    //　座標系変換用の行列
    const DirectX::XMFLOAT4X4 coordinate_system_transform[]
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:右手系 Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:左手系 Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:右手系 Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:左手系 Z-UP
    };
    //　デフォルトのスケールファクタを設定して行列に反映
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
    // スケール行列作成
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) };
    // 回転行列作成
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z) };
    // 位置行列作成
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    // 行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W= C * S * R * T;
    //計算したワールド行列をtrabsformに取り出す
    DirectX::XMStoreFloat4x4(&transform, W);

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
    // ２つのベクトルの内積値は-1.0～1.0で表現されます
    float dot = (frontX * vx) + (frontZ * vz);

    // ２つのベクトルが重なったとき、回転速度は0.0fになる
    float rot = (1.0f - dot);

    // あまり離れすぎると回転速度が早くなりすぎるので speed 以上の回転速度にはならないよう制限
    if (rot > speed)
    {
        rot = speed;
    }
    // 外積が正の場合は右回転,負の場合は左回転
    if (cross < 0.0f)
    {
        angle.y -= rot;
    }
    else
    {
        angle.y += rot;
    }
}
// ジャンプ処理
void Character::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y = speed;
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
    velocity.y += gravity * elapsedTime * 60.0f;
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
    // 移動処理
    position.y += velocity.y * elapsedTime;

    // 地面判定
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0;

        // 着地した
        if (!groundedFlag)
        {
            OnLanding();
        }
        groundedFlag = true;
    }
    else
    {
        groundedFlag = false;
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
        }
    }
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // 移動処理
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}