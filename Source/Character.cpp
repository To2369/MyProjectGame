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

//移動処理
void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
    //移動処理
    float moveSpeed = speed * elapsedTime;
    position.x += vx * moveSpeed;
    position.z += vz * moveSpeed;
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
    // 重力処理（フレーム単位で計算）
    velocity.y += gravity * elapsedTime * 60.0f;

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
            onLanding();
        }
        groundedFlag = true;
    }
    else
    {
        groundedFlag = false;
    }
}