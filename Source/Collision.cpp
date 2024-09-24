#include "Collision.h"

// 球と球の交差判定
bool Collision::IntersectSphereAndSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    // ２点間の位置ベクトル引く
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);
    // 単一の長さを計算
    DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
    // float 型に変換
    float len;
    DirectX::XMStoreFloat(&len, length);

    // 距離判定
    float range = radiusA + radiusB;
    if (len > range)
    {
        return false;
    }

    // 衝突していたら衝突方向のベクトルを設定
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&outVec, vec);

    return true;
}

// 円柱と円柱の交差判定
bool Collision::IntersectCylinderAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A の足もとが B の頭より上なら衝突していない
    if (positionA.y > positionB.y + heightB)
        return false;

    // A の頭が B の足もとより下なら衝突していない
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

// 球と円柱の交差反映
bool Collision::IntersectSphereAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A（球）の足もとが B（円柱）の頭より上なら衝突していない
    if (positionA.y - radiusA > positionB.y + heightB)
        return false;

    // A（球）の頭が B（円柱）の足もとより下なら衝突していない
    if (positionA.y + radiusA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}