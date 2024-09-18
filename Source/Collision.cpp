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