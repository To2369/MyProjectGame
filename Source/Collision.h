#pragma once
#include <DirectXMath.h>
#include "Graphics/Model.h"
#include <DirectXCollision.h>
#include <iostream>

// ヒット結果（レイキャスト後に取得したい情報）
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };	// レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0,0,0 };	// 衝突したポリゴンの法線ベクトル
	float distance = 0.0f;					// レイの視点から交点までの距離
	int materialIndex = -1;					// 衝突したポリゴンのマテリアル番号
};

// オブジェクトの交差結果の構造体
struct IntersectionResult
{
	DirectX::XMVECTOR	pointA = {};			// オブジェクトA側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	pointB = {};			// オブジェクトB側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	normal = {};			// 交点を結ぶ衝突の単位法線ベクトル。方向はオブジェクトB→オブジェクトA
	float				penetration = 0.0f; 	// 法線ベクトルを元にしためり込み量。交差している場合にプラスの値が返却される
};

struct AABB
{
	DirectX::XMFLOAT3 min; // 最小点
	DirectX::XMFLOAT3 max; // 最大点

	// AABBの中心を返す
	DirectX::XMFLOAT3 GetCenter() const
	{
		return DirectX::XMFLOAT3((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
	}

	// AABBがレイと交差するかどうかを判定する
	bool IntersectsRay(const DirectX::XMFLOAT3& rayOrigin, const DirectX::XMFLOAT3& rayDirection, float& tMin, float& tMax) const
	{
		using namespace DirectX;

		XMVECTOR rayOrig = XMLoadFloat3(&rayOrigin);
		XMVECTOR rayDir = XMLoadFloat3(&rayDirection);

		XMVECTOR boxMin = XMLoadFloat3(&min);
		XMVECTOR boxMax = XMLoadFloat3(&max);

		// AABBとレイの交差判定を行う
		for (int i = 0; i < 3; ++i)
		{
			float origin = reinterpret_cast<const float*>(&rayOrig)[i];
			float direction = reinterpret_cast<const float*>(&rayDir)[i];

			float boxMinValue = reinterpret_cast<const float*>(&boxMin)[i];
			float boxMaxValue = reinterpret_cast<const float*>(&boxMax)[i];

			float t1 = (boxMinValue - origin) / direction;
			float t2 = (boxMaxValue - origin) / direction;

			if (t1 > t2) std::swap(t1, t2);
			tMin = (tMin > t1) ? tMin : t1; // 最大値を取得
			tMax = (tMax < t2) ? tMax : t2; // 最小値を取得

			if (tMin > tMax)
				return false;
		}

		return true;
	}

	bool IntersectRaySphere(
		const DirectX::XMFLOAT3& rayStart,
		const DirectX::XMFLOAT3& rayEnd,
		const DirectX::XMFLOAT3& sphereCenter,
		float sphereRadius)
	{
		using namespace DirectX;

		XMVECTOR s = XMLoadFloat3(&rayStart);
		XMVECTOR e = XMLoadFloat3(&rayEnd);
		XMVECTOR c = XMLoadFloat3(&sphereCenter);

		XMVECTOR d = XMVectorSubtract(e, s);        // レイの方向ベクトル
		XMVECTOR m = XMVectorSubtract(s, c);        // 球中心へのベクトル

		float a = XMVectorGetX(XMVector3Dot(d, d));
		float b = 2.0f * XMVectorGetX(XMVector3Dot(m, d));
		float c_val = XMVectorGetX(XMVector3Dot(m, m)) - sphereRadius * sphereRadius;

		float discriminant = b * b - 4.0f * a * c_val;

		if (discriminant < 0.0f)
			return false; // 交差なし

		discriminant = std::sqrt(discriminant);

		float t1 = (-b - discriminant) / (2.0f * a);
		float t2 = (-b + discriminant) / (2.0f * a);

		// レイの範囲 [0,1] 内に交差があるか
		return (t1 >= 0.0f && t1 <= 1.0f) || (t2 >= 0.0f && t2 <= 1.0f);
	}
};

// 光線の定義
struct Ray
{
	DirectX::XMFLOAT3	p;	// 光線の始点
	DirectX::XMFLOAT3	d;	// 光線の方向
	float				l;	// 光線の長さ
};

struct Triangle
{
	DirectX::XMFLOAT3	p0;
	DirectX::XMFLOAT3	p1;
	DirectX::XMFLOAT3	p2;
};

struct Sphere
{
	DirectX::XMFLOAT3	p;
	DirectX::XMFLOAT3	d;	// 光線の方向
	float				l;	// 光線の長さ
	float r;
};
class Collision
{
public:
	// 球と球の交差判定
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec
	);

	// 円柱と円柱の交差判定
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// カプセルとカプセルの交差判定
	static bool IntersectCapsuleVsCapsule(
		const DirectX::XMVECTOR& position1,	// 中心
		const DirectX::XMVECTOR& direction1,	// 向き（正規化）
		const float					length1,	// 長さ
		const float					radius1,	// 半径
		const DirectX::XMVECTOR& position2,	// 中心
		const DirectX::XMVECTOR& direction2,	// 向き（正規化）
		const float					length2,	// 長さ
		const float					radius2,	// 半径
		IntersectionResult* result);

	// 線分と線分の最短距離の二乗を取得する
	static float GetMinDistSq_SegmentSegment(
		const  DirectX::XMVECTOR& point1A,
		const  DirectX::XMVECTOR& point1B,
		const  DirectX::XMVECTOR& point2A,
		const  DirectX::XMVECTOR& point2B,
		DirectX::XMVECTOR* nearPoint1,
		DirectX::XMVECTOR* nearPoint2);

	// 球と円柱の交差反映
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// レイとモデルの交差判定
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		DirectX::XMFLOAT4X4 modelTransform,
		HitResult& result);

	/*static bool Collision::IntersectSphereRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const float radius,
		const Model* model,
		HitResult& result)*/
};