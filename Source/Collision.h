#pragma once
#include <DirectXMath.h>
#include "Graphics/Model.h"

// ヒット結果（レイキャスト後に取得したい情報）
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };	// レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0,0,0 }; // 衝突したポリゴンの法線ベクトル
	float distance = 0.0f; // レイの視点から交点までの距離
	int materialIndex = -1; // 衝突したポリゴンのマテリアル番号
};

class Collision
{
public:
	// 球と球の交差判定
	static bool IntersectSphereAndSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec
	);

	// 円柱と円柱の交差判定
	static bool IntersectCylinderAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// 球と円柱の交差反映
	static bool IntersectSphereAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// レイとモデルの交差判定
	static bool IntersectRayAndModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result);
};