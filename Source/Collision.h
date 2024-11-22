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

// オブジェクトの交差結果の構造体
struct IntersectionResult
{
	DirectX::XMVECTOR	pointA = {};			// オブジェクトA側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	pointB = {};			// オブジェクトB側の交点。オブジェクトA Vs オブジェクトBで関数が構成される
	DirectX::XMVECTOR	normal = {};			// 交点を結ぶ衝突の単位法線ベクトル。方向はオブジェクトB→オブジェクトA
	float				penetration = 0.0f; 	// 法線ベクトルを元にしためり込み量。交差している場合にプラスの値が返却される
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

	static bool IntersectCapsuleAndCapsule(
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

	//球と円柱の交差判定
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,    //球位置
		float sphereRadius,                         //球の半径
		const DirectX::XMFLOAT3& cylinderPosition,  //円柱位置
		float cylinderRadius,                       //円柱の半径
		float cylinderHeight,                       //円柱の高さ
		DirectX::XMFLOAT3& outCylinderPosition
	);
};