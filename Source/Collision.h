#pragma once
#include <DirectXMath.h>

class Collision
{
public:
	// ‹…‚Æ‹…‚ÌŒğ·”»’è
	static bool IntersectSphereAndSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec
	);

	// ‰~’Œ‚Æ‰~’Œ‚ÌŒğ·”»’è
	static bool IntersectCylinderAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// ‹…‚Æ‰~’Œ‚ÌŒğ·”½‰f
	static bool IntersectSphereAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);
};