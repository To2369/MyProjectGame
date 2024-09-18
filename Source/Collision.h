#pragma once
#include <DirectXMath.h>

class Collision
{
public:
	// ���Ƌ��̌�������
	static bool IntersectSphereAndSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec
	);
};