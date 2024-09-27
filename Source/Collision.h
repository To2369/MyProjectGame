#pragma once
#include <DirectXMath.h>
#include "Graphics/Model.h"

// �q�b�g���ʁi���C�L���X�g��Ɏ擾���������j
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };	// ���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0,0,0 }; // �Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f; // ���C�̎��_�����_�܂ł̋���
	int materialIndex = -1; // �Փ˂����|���S���̃}�e���A���ԍ�
};

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

	// �~���Ɖ~���̌�������
	static bool IntersectCylinderAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// ���Ɖ~���̌������f
	static bool IntersectSphereAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// ���C�ƃ��f���̌�������
	static bool IntersectRayAndModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result);
};