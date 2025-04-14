#pragma once
#include <DirectXMath.h>
#include "Graphics/Model.h"
#include <DirectXCollision.h>
#include <algorithm>  // std::min ���g�����߂ɕK�v
#include <iostream>

// �q�b�g���ʁi���C�L���X�g��Ɏ擾���������j
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };	// ���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0,0,0 }; // �Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f; // ���C�̎��_�����_�܂ł̋���
	int materialIndex = -1; // �Փ˂����|���S���̃}�e���A���ԍ�
};

// �I�u�W�F�N�g�̌������ʂ̍\����
struct IntersectionResult
{
	DirectX::XMVECTOR	pointA = {};			// �I�u�W�F�N�gA���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
	DirectX::XMVECTOR	pointB = {};			// �I�u�W�F�N�gB���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
	DirectX::XMVECTOR	normal = {};			// ��_�����ԏՓ˂̒P�ʖ@���x�N�g���B�����̓I�u�W�F�N�gB���I�u�W�F�N�gA
	float				penetration = 0.0f; 	// �@���x�N�g�������ɂ����߂荞�ݗʁB�������Ă���ꍇ�Ƀv���X�̒l���ԋp�����
};

struct AABB
{
	DirectX::XMFLOAT3 min; // �ŏ��_
	DirectX::XMFLOAT3 max; // �ő�_

	// AABB�̒��S��Ԃ�
	DirectX::XMFLOAT3 GetCenter() const
	{
		return DirectX::XMFLOAT3((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
	}

	// AABB�����C�ƌ������邩�ǂ����𔻒肷��
	bool IntersectsRay(const DirectX::XMFLOAT3& rayOrigin, const DirectX::XMFLOAT3& rayDirection, float& tMin, float& tMax) const
	{
		using namespace DirectX;

		XMVECTOR rayOrig = XMLoadFloat3(&rayOrigin);
		XMVECTOR rayDir = XMLoadFloat3(&rayDirection);

		XMVECTOR boxMin = XMLoadFloat3(&min);
		XMVECTOR boxMax = XMLoadFloat3(&max);

		// AABB�ƃ��C�̌���������s��
		for (int i = 0; i < 3; ++i)
		{
			float origin = reinterpret_cast<const float*>(&rayOrig)[i];
			float direction = reinterpret_cast<const float*>(&rayDir)[i];

			float boxMinValue = reinterpret_cast<const float*>(&boxMin)[i];
			float boxMaxValue = reinterpret_cast<const float*>(&boxMax)[i];

			float t1 = (boxMinValue - origin) / direction;
			float t2 = (boxMaxValue - origin) / direction;

			if (t1 > t2) std::swap(t1, t2);
			tMin = (tMin > t1) ? tMin : t1; // �ő�l���擾
			tMax = (tMax < t2) ? tMax : t2; // �ŏ��l���擾

			if (tMin > tMax)
				return false;
		}

		return true;
	}
};

class Collision
{
public:
	// ���Ƌ��̌�������
	static bool IntersectSphereVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec
	);

	// �~���Ɖ~���̌�������
	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// �J�v�Z���ƃJ�v�Z���̌�������
	static bool IntersectCapsuleVsCapsule(
		const DirectX::XMVECTOR& position1,	// ���S
		const DirectX::XMVECTOR& direction1,	// �����i���K���j
		const float					length1,	// ����
		const float					radius1,	// ���a
		const DirectX::XMVECTOR& position2,	// ���S
		const DirectX::XMVECTOR& direction2,	// �����i���K���j
		const float					length2,	// ����
		const float					radius2,	// ���a
		IntersectionResult* result);

	// �����Ɛ����̍ŒZ�����̓����擾����
	static float GetMinDistSq_SegmentSegment(
		const  DirectX::XMVECTOR& point1A,
		const  DirectX::XMVECTOR& point1B,
		const  DirectX::XMVECTOR& point2A,
		const  DirectX::XMVECTOR& point2B,
		DirectX::XMVECTOR* nearPoint1,
		DirectX::XMVECTOR* nearPoint2);

	// ���Ɖ~���̌������f
	static bool IntersectSphereVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	// ���C�ƃ��f���̌�������
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result);


	static bool RayIntersectsTriangle(
		const DirectX::XMFLOAT3& rayStart,
		const DirectX::XMFLOAT3& rayEnd,
		const DirectX::XMFLOAT3& a,
		const DirectX::XMFLOAT3& b,
		const DirectX::XMFLOAT3& c,
		HitResult& result);
private:
	struct CollisionMesh
	{
		struct Triangle
		{
			DirectX::XMFLOAT3	positions[3];
			DirectX::XMFLOAT3	normal;
		};
		struct Area
		{
			DirectX::BoundingBox	boundingBox;
			std::vector<int>		triangleIndices;
		};

		std::vector<Triangle>	triangles;
		std::vector<Area>		areas;
	};
private:
	static CollisionMesh collisionMesh;
};