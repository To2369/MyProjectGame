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

// �I�u�W�F�N�g�̌������ʂ̍\����
struct IntersectionResult
{
	DirectX::XMVECTOR	pointA = {};			// �I�u�W�F�N�gA���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
	DirectX::XMVECTOR	pointB = {};			// �I�u�W�F�N�gB���̌�_�B�I�u�W�F�N�gA Vs �I�u�W�F�N�gB�Ŋ֐����\�������
	DirectX::XMVECTOR	normal = {};			// ��_�����ԏՓ˂̒P�ʖ@���x�N�g���B�����̓I�u�W�F�N�gB���I�u�W�F�N�gA
	float				penetration = 0.0f; 	// �@���x�N�g�������ɂ����߂荞�ݗʁB�������Ă���ꍇ�Ƀv���X�̒l���ԋp�����
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
};