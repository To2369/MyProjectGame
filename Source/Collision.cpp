#include "Collision.h"

// ���Ƌ��̌�������
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
    // �Q�_�Ԃ̈ʒu�x�N�g������
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);
    // �P��̒������v�Z
    DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
    // float �^�ɕϊ�
    float len;
    DirectX::XMStoreFloat(&len, length);

    // ��������
    float range = radiusA + radiusB;
    if (len > range)
    {
        return false;
    }

    // �Փ˂��Ă�����Փ˕����̃x�N�g����ݒ�
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&outVec, vec);

    return true;
}

// �~���Ɖ~���̌�������
bool Collision::IntersectCylinderAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A �̑����Ƃ� B �̓�����Ȃ�Փ˂��Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
        return false;

    // A �̓��� B �̑����Ƃ�艺�Ȃ�Փ˂��Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ ���ʂł͈̔̓`�F�b�N
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // �Փ˕����̃x�N�g��
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

// ���Ɖ~���̌������f
bool Collision::IntersectSphereAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A�i���j�̑����Ƃ� B�i�~���j�̓�����Ȃ�Փ˂��Ă��Ȃ�
    if (positionA.y - radiusA > positionB.y + heightB)
        return false;

    // A�i���j�̓��� B�i�~���j�̑����Ƃ�艺�Ȃ�Փ˂��Ă��Ȃ�
    if (positionA.y + radiusA < positionB.y)
        return false;

    // XZ ���ʂł͈̔̓`�F�b�N
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // �Փ˕����̃x�N�g��
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}