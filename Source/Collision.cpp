#include "Collision.h"
#include <algorithm>
using namespace DirectX;
// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
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
bool Collision::IntersectCylinderVsCylinder(
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

bool Collision::IntersectCapsuleVsCapsule(
    const DirectX::XMVECTOR& position1,	// ���S
    const DirectX::XMVECTOR& direction1,	// �����i���K���j
    const float					length1,	// ����
    const float					radius1,	// ���a
    const DirectX::XMVECTOR& position2,	// ���S
    const DirectX::XMVECTOR& direction2,	// �����i���K���j
    const float					length2,	// ����
    const float					radius2,	// ���a
    IntersectionResult* result)
{


    // �e�J�v�Z���̒��S����̍ŋߓ_�Z�o
    //�����̕����ɂ����L�тĂȂ����炱��͂��߂�������
    DirectX::XMVECTOR point1 = {}, point2 = {};
    DirectX::XMVECTOR end1 = DirectX::XMVectorAdd(position1, DirectX::XMVectorScale(direction1, length1));
    DirectX::XMVECTOR end2 = DirectX::XMVectorAdd(position2, DirectX::XMVectorScale(direction2, length2));

    // �������m�̍ŒZ�����̓����擾
    float distSq = GetMinDistSq_SegmentSegment(position1, end1, position2, end2, &point1, &point2);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);
    float radiusAdd = radius1 + radius2;

    // �ŋߐړ_�Ԃ̋������J�v�Z���̔��a�̘a��菬�����ꍇ�A�Փ˂��Ă���
    if (distSq < radiusAdd * radiusAdd)
    {
        if (result)
        {
            result->normal = DirectX::XMVector3Normalize(vec);
            result->penetration = radiusAdd - sqrtf(distSq);
            result->pointA = DirectX::XMVectorAdd(point1, DirectX::XMVectorScale(result->normal, -radius1));
            result->pointB = DirectX::XMVectorAdd(point2, DirectX::XMVectorScale(result->normal, radius2));
        }

        return true;
    }

    return false;
}

// �����Ɛ����̍ŒZ�����̓����擾����
float Collision::GetMinDistSq_SegmentSegment(
    const  DirectX::XMVECTOR& point1A,
    const  DirectX::XMVECTOR& point1B,
    const  DirectX::XMVECTOR& point2A,
    const  DirectX::XMVECTOR& point2B,
    DirectX::XMVECTOR* nearPoint1,
    DirectX::XMVECTOR* nearPoint2)
{
    DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
    DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

    float t1 = 0.0f, t2 = 0.0f;

    if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// ���������_�ɏk�ނ��Ă���ꍇ
    {
        t1 = t2 = 0.0f;
    }
    else if (a <= FLT_EPSILON)					// �Е��id0�j���_�ɏk�ނ��Ă���ꍇ
    {
        t1 = 0.0f;
        t2 = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
        if (e <= FLT_EPSILON)					// �Е��id1�j���_�ɏk�ނ��Ă���ꍇ
        {
            t2 = 0.0f;
            t1 = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else									// ����������
        {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
            float denom = a * e - b * b;

            if (denom != 0.0f)					// ���s�m�F�i���s���� t1 = 0.0f�i�����̎n�[�j�����̏����l�Ƃ��Čv�Z�������߂�j
            {
                t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t2 = b * t1 + f;

            if (t2 < 0.0f)						// t1���n�[���O���ɂ���ꍇ
            {
                t2 = 0.0f;
                t1 = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t2 > e)					// t1���I�[���O���ɂ���ꍇ
            {
                t2 = 1.0f;
                t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
            else								// t1��������ɂ���ꍇ
            {
                t2 /= e;
            }
        }
    }

    // �e������̍ŋߓ_�Z�o
    DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
    DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

    if (nearPoint1)
    {
        *nearPoint1 = point1;
    }
    if (nearPoint2)
    {
        *nearPoint2 = point2;
    }

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

// ���Ɖ~���̌������f
bool Collision::IntersectSphereVsCylinder(
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

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    // ���[���h��ԏ�ł̃��C�̎n�_
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    // ���[���h��ԏ�ł̃��C�̏I�_
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    // ���[���h��ԏ�ł̃��C�̎n�_����I�_�܂ł̃x�N�g��
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    // ���[���h��ԏ�ł̃��C�̒���
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    // true...�Փ˂���
    bool hit = false;

    // ���b�V�����Ƃɏ������s��
    for (const Model::Mesh& mesh_ : model->meshes)
    {
        // AABB�̍ŏ��E�ő�_���擾
        DirectX::XMFLOAT3 meshMin = mesh_.boundingBox[0];
        DirectX::XMFLOAT3 meshMax = mesh_.boundingBox[1];

        // AABB�̌�������
        float tMin = 0.0f, tMax = 1.0f;
        AABB meshAABB = { meshMin, meshMax };
        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
        {
            continue;  // AABB�ƃ��C���������Ȃ��ꍇ�A���̃��b�V�����X�L�b�v
        }

        // AABB�ƌ�������ꍇ�ɎO�p�`�Ƃ̌���������s��
        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ���[�J����Ԃł̃��C�̎n�_�ƏI�_���v�Z
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
        float localRayLength;
        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);

        // ���_�f�[�^���擾
        const std::vector<Model::vertex>& vertices = mesh_.vertices;
        const std::vector<UINT>& indices = mesh_.indices;

        // ���ƂȂ���
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const Model::Mesh::Subset& subset : mesh_.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndexLocation + i;

                // �O�p�`�̒��_�̒��o
                const Model::vertex& a = vertices.at(indices.at(index));
                const Model::vertex& b = vertices.at(indices.at(index + 1));
                const Model::vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
                DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
                DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

                // �O�p�`�̖@�����v�Z
                DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

                // ���C�̕����Ɩʂ̕������`�F�b�N�i���ʂ̕\������j
                DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
                float dot;
                DirectX::XMStoreFloat(&dot, dotVec);
                if (dot >= 0)
                {
                    continue;
                }

                // ���C�̎n�_�����_�܂ł̒������v�Z
                DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
                float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

                // ������ 0 �ȉ��Ȃ�ȍ~�̏����͔�΂�
                if (length < 0.0f)
                {
                    continue;
                }

                // ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
                if (length > localRayLength)
                {
                    continue;
                }

                // ���C�ƕ��ʂ̌�_���v�Z
                DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

                // ��_���O�p�`�̓����ɂ��邩�ǂ�������
                DirectX::XMVECTOR paVec = DirectX::XMVectorSubtract(aVec, hitPosVec);
                DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVec, abVec);
                DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
                float dot1;
                DirectX::XMStoreFloat(&dot1, dotVec1);
                if (dot1 < 0.0f)
                {
                    continue;
                }

                DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
                DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
                DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
                float dot2;
                DirectX::XMStoreFloat(&dot2, dotVec2);
                if (dot2 < 0.0f)
                {
                    continue;
                }

                DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
                DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
                DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
                float dot3;
                DirectX::XMStoreFloat(&dot3, dotVec3);
                if (dot3 < 0.0f)
                {
                    continue;
                }

                // �ŋߋ������X�V
                localRayLength = length;

                // �}�e���A���ԍ����X�V
                materialIndex = subset.materialUniqueID;

                // ��_�Ɩ@�����X�V
                hitPosition = hitPosVec;
                hitNormal = normalVec;
            }
        }

        if (materialIndex >= 0)
        {
            // ��_���W�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
            // ���[���h��ԏ�ł̃��C�̎n�_�����_�܂ł̃x�N�g��
            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
            // ���[���h��ԏ�ł̃��C�̎��_�����_�܂ł̒���
            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
            float distance;
            DirectX::XMStoreFloat(&distance, worldLengthVec);

            // �q�b�g���ʏ��ۑ�
            if (result.distance > distance)
            {
                // �q�b�g���̖ʂ̖@�������[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
                hit = true;
            }
        }
    }

    return hit;
}
