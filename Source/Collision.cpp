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

inline DirectX::XMFLOAT3 GetTriangleNormVector(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
    //�@�O�p�`tr�̖@��n�����߂�
    //���R�_�����v���Ɍ����鑤��\�Ƃ���
    DirectX::XMFLOAT3 n = {};

    //p1-p0�̃x�N�g��
    DirectX::XMVECTOR a = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
    //p2-p1�̃x�N�g��
    DirectX::XMVECTOR b = DirectX::XMVectorSubtract(
        DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p1)
    );
    //��L��2�ӂ̊O�ς̐��K�����ꂽ�l
    DirectX::XMStoreFloat3(&n, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(a, b)));

    return n;
}

bool RayVsTriangle(DirectX::XMFLOAT3* p, const Ray& r, const Triangle& t, float& neart)
{
    //�C����r���O�p�`t�̕\������������A���A��_p���O�p�`t�̓����ɂ���ꍇ�̂݁Atrue��Ԃ�
    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);



    //����(���C)�̕����x�N�g��r.d�Ɩʖ@���x�N�g��n�̓���fDot���v�Z���܂�
    float fDot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&r.d), DirectX::XMLoadFloat3(&n)));

    // ���ς��}�C�i�X���ǂ�������i�\�ʂȂ�𑱂���j
    if (fDot < 0.0f)
    {
        // �������^�����瓖�����Ă��Ȃ�������ifabs(fDot) > 1e-6f�j
        if (fabs(fDot) > 1e-6f)
        {
            // �����ƎO�p�`�̌�_�܂ł̌�����̋������v�Z�i�X�P�[���l�̌v�Z�j
            float l;
            DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(&r.p));
            l = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, DirectX::XMLoadFloat3(&n))) / fDot;

            if (l > neart)
                return false;

            // �������O���傫�����ǂ�������
            if (l > 0)
            {
                // ��_�̍��W�ʒu���v�Z
                //*p = {r.p.x+r.d.x+l,r.p.y + r.d.y + l, r.p.z + r.d.z + l };
                DirectX::XMStoreFloat3(p,
                    DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&r.p),
                        DirectX::XMVectorScale(DirectX::XMLoadFloat3(&r.d), l)));
                //���ʏ�ɂ���_p���v�Z(�n�_�{�����x�N�g�����X�P�[��)
                // ��_����O�p�`�̂��ꂼ��̓_�܂ł̃x�N�g�����v�Z
                DirectX::XMVECTOR tmp0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p0), DirectX::XMLoadFloat3(p));
                DirectX::XMVECTOR tmp1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p1), DirectX::XMLoadFloat3(p));
                DirectX::XMVECTOR tmp2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&t.p2), DirectX::XMLoadFloat3(p));

                // ��L�Ōv�Z�����x�N�g���̂��ꂼ��̊O�ς��v�Z
                DirectX::XMVECTOR n0 = DirectX::XMVector3Cross(tmp0, tmp1);
                DirectX::XMVECTOR n1 = DirectX::XMVector3Cross(tmp1, tmp2);
                DirectX::XMVECTOR n2 = DirectX::XMVector3Cross(tmp2, tmp0);

                // �v�Z�����O�ς̒l�ƎO�p�`�̓��ς����ׂē��������������Ă�����i���ς̒l���v���X�Ȃ�j
                if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(n0, DirectX::XMLoadFloat3(&n))) > 0 &&
                    DirectX::XMVectorGetX(DirectX::XMVector3Dot(n1, DirectX::XMLoadFloat3(&n))) > 0 &&
                    DirectX::XMVectorGetX(DirectX::XMVector3Dot(n2, DirectX::XMLoadFloat3(&n))) > 0)
                {
                    neart = l;
                    // �������O�p�`�Ɍ������Ă���
                    return true;
                }
            }
        }
    }
    *p = {};
    return false;
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

                // ���C�ƎO�p�`�̌�_�̍��W�i�o�͗p�j
                DirectX::XMFLOAT3 p;

                Ray r;
                DirectX::XMStoreFloat3(&r.p, localRayStartVec);
                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
                DirectX::XMStoreFloat(&r.l, localRayLengthVec);

                Triangle t;
                t.p0 = a.position;
                t.p1 = b.position;
                t.p2 = c.position;

                Sphere s;
                DirectX::XMStoreFloat3(&s.p, localRayStartVec);
                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
                DirectX::XMStoreFloat(&r.l, localRayLengthVec);
                s.r = 0;

                if (RayVsTriangle(&p, r, t, localRayLength))
                {
                    // �ŋߋ������X�V
                    //localRayLength = length;

                    // �}�e���A���ԍ����X�V
                    materialIndex = subset.materialUniqueID;

                    // ��_�Ɩ@�����X�V
                    hitPosition = DirectX::XMLoadFloat3(&p);
                    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);
                    hitNormal = DirectX::XMLoadFloat3(&n);
                }
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

//bool Collision::IntersectSphereRayVsModel(
//    const DirectX::XMFLOAT3& start,
//    const DirectX::XMFLOAT3& end,
//    const float radius,
//    const Model* model,
//    HitResult& result)
//{
//    // ���[���h��ԏ�ł̃��C�̎n�_
//    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
//    // ���[���h��ԏ�ł̃��C�̏I�_
//    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
//    // ���[���h��ԏ�ł̃��C�̎n�_����I�_�܂ł̃x�N�g��
//    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
//    // ���[���h��ԏ�ł̃��C�̒���
//    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
//    DirectX::XMStoreFloat(&result.distance, worldRayLength);
//
//    // true...�Փ˂���
//    bool hit = false;
//
//    // ���b�V�����Ƃɏ������s��
//    for (const Model::Mesh& mesh_ : model->meshes)
//    {
//        // AABB�̍ŏ��E�ő�_���擾
//        DirectX::XMFLOAT3 meshMin = mesh_.boundingBox[0];
//        DirectX::XMFLOAT3 meshMax = mesh_.boundingBox[1];
//
//        // AABB�̌�������
//        float tMin = 0.0f, tMax = 1.0f;
//        AABB meshAABB = { meshMin, meshMax };
//        if (!meshAABB.IntersectsRay(start, end, tMin, tMax))
//        {
//            continue;  // AABB�ƃ��C���������Ȃ��ꍇ�A���̃��b�V�����X�L�b�v
//        }
//
//        // AABB�ƌ�������ꍇ�ɎO�p�`�Ƃ̌���������s��
//        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.defaultGlobalTransform);
//        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);
//
//        // ���[�J����Ԃł̃��C�̎n�_�ƏI�_���v�Z
//        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
//        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
//        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
//        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
//        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
//        float localRayLength;
//        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);
//
//        // ���_�f�[�^���擾
//        const std::vector<Model::vertex>& vertices = mesh_.vertices;
//        const std::vector<UINT>& indices = mesh_.indices;
//
//        // ���ƂȂ���
//        int materialIndex = -1;
//        DirectX::XMVECTOR hitPosition;
//        DirectX::XMVECTOR hitNormal;
//
//        for (const Model::Mesh::Subset& subset : mesh_.subsets)
//        {
//            for (UINT i = 0; i < subset.indexCount; i += 3)
//            {
//                UINT index = subset.startIndexLocation + i;
//
//                // �O�p�`�̒��_�̒��o
//                const Model::vertex& a = vertices.at(indices.at(index));
//                const Model::vertex& b = vertices.at(indices.at(index + 1));
//                const Model::vertex& c = vertices.at(indices.at(index + 2));
//
//                // ���C�ƎO�p�`�̌�_�̍��W�i�o�͗p�j
//                DirectX::XMFLOAT3 p;
//
//                Ray r;
//                DirectX::XMStoreFloat3(&r.p, localRayStartVec);
//                DirectX::XMStoreFloat3(&r.d, localRayDirectVec);
//                DirectX::XMStoreFloat(&r.l, localRayLengthVec);
//
//                Triangle t;
//                t.p0 = a.position;
//                t.p1 = b.position;
//                t.p2 = c.position;
//
//                Sphere s;
//                DirectX::XMStoreFloat3(&s.p, localRayStartVec);
//
//                if (SphereCastVsTriangle(&p, r, t,s, localRayLength))
//                {
//                    // �ŋߋ������X�V
//                    //localRayLength = length;
//
//                    // �}�e���A���ԍ����X�V
//                    materialIndex = subset.materialUniqueID;
//
//                    // ��_�Ɩ@�����X�V
//                    hitPosition = DirectX::XMLoadFloat3(&p);
//                    DirectX::XMFLOAT3 n = GetTriangleNormVector(t.p0, t.p1, t.p2);
//                    hitNormal = DirectX::XMLoadFloat3(&n);
//                }
//            }
//        }
//
//        if (materialIndex >= 0)
//        {
//            // ��_���W�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
//            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
//            // ���[���h��ԏ�ł̃��C�̎n�_�����_�܂ł̃x�N�g��
//            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
//            // ���[���h��ԏ�ł̃��C�̎��_�����_�܂ł̒���
//            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
//            float distance;
//            DirectX::XMStoreFloat(&distance, worldLengthVec);
//
//            // �q�b�g���ʏ��ۑ�
//            if (result.distance > distance)
//            {
//                // �q�b�g���̖ʂ̖@�������[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
//                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);
//
//                result.distance = distance;
//                result.materialIndex = materialIndex;
//                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
//                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
//                hit = true;
//            }
//        }
//    }
//
//    return hit;
//}