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

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayAndModel(
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
    for (const Model::mesh& mesh_ : model->meshes)
    {
        // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh_.default_global_transform);
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ���[�J����ԏ�ł̃��C�̎n�_
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        // ���[�J����ԏ�ł̃��C�̏I�_
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        // ���[�J����ԏ�ł̃��C�̎n�_����I�_�܂ł̃x�N�g��
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        // ���[�J����ԏ�ł̃��C�̕����x�N�g��
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        // ���[�J����ԏ�ł̃��C�̒���
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

        for (const Model::mesh::subset& subset : mesh_.subsets)
        {
            for (UINT i = 0; i < subset.index_count; i += 3)
            {
                UINT index = subset.start_index_location + i;

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
                // ��_����_ A �܂ł̃x�N�g��
                DirectX::XMVECTOR paVec = DirectX::XMVectorSubtract(aVec, hitPosVec);
                // ��_����_ A �܂ł̃x�N�g���Ɠ_A�_B�̃x�N�g���̊O��
                DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVec, abVec);
                // �O�ςƖ@���̌������`�F�b�N
                DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
                float dot1;
                DirectX::XMStoreFloat(&dot1, dotVec1);
                if (dot1 < 0.0f)
                {
                    continue;
                }

                // ��_����_ B �܂ł̃x�N�g��
                DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
                // ��_����_ B �܂ł̃x�N�g���Ɠ_B�_C�̃x�N�g���̊O��
                DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
                // �O�ςƖ@���̌������`�F�b�N
                DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
                float dot2;
                DirectX::XMStoreFloat(&dot2, dotVec2);
                if (dot2 < 0.0f)
                {
                    continue;
                }

                // ��_����_ C �܂ł̃x�N�g��
                DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
                // ��_����_ C �܂ł̃x�N�g���Ɠ_C�_A�̃x�N�g���̊O��
                DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
                // �O�ςƖ@���̌������`�F�b�N
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
                materialIndex = subset.material_unique_id;

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