#include "NormalBallet.h"
#include "Graphics/Graphics.h"
NormalBallet::NormalBallet(ArtsManager* manager) : Arts(manager)
{
    position = { 1,5,1 };
    damage = 1;
    useSkillEnergy = 2;
    radius = 0.5f;
    //angle.x = DirectX::XMConvertToRadians(90);
}

NormalBallet::~NormalBallet()
{
}

void NormalBallet::Update(float elapsedTime)
{
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        Destroy();
    }

    float speed = this->speed * elapsedTime;

    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(targetVec, posVec);

    DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
    if (lengthSq > 0.00001f)
    {
        vec = DirectX::XMVector3Normalize(vec);
        DirectX::XMVECTOR directVec = DirectX::XMLoadFloat3(&direction);
        DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(directVec, vec);

        float dot;
        DirectX::XMStoreFloat(&dot, dotVec);

        float rot = 1.0f - dot;
        if (rot > turnSpeed)
        {
            rot = turnSpeed;
        }

        if (fabsf(rot) > 0.0001f)
        {
            DirectX::XMVECTOR axisVec = DirectX::XMVector3Cross(directVec, vec);

            DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationAxis(axisVec, rot);

            DirectX::XMMATRIX transformMat = DirectX::XMLoadFloat4x4(&transform);
            transformMat = DirectX::XMMatrixMultiply(transformMat, rotationMat);

            directVec = DirectX::XMVector3Normalize(transformMat.r[2]);
            DirectX::XMStoreFloat3(&direction, directVec);

        }
    }
    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void NormalBallet::Render(ID3D11DeviceContext* dc)
{
    //衝突判定用のデバッグ球を描画
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawSphere(position, angle,radius, { 0,1,0,1 });
}

//発射
void NormalBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}