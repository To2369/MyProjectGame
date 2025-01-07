#include "DebugPrimitive.h"
#include "Graphics.h"
DebugPrimitive::DebugPrimitive(ID3D11Device*device)
{
    geometricPrimitives[static_cast<int>(FORM_STATE::Cube)] =
        std::make_unique<GeometricCube>(device);

    geometricPrimitives[static_cast<int>(FORM_STATE::Cylinder)] =
        std::make_unique<GeometricCylinder>(device, 1.0f,1.0f,16);

    geometricPrimitives[static_cast<int>(FORM_STATE::Sphere)] =
        std::make_unique<GeometricSphere>(device,1.0f,32,32);

    geometricPrimitives[static_cast<int>(FORM_STATE::Capsule)] =
        std::make_unique<GeometricCapsule>(device, 0.0f, DirectX::XMFLOAT3{ 1.0f,1.0f,1.0f }, 6, 3, 3, DirectX::XMFLOAT3{ 0,0,0 });
    
}

void DebugPrimitive::UpdateTransform(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
{
    // スケール行列作成
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) };
    // 回転行列作成
    //DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    //DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    //DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    //DirectX::XMMATRIX R = Y * X * Z;
    // 位置行列作成
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    // 行列を組み合わせ、ワールド行列を作成
    //DirectX::XMMATRIX W = S * R * T;
    DirectX::XMMATRIX W = S * T;
    //計算したワールド行列をtransformに取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

void DebugPrimitive::Render(ID3D11DeviceContext* immediate_context)
{
    Graphics* graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics->GetDeviceContext();
    RenderState* renderState = graphics->GetRenderState();
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::WIRE_CULLNONE));
    //正方形
    for (const Cube& cube : cubes)
    {
        UpdateTransform(cube.position, cube.scale);
        materialColor = cube.color;
        geometricPrimitives[static_cast<int>(FORM_STATE::Cube)]->Render(immediate_context, transform, materialColor);
    }
    cubes.clear();

    // 円柱
    for (const Cylinder& cylinder : cylinders)
    {
        UpdateTransform(cylinder.position,{cylinder.radius,cylinder.height,cylinder.radius});
        materialColor = cylinder.color;
        geometricPrimitives[static_cast<int>(FORM_STATE::Cylinder)]->Render(immediate_context, transform, materialColor);
    }
    cylinders.clear();

    //球
    for (const Sphere& sphere : spheres)
    {
        UpdateTransform(sphere.center, { sphere.radius,sphere.radius,sphere.radius });
        materialColor = sphere.color;
        geometricPrimitives[static_cast<int>(FORM_STATE::Sphere)]->Render(immediate_context, transform, materialColor);
    }
    spheres.clear();

    // カプセル
    for (const Capsule& capsule : capsules)
    {
        UpdateTransform(capsule.position, { capsule.radius,capsule.height,capsule.radius });
        materialColor = capsule.color;
        geometricPrimitives[static_cast<int>(FORM_STATE::Capsule)]->Render(immediate_context, transform, materialColor);
    }
    capsules.clear();
}


void DebugPrimitive::DrawCube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& color)
{
    Cube cube;
    cube.position = position;
    cube.scale = scale;
    cube.color = color;
    cubes.emplace_back(cube);
}

void DebugPrimitive::DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
    Cylinder cylinder;
    cylinder.position = position;
    cylinder.radius = radius;
    cylinder.height = height;
    cylinder.color = color;
    cylinders.emplace_back(cylinder);
}

void DebugPrimitive::DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color)
{
    Sphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    sphere.color = color;
    spheres.emplace_back(sphere);
}

void DebugPrimitive::DrawCapsule(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color)
{
    Capsule capsule;
    capsule.position = position;
    capsule.radius = radius;
    capsule.height = height;
    capsule.color = color;
    capsules.emplace_back(capsule);
}

