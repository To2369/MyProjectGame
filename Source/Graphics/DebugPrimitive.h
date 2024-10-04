#pragma once
#include <DirectXMath.h>
#include "GeometricPrimitive.h"
#include <memory>
#include <vector>
enum class FORM_STATE
{
    Cube,       // éläpå`
    Cylinder,   // â~íå
    Sphere,     // ãÖ
    Capsule,    // ÉJÉvÉZÉã

    Enum_Max,
};

class DebugPrimitive
{
public:
    DebugPrimitive(ID3D11Device* device);
    ~DebugPrimitive(){}

    void UpdateTransform(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale);

    void Render(ID3D11DeviceContext* dc);

    void DrawCube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& color);

    void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

    void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

    void DrawCapsule(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 radius,float height, const DirectX::XMFLOAT4& color);
private:
    struct Cube
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 scale;
    };

    struct Cylinder
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        float radius;
        float height;
    };

    struct Sphere
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	center;
        float				radius;
    };

    struct Capsule
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 radius;
        float height;
    };

    DirectX::XMFLOAT4 materialColor = { 1,1,1,1 };
    //épê®êßå‰
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    std::unique_ptr<GeometricPrimitive> geometricPrimitives[static_cast<int>(FORM_STATE::Enum_Max)];
    std::vector<Cube> cubes;
    std::vector<Sphere>	spheres;
    std::vector<Cylinder> cylinders;
    std::vector<Capsule> capsules;
};