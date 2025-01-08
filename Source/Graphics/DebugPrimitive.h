#pragma once
#include <DirectXMath.h>
#include "GeometricPrimitive.h"
#include <memory>
#include <vector>
enum class FORM_STATE
{
    Cube,       // �l�p�`
    Cylinder,   // �~��
    Sphere,     // ��
    Capsule,    // �J�v�Z��

    Enum_Max,
};

class DebugPrimitive
{
public:
    DebugPrimitive(ID3D11Device* device);
    ~DebugPrimitive(){}

    void UpdateTransform(const DirectX::XMFLOAT3& position,const DirectX::XMFLOAT3& angle, const DirectX::XMFLOAT3& scale);

    void Render(ID3D11DeviceContext* dc);

    void DrawCube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& color);

    void DrawCylinder(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle, float radius, float height, const DirectX::XMFLOAT4& color);

    void DrawSphere(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& angle, float radius, const DirectX::XMFLOAT4& color);

    void DrawCapsule(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& angle, float radius,float height, const DirectX::XMFLOAT4& color);
private:
    struct Cube
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 angle;
        DirectX::XMFLOAT3 scale;
    };

    struct Cylinder
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 angle;
        float radius;
        float height;
    };

    struct Sphere
    {
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT3	center;
        DirectX::XMFLOAT3   angle;
        float				radius;
    };

    struct Capsule
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 angle;
        float radius;
        float height;
    };

    DirectX::XMFLOAT4 materialColor = { 1,1,1,1 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    //�p������
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