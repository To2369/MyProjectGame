#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device);
	~DebugRenderer() {}

public:
	// 描画実行
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	// 球描画
	void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

	// 円柱描画
	void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

	// カプセル描画
	void DrawCapsule(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);
private:
	// 球メッシュ作成
	void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

	// 円柱メッシュ作成
	void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

	// カプセルメッシュの作成
	void CreateCapsuleMesh(ID3D11Device* device, float radius, float height, int slices, int stacks);
private:
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	wvp;
		DirectX::XMFLOAT4	color;
	};

	struct Sphere
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	center;
		float				radius;
	};

	struct Cylinder
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
		float				radius;
		float				height;
	};

	struct Capsule
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 position;
		float	radius;
		float	height;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			capsuleVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	std::vector<Sphere>		spheres;
	std::vector<Cylinder>	cylinders;
	std::vector<Capsule>	capsules;

	UINT	sphereVertexCount = 0;
	UINT	cylinderVertexCount = 0;
	UINT capsuleVertexCount = 0;
};
