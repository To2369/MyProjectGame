#pragma once

#include "Graphics/Model.h"
#include "Collision.h"
// ステージ
class Stage
{
public:
	Stage();
	~Stage() {};

	void Update(float elapsedTime);

	void UpdateTransform();

	void Render(ID3D11DeviceContext* dc);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float r, HitResult& hit);
private:
	std::unique_ptr<Model> model;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
										  1,0,0,0,
										  0,1,0,0,
										  0,0,1,0,
										  0,0,0,1 };
};