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

	void Render(ID3D11DeviceContext* dc);

	// レイキャスト
	bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
private:
	std::unique_ptr<Model> model;
};