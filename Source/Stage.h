#pragma once

#include "Graphics/Model.h"

// ステージ
class Stage
{
public:
	Stage();
	~Stage() {};

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);
private:
	std::unique_ptr<Model> model;
};