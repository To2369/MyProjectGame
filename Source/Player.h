#pragma once

#include "Graphics/Model.h"
#include "Character.h"

// �v���C���[
class Player :public Character
{
public:
	Player();
	~Player()override;

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);
private:
	std::unique_ptr<Model> model;
};