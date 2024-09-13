#include "Player.h"
#include "Graphics/Graphics.h"
#include "Input/GamePad.h"
#include "Input/InputManager.h"
Player::Player()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Jammo//Jammo.fbx");

	const float scale_factor = 0.1f;
	scale = { scale_factor,scale_factor,scale_factor };
}

Player::~Player()
{

}

void Player::Update(float elapsedTime)
{
	//入力情報を取得
	GamePad* gamepad = InputManager::Instance()->getGamePad();
	float ax = gamepad->getAxisLX();
	float ay = gamepad->getAxisLY();

	float moveSpeed = 1.0f;
	position.x += ax * moveSpeed;
	position.z += ay * moveSpeed;

	// ワールド行列更新
	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}