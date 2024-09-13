#pragma once
#include <DirectXMath.h>
// �L�����N�^�[
class Character
{
public:
	Character(){}
	virtual ~Character() {}

	//�s��X�V
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };

	//�p������
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};