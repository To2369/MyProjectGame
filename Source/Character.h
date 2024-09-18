#pragma once
#include <DirectXMath.h>
#ifdef USE_IMGUI

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern ImWchar glyphRangesJapanese[];
#endif
// �L�����N�^�[
class Character
{
public:
	Character(){}
	virtual ~Character() {}

	//�s��X�V
	void UpdateTransform();

public:
	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition()const { return position; }

	//�ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	//��]�擾
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }

	//��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//�X�P�[���ݒ�
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	//�X�P�[���擾
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// ���a�̎擾
	float GetRadius() const { return radius; }

	// ���n�������ǂ����̃t���O�̎擾
	bool isGrounded() const { return groundedFlag; }
protected:
	//�ړ�����
	void Move(float elapsedTime, float vx, float vz, float speed);
	// ���񏈗�
	void Turn(float elapsedTime, float vx, float vz, float speed);
	// �W�����v����
	void Jump(float speed);
	// ���x�����X�V
	void UpdateVelocity(float elpasedTime);

	// ���n�����Ƃ��ɌĂяo�����
	virtual void onLanding() {}
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	float radius = 0.5f;
	float gravity = -1.0f;	// �d�́i�t���[���P�ʂ̒l�j
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// ���x
	bool groundedFlag = false;	// true...���n����
	//�p������
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};