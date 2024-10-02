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

	// �_���[�W����
	bool ApplyDamage(float invincibleTime, int damage);

	// �Ռ���^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);
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

	// �����擾
	float GetHeight() const { return height; }

	// ���C�t���擾
	int GetHealth() const { return health; }

	// �ő僉�C�t���擾
	int GetMaxHealth() const { return maxHealth; }

	// �C�͂��擾
	int GetSpritEnergy() const { return spiritEnergy; }

	// �ő�C�͂��擾
	int GetMaxSpritEnergy() const { return maxSpritEnergy; }

	// �C�͂��擾
	int GetSkillEnergy() const { return skillEnergy; }

	// �ő�C�͂��擾
	int GetMaxSkillEnergy() const { return maxSkillEnergy; }

	bool IsUseSpiritEnergy()const { return useSpiritEnergyFlag; }
private:
	// �������x�X�V����
	void UpdateVerticalVelocity(float elapsedTime);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������x�X�V����
	void UpdateHorizontalVelocity(float elapsedTime);

	// �����ړ��X�V����
	void UpdateHorizontalMove(float elapsedTime);
protected:
	// �ړ�����������
	void Move(float vx, float vz, float speed);
	// ���񏈗�
	void Turn(float elapsedTime, float vx, float vz, float speed);
	// �W�����v����
	void Jump(float speed);
	// ���x�����X�V
	void UpdateVelocity(float elpasedTime);

	// ���G���Ԃ̍X�V
	void UpdateInvincibleTimer(float elapsedTime);

	//�X�e�[�^�X�X�V
	void UpdateStatus(float elapsedTime);

	// ���n�����Ƃ��ɌĂяo�����
	virtual void OnLanding() {}

	// �_���[�W���󂯂��Ƃ��ɌĂ΂��
	virtual void OnDamaged() {}

	// ���S�����Ƃ��ɌĂ΂��
	virtual void OnDead() {}
protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	float radius = 0.5f;
	float gravity = -1.0f;	// �d�́i�t���[���P�ʂ̒l�j
	DirectX::XMFLOAT3 velocity = { 0,0,0 };	// ���x
	//�p������
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	bool groundedFlag = false;	// true...���n����
	float height = 2.0f;	// ����
	int health = 5; // ���C�t
	int maxHealth = 5;	// �ő僉�C�t
	int spiritEnergy = 500;	//�C��
	int maxSpritEnergy = 500;//�ő�C��
	int skillEnergy = 200;	//�Z��
	int maxSkillEnergy = 600;	//�ő�Z��

	float invincibleTimer = 0.5f;   // ���G����
	float friction = 0.5f; // ���C
	float acceleration = 1.0f;  // �����x
	float maxMoveSpeed = 5.0f;  // �ő�ړ����x
	DirectX::XMFLOAT3 direction = { 0,0,0 };    // �ړ�����
	float airControl = 0.3f;    // �󒆐���p�W��
	float slopeRate = 1.0f; // �X�Η�

	bool useSpiritEnergyFlag = false;
	float energyRecoveryTimer = 0.0f;
	float energyNoRecoveryTimer = 1.0f;

	int useDashSkillEnergy = 1;
	float skillEnergyTimer = 0.0f;	// �Z�͂��g���ĉ��b��������

	bool dashFlag = false;
};