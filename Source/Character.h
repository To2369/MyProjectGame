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

#include "Graphics/Model.h"

// �L�����N�^�[
class Character
{
public:
	Character() {}
	virtual ~Character() {}

	//�s��X�V
	void UpdateTransform();

	void UpdateAnimation(float elapsedTime);

	// �_���[�W����
	bool ApplyDamage(float invincibleTime, int damage);

	// �Ռ���^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);
public:
	Model* GetModel() { return model.get(); }

	//�ʒu�擾
	const DirectX::XMFLOAT3& GetPosition()const { return position; }

	//�ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	//��]�擾
	const DirectX::XMFLOAT4& GetQuarternion()const { return quaternion; }

	//��]�ݒ�
	void SetQuarternion(const DirectX::XMFLOAT4& quaternion) { this->quaternion = quaternion; }

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

	int GetSpritGaugeCount() const { return spiritGaugeCount; }
	int GetSpritOneGauge() const { return spritOneGauge; }
	// �C�͂��擾
	int GetSkillEnergy() const { return skillEnergy; }

	// �ő�C�͂��擾
	int GetMaxSkillEnergy() const { return maxSkillEnergy; }

	bool IsUseSpiritEnergy()const { return useSpiritEnergyFlag; }

	const DirectX::XMVECTOR& GetRight() const { return right; }
	const DirectX::XMVECTOR& GetFront() const { return front; }

	const bool IsAttackNextFlag()const { return attackNextFlag; }
	const bool IsAttackCollisionFlag() const { return attackCollisionFlag; }

	void SetAttackNextFlag(const bool& flag) { this->attackNextFlag = flag; }
	void SetAttackCollisionFlag(const bool& flag) { this->attackCollisionFlag = flag; }

	float GetInvincibleTimer() const { return invincibleTimer; }
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
	void Fly(float speed);
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
	std::unique_ptr<Model> model;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
	DirectX::XMFLOAT4 quaternion_additional = { 0,0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMVECTOR right = { 1,0,0 };
	DirectX::XMVECTOR up = { 0,1,0 };
	DirectX::XMVECTOR front = { 0,0,1 };
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
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
	int spiritEnergy = 300;	//�C��
	int maxSpritEnergy = 500;//�ő�C��
	int skillEnergy = 200;	//�Z��
	int maxSkillEnergy = 700;	//�ő�Z��

	float invincibleTimer = 0.2f;   // ���G����
	float friction = 0.5f; // ���C
	float acceleration = 1.0f;  // �����x
	float maxMoveSpeed = 5.0f;  // �ő�ړ����x
	DirectX::XMFLOAT3 direction = { 0,0,0 };    // �ړ�����
	float airControl = 0.3f;    // �󒆐���p�W��
	float slopeRate = 1.0f; // �X�Η�

	bool useSpiritEnergyFlag = false;
	float energyRecoveryTimer = 0.0f;
	float energyNoRecoveryTimer = 1.0f;
	int spiritHealSpeed = 5;
	int spritOneGauge = 0;
	int oneGaugeMax = 100;
	int spiritGaugeCount = 1;


	int useDashSkillEnergy = 1;
	float skillEnergyTimer = 0.0f;	// �Z�͂��g���ĉ��b��������

	bool dashFlag = false;

	bool attackCollisionFlag = false;
	bool attackNextFlag = false;
};