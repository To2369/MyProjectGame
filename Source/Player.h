#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "BulletManager.h"
#include "Effect/Effect.h"

// �v���C���[
class Player :public Character
{
public:
	Player();
	~Player()override;

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�\��
	void DrawDebugPrimitive();
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
private:
	DirectX::XMFLOAT3 GetMoveVec() const;

	//����ړ�
	void InputMove(float elapsedTime);

	// �W�����v���͏���
	void InputJump();

	// ���͂ɂ��e���ˏ���
	void InputLaunchBullet();

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void CollisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	void CollisionBulletsAndEnemies();
protected:
	// ���n�����Ƃ��ɌĂяo�����
	void OnLanding() override;
private:
	std::unique_ptr<Model> model;
	// �ړ��X�s�[�h
	float moveSpeed = 5.0f;
	// ����X�s�[�h
	float turnSpeed = DirectX::XMConvertToRadians(720);

	// �W�����v��
	float jumpSpeed = 20.0f;
	// �d��
	float gravity = -1.0f;
	// ���x
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	int jumpCount = 0;	// �W�����v��
	int jumpLimit = 2;	// �W�����v�����i�ő�W�����v���A�ЂƂ܂��Q�i�W�����v�j

	//std::unique_ptr<BulletManager> bulletMgr;
	BulletManager bulletMgr;

	// �q�b�g�G�t�F�N�g
	std::unique_ptr<Effect> hitEffect;
};