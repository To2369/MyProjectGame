#pragma once
struct AttackData
{
	float hitStartTime;							// �����蔻�肪�n�܂鎞��
	float hitEndTime;							// �����蔻�肪�I��鎞��
	float inputAcceptStartTime;					// ���̍U���ɑJ�ڂ��邽�߂̓��͎�t�J�n
	float inputAcceptEndTime;					// ���̍U���ɑJ�ڂ��邽�߂̓��͎�t�I��
	float progressLimit;						// ���̍U���Ɉڂ�ۂ̃A�j���[�V�����i�s�x���~�b�g
	float hitRadius;							// �����蔻��̑傫��
	const char* hitBoneName;					// �����蔻���t����{�[���̖��O
	int damage;

	/*�A�j���[�V�����ɓ�ȏ�U�����肪����Ƃ��̏��*/
	float secondHitStartTime = 0.0f;			// ���ڂ̓����蔻�肪�n�܂鎞��
	float secondHitEndTime = 0.0f;				// ���ڂ̓����蔻�肪�I��鎞��
	const char* secondHitBoneName = nullptr;	// ���ڂ̓����蔻�������{�[���̖��O
};

// �U���Ɏg�p���Ă���f�[�^
namespace PlAttackDatas
{
	// ��U��
	static constexpr AttackData strongAttack01 =
	{
		0.4f, 0.45f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData strongAttack02 =
	{
		0.25f, 0.3f,
		0.1f, 0.8f,
		0.6f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData strongAttack03 =
	{
		0.38f, 0.45f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_l",
		1
	};
	static constexpr AttackData strongAttack04 =
	{
		0.2f,  0.4f,
		0.1f, 0.9f,
		0.4f,
		0.7f, "calf_r",
		1
	};
	static constexpr AttackData strongAttack05 =
	{
		0.27f, 0.31f,
		0.1f, 1.0f,
		0.7f,
		0.7f, "calf_r",
		1,
		0.55f, 0.73f,
		"hand_l"
	};
	static constexpr AttackData  strongAttack06 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};

	// ���U��
	static constexpr AttackData weakAttack01 =
	{
		0.4f, 0.9f,
		0.1f, 1.4f,
		0.5f,
		0.7f, "lowerarm_l",
		1
	};
	static constexpr AttackData weakAttack02 =
	{
		0.45f, 0.7f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack03 =
	{
		0.3f, 0.4f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1,
		0.5f, 0.65f,
		"calf_r"
	};
	static constexpr AttackData weakAttack04 =
	{
		0.4f, 0.55f,
		0.1f, 0.8f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack05 =
	{
		0.45f, 0.6f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData weakAttack06 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack07 =
	{
		0.25f, 0.3f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_r",
		1,
		0.5f, 0.75f,
		"ball_r"
	};
	static constexpr AttackData weakAttack08 =
	{
		0.3f, 0.5f,
		0.1f, 0.8f,
		0.5f,
		0.7f,"calf_r",
		1
	};
}