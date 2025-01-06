#include "StateDerived.h"
#include "Player.h"
#include "Mathf.h"
// TODO 02_03 State�����N���X�Ƃ��Ċe��State�N���X��p�ӂ���B
// Wander�̓T���v���Ƃ��Ă��łɋL�q�ς�
// �e��Enter�֐��̓��e�͊eTransition����State�֐����Q�l��
// �e��Execute�֐��̓��e�͊eUpdate����State�֐����Q�l��

MovementState::~MovementState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//�T�[�`�X�e�[�g�ɓ��������̃��\�b�h
void MovementState::Enter()
{
	SetSubState(static_cast<int>(Player::Movement::Idle));
}

//�T�[�`�X�e�[�g�Ŏ��s���郁�\�b�h
void MovementState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//�T�[�`�X�e�[�g����łĂ����Ƃ��̃��\�b�h
void MovementState::Exit()
{

}

BattleState::~BattleState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//�o�g���X�e�[�g�ɓ��������̃��\�b�h
void BattleState::Enter()
{
	SetSubState(static_cast<int>(Player::Battle::Attack));
}

//�o�g���X�e�[�g�Ŏ��s���郁�\�b�h
void BattleState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//�o�g���X�e�[�g����łĂ����Ƃ��̃��\�b�h
void BattleState::Exit()
{

}

HitDamegeState::~HitDamegeState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

void HitDamegeState::Enter()
{
	SetSubState(static_cast<int>(Player::HitDamege::Damege));
}

void HitDamegeState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

void HitDamegeState::Exit()
{

}


// �ҋ@�X�e�[�g�ɓ��������̃��\�b�h
void IdleState::Enter()
{
	// TODO 02_03
	// �e��Enter�֐��̓��e�͊eTransition����State�֐����Q�l��
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
}

// �ҋ@�X�e�[�g�Ŏ��s���郁�\�b�h
void IdleState::Execute(float elapsedTime)
{
	if (owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Move));
	}

	owner->InputAttack();
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Battle));
	}

	if (owner->InputRecoverySkillEnergy(elapsedTime))
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Battle));
	}

	/*pl->InputJump();
	if (pl->InputJump())
	{
		pl->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Jump));
	}*/

}

// �ҋ@�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void IdleState::Exit()
{
	//�����Ȃ��Ă悢
}


// �ǐՃX�e�[�g�ɓ��������̃��\�b�h
void MoveState::Enter()
{
	// TODO 02_03
	// �e��Enter�֐��̓��e�͊eTransition����State�֐����Q�l��
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
}

// �ǐՃX�e�[�g�Ŏ��s���郁�\�b�h
void MoveState::Execute(float elapsedTime)
{
	if (!owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
	}

	owner->InputAttack();
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Battle));
	}

	if (owner->InputRecoverySkillEnergy(elapsedTime))
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Battle));
	}

	owner->InputJump();
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Jump));
	}

}

// �ǐՃX�e�[�g����o�Ă����Ƃ��̃��\�b�h
void MoveState::Exit()
{
	//�����Ȃ��Ă悢
}

void DashState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void DashState::Execute(float elapsedTime)
{
	if (!owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
	}

	owner->InputAttack();
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Battle));
	}

	owner->InputDashTowardsEnemy(elapsedTime);
	if (!owner->InputDashTowardsEnemy(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Move));
	}
}

void DashState::Exit()
{
	//�����Ȃ��Ă悢
}

void JumpState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void JumpState::Execute(float elapsedTime)
{
	
}

void JumpState::Exit()
{
	//�����Ȃ��Ă悢
}

void LandState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  LandState::Execute(float elapsedTime)
{

}

void  LandState::Exit()
{
	//�����Ȃ��Ă悢
}

void AttackState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  AttackState::Execute(float elapsedTime)
{

}

void AttackState::Exit()
{
	//�����Ȃ��Ă悢
}

void RecoverySkillEnergyState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  RecoverySkillEnergyState::Execute(float elapsedTime)
{

}

void RecoverySkillEnergyState::Exit()
{
	//�����Ȃ��Ă悢
}

void DashToEnemyState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  DashToEnemyState::Execute(float elapsedTime)
{

}

void DashToEnemyState::Exit()
{
	//�����Ȃ��Ă悢
}

void DamegeState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  DamegeState::Execute(float elapsedTime)
{

}

void DamegeState::Exit()
{
	//�����Ȃ��Ă悢
}

void DeathState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimRun), true);
}

void  DeathState::Execute(float elapsedTime)
{

}

void DeathState::Exit()
{
	//�����Ȃ��Ă悢
}