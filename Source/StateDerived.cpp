#include "StateDerived.h"
#include "Player.h"
#include "Mathf.h"

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

WeakAttackState::~WeakAttackState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//�o�g���X�e�[�g�ɓ��������̃��\�b�h
void WeakAttackState::Enter()
{
	SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

//�o�g���X�e�[�g�Ŏ��s���郁�\�b�h
void WeakAttackState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//�o�g���X�e�[�g����łĂ����Ƃ��̃��\�b�h
void WeakAttackState::Exit()
{

}

UseSkillState::~UseSkillState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//�o�g���X�e�[�g�ɓ��������̃��\�b�h
void UseSkillState::Enter()
{
	SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

//�o�g���X�e�[�g�Ŏ��s���郁�\�b�h
void UseSkillState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//�o�g���X�e�[�g����łĂ����Ƃ��̃��\�b�h
void UseSkillState::Exit()
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
	// �e��Enter�֐��̓��e�͊eTransition����State�֐����Q�l��
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbatIdle), true);
}

// �ҋ@�X�e�[�g�Ŏ��s���郁�\�b�h
void IdleState::Execute(float elapsedTime)
{
	if (owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Move));
	}

	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
	}

	if (owner->InputJump())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Jump));
	}

	if (owner->InputArts(elapsedTime))
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::UseSkill));
	}
}

// �ҋ@�X�e�[�g����o�Ă����Ƃ��̃��\�b�h
void IdleState::Exit()
{
	
}


// �ǐՃX�e�[�g�ɓ��������̃��\�b�h
void MoveState::Enter()
{
	// �e��Enter�֐��̓��e�͊eTransition����State�֐����Q�l��
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

// �ǐՃX�e�[�g�Ŏ��s���郁�\�b�h
void MoveState::Execute(float elapsedTime)
{
	if (!owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
	}

	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
	}

	if(owner->InputJump())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Jump));
	}

	if (owner->InputArts(elapsedTime))
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::UseSkill));
	}

}

// �ǐՃX�e�[�g����o�Ă����Ƃ��̃��\�b�h
void MoveState::Exit()
{

}

void DashState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void DashState::Execute(float elapsedTime)
{
	if (!owner->InputMove(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
	}

	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
	}

	/*owner->InputDashTowardsEnemy(elapsedTime);
	if (!owner->InputDashTowardsEnemy(elapsedTime))
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Move));
	}*/
}

void DashState::Exit()
{
	//�����Ȃ��Ă悢
}

void JumpState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimJumpStart), false);
}

void JumpState::Execute(float elapsedTime)
{
	owner->InputMove(elapsedTime);

	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetModel()->PlayAnimation(AnimJumpFalling, true);
	}
	if (owner->isGrounded())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Land));
	}
}

void JumpState::Exit()
{
	
}

void LandState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimJumpLanding), false);
}

void  LandState::Execute(float elapsedTime)
{
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void  LandState::Exit()
{
	
}

void WeakAttackState01::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_1), false);
}

void  WeakAttackState01::Execute(float elapsedTime)
{
	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.4f && animationTime <= 0.45f)
	{
		owner->CollisionNodeVsEnemies("ball_l", 0.3f);
	}
	else
	{
		owner->attackCollisionFlag = false;
	}
	if (animationTime >= 0.1f && animationTime <= 0.9f)
	{
		if (owner->InputAttack())
		{
			owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack02));
		}
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState01::Exit()
{

}

void WeakAttackState02::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_2), false);
}

void  WeakAttackState02::Execute(float elapsedTime)
{
	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.25f && animationTime <= 0.3f)
	{

	}
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack03));
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState02::Exit()
{

}

void WeakAttackState03::Enter()
{

	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_3), false);
}

void  WeakAttackState03::Execute(float elapsedTime)
{

	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.38f && animationTime <= 0.45f)
	{

	}
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack04));
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState03::Exit()
{
	
}

void WeakAttackState04::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_4), false);
}

void  WeakAttackState04::Execute(float elapsedTime)
{
	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.2f && animationTime <= 0.4f)
	{

	}
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack05));
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState04::Exit()
{
	
}

void WeakAttackState05::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_5), false);
}

void  WeakAttackState05::Execute(float elapsedTime)
{
	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.27f && animationTime <= 0.31f)
	{

	}
	if (owner->InputAttack())
	{
		owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState05::Exit()
{
	//�����Ȃ��Ă悢
}

void WeakAttackState06::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_6), false);
}

void  WeakAttackState06::Execute(float elapsedTime)
{
	float animationTime = owner->GetModel()->currentAnimationSeconds;
	if (animationTime >= 0.4f && animationTime <= 0.5f)
	{

	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState06::Exit()
{
	//�����Ȃ��Ă悢
}

void DashToEnemyState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void  DashToEnemyState::Execute(float elapsedTime)
{

}

void DashToEnemyState::Exit()
{
	//�����Ȃ��Ă悢
}

void SkillSelectState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimTpose), true);
}

void  SkillSelectState::Execute(float elapsedTime)
{
	if (!owner->InputArts(elapsedTime))
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void SkillSelectState::Exit()
{
	//�����Ȃ��Ă悢
}


void DamegeState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
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
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void  DeathState::Execute(float elapsedTime)
{

}

void DeathState::Exit()
{
	//�����Ȃ��Ă悢
}