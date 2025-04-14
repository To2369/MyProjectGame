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

//サーチステートに入った時のメソッド
void MovementState::Enter()
{
	SetSubState(static_cast<int>(Player::Movement::Idle));
}

//サーチステートで実行するメソッド
void MovementState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//サーチステートからでていくときのメソッド
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

//バトルステートに入った時のメソッド
void WeakAttackState::Enter()
{
	SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

//バトルステートで実行するメソッド
void WeakAttackState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//バトルステートからでていくときのメソッド
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

//バトルステートに入った時のメソッド
void UseSkillState::Enter()
{
	SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

//バトルステートで実行するメソッド
void UseSkillState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//バトルステートからでていくときのメソッド
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


// 待機ステートに入った時のメソッド
void IdleState::Enter()
{
	// 各種Enter関数の内容は各Transition○○State関数を参考に
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbatIdle), true);
}

// 待機ステートで実行するメソッド
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

// 待機ステートから出ていくときのメソッド
void IdleState::Exit()
{
	
}


// 追跡ステートに入った時のメソッド
void MoveState::Enter()
{
	// 各種Enter関数の内容は各Transition○○State関数を参考に
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

// 追跡ステートで実行するメソッド
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

// 追跡ステートから出ていくときのメソッド
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
	//書かなくてよい
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
		owner->CollisionNodeVsEnemies("ball_l", 0.7f);
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
	else if (!owner->GetModel()->IsPlayAnimation())
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
		owner->CollisionNodeVsEnemies("ball_r", 0.7f);
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
		owner->CollisionNodeVsEnemies("hand_l", 0.7f);
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
		owner->CollisionNodeVsEnemies("calf_r", 0.7f);
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
		owner->CollisionNodeVsEnemies("calf_r", 0.7f);
	}
	if (animationTime >= 0.55f && animationTime <= 0.73f)
	{
		owner->CollisionNodeVsEnemies("hand_l", 0.7f);
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
	//書かなくてよい
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
		owner->CollisionNodeVsEnemies("ball_r", 0.7f);
	}
	if (!owner->GetModel()->IsPlayAnimation())
	{
		owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
	}
}

void WeakAttackState06::Exit()
{
	//書かなくてよい
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
	//書かなくてよい
}

void SkillSelectState::Enter()
{
	owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimUnequip), true);
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
}