#include "StateDerived.h"
#include "Player.h"
#include "Mathf.h"
// TODO 02_03 Stateを基底クラスとして各種Stateクラスを用意する。
// Wanderはサンプルとしてすでに記述済み
// 各種Enter関数の内容は各Transition○○State関数を参考に
// 各種Execute関数の内容は各Update○○State関数を参考に

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

BattleState::~BattleState()
{
	for (State* state : subStatePool)
	{
		delete state;
	}
	subStatePool.clear();
}

//バトルステートに入った時のメソッド
void BattleState::Enter()
{
	SetSubState(static_cast<int>(Player::Battle::Attack));
}

//バトルステートで実行するメソッド
void BattleState::Execute(float elapsedTime)
{
	subState->Execute(elapsedTime);
}

//バトルステートからでていくときのメソッド
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


// 待機ステートに入った時のメソッド
void IdleState::Enter()
{
	// TODO 02_03
	// 各種Enter関数の内容は各Transition○○State関数を参考に
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
}

// 待機ステートで実行するメソッド
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

// 待機ステートから出ていくときのメソッド
void IdleState::Exit()
{
	//書かなくてよい
}


// 追跡ステートに入った時のメソッド
void MoveState::Enter()
{
	// TODO 02_03
	// 各種Enter関数の内容は各Transition○○State関数を参考に
	owner->GetModel()->PlayAnimation(static_cast<int>(Animation::AnimIdle), true);
}

// 追跡ステートで実行するメソッド
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

// 追跡ステートから出ていくときのメソッド
void MoveState::Exit()
{
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
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
	//書かなくてよい
}