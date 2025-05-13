#pragma once

#include "StateDerived.h"
#include "AttackData.h"
#include "EnemyHuman.h"
void MovementState<EnemyHuman>::Enter()
{
    SetSubState(static_cast<int>(EnemyHuman::Movement::Idle));
}

void MovementState<EnemyHuman>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void MovementState<EnemyHuman>::Exit()
{

}

void AttackState<EnemyHuman>::Enter()
{
   /* if (owner->InputWeekAttack())
    {
        SetSubState(static_cast<int>(Player::Attack::WeakAttack01_1));
    }
    else if (owner->InputStrongAttack())
    {
        SetSubState(static_cast<int>(Player::Attack::StrongAttack01));
    }*/
}

void AttackState<EnemyHuman>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void AttackState<EnemyHuman>::Exit()
{

}

void IdleState<EnemyHuman>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbatIdle), true);
}

void IdleState<EnemyHuman>::Execute(float elapsedTime)
{

}

void IdleState<EnemyHuman>::Exit()
{

}

void MoveState<EnemyHuman>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
}

void MoveState<EnemyHuman>::Execute(float elapsedTime)
{

}

void MoveState<EnemyHuman>::Exit()
{

}