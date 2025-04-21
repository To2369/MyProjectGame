#include "StateDerived.h"
#include "AttackData.h"
#include "Player.h"

void MovementState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::Movement::Idle));
}

void MovementState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void MovementState<Player>::Exit()
{

}

void WeakAttackState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

void WeakAttackState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void WeakAttackState<Player>::Exit()
{

}

void StrongAttackState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::StrongAttack::StrongAttack01));
}

void StrongAttackState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void StrongAttackState<Player>::Exit()
{

}

void UseSkillState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::WeakAttack::WeakAttack01));
}

void UseSkillState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void UseSkillState<Player>::Exit()
{

}

void HitDamegeState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::HitDamege::Damege));
}

void HitDamegeState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void HitDamegeState<Player>::Exit()
{

}


void IdleState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbatIdle), true);
}

void IdleState<Player>::Execute(float elapsedTime)
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

void IdleState<Player>::Exit()
{

}

void MoveState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void MoveState<Player>::Execute(float elapsedTime)
{
    if (!owner->InputMove(elapsedTime))
    {
        owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
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

void MoveState<Player>::Exit()
{

}

void DashState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void DashState<Player>::Execute(float elapsedTime)
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

void DashState<Player>::Exit()
{

}

void JumpState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimJumpStart), false);
}

void JumpState<Player>::Execute(float elapsedTime)
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

void JumpState<Player>::Exit()
{

}

void LandState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimJumpLanding), false);
}

void  LandState<Player>::Execute(float elapsedTime)
{
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  LandState<Player>::Exit()
{

}

void WeakAttackState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_1), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  WeakAttackState01<Player>::Execute(float elapsedTime)
{

    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack01.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack01.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack01.hitBoneName,
            AttackDatas::weakAttack01.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack01.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }


    //　次の攻撃に移るための入力受付時間
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack01.inputAcceptStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack01.inputAcceptEndTime)
    {
        if (owner->InputAttack())
        {
            owner->SetAttackNextFlag(true);
        }
    }

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= AttackDatas::weakAttack01.progressLimit)
    {
        if (owner->IsAttackNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState01<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void WeakAttackState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_2), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  WeakAttackState02<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack02.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack02.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack02.hitBoneName,
            AttackDatas::weakAttack02.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack02.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }


    //　次の攻撃に移るための入力受付時間
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack02.inputAcceptStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack02.inputAcceptEndTime)
    {
        if (owner->InputAttack())
        {
            owner->SetAttackNextFlag(true);
        }
    }

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= AttackDatas::weakAttack02.progressLimit)
    {
        if (owner->IsAttackNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack03));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState02<Player>::Exit()
{
    owner->SetAttackNextFlag(false); // 入力初期化
}

void WeakAttackState03<Player>::Enter()
{

    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_3), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  WeakAttackState03<Player>::Execute(float elapsedTime)
{

    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack03.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack03.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack03.hitBoneName,
            AttackDatas::weakAttack03.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack03.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }


    //　次の攻撃に移るための入力受付時間
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack03.inputAcceptStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack03.inputAcceptEndTime)
    {
        if (owner->InputAttack())
        {
            owner->SetAttackNextFlag(true);
        }
    }

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= AttackDatas::weakAttack03.progressLimit)
    {
        if (owner->IsAttackNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack04));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState03<Player>::Exit()
{
    owner->SetAttackNextFlag(false); // 入力初期化
}

void WeakAttackState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_4), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  WeakAttackState04<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack04.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack04.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack04.hitBoneName,
            AttackDatas::weakAttack04.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack04.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }


    //　次の攻撃に移るための入力受付時間
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack04.inputAcceptStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack04.inputAcceptEndTime)
    {
        if (owner->InputAttack())
        {
            owner->SetAttackNextFlag(true);
        }
    }

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= AttackDatas::weakAttack04.progressLimit)
    {
        if (owner->IsAttackNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState04<Player>::Exit()
{
    owner->SetAttackNextFlag(false); // 入力初期化
}

void WeakAttackState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_5), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  WeakAttackState05<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack05.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack05.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack05.hitBoneName,
            AttackDatas::weakAttack05.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack05.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }

    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack05.secondHitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack05.secondHitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack05.hitBoneName,
            AttackDatas::weakAttack05.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack05.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }

    //　次の攻撃に移るための入力受付時間
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack05.inputAcceptStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack05.inputAcceptEndTime)
    {
        if (owner->InputAttack())
        {
            owner->SetAttackNextFlag(true);
        }
    }

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= AttackDatas::weakAttack05.progressLimit)
    {
        if (owner->IsAttackNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState05<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void WeakAttackState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_6), false);
    owner->SetAttackNextFlag(false);
}

void  WeakAttackState06<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack06.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack06.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack06.hitBoneName,
            AttackDatas::weakAttack06.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack06.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAttackState06<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}


void StrongAttackState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack01), false);
    owner->SetAttackNextFlag(false); // 入力初期化
}

void  StrongAttackState01<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= AttackDatas::weakAttack06.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= AttackDatas::weakAttack06.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            AttackDatas::weakAttack06.hitBoneName,
            AttackDatas::weakAttack06.hitRadius,
            owner->GetInvincibleTimer(),
            AttackDatas::weakAttack06.damage);
    }
    else
    {
        owner->SetAttackCollisionFlag(false);
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState01<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack02), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState02<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState02<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState03<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack03), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState03<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState03<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack04), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState04<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState04<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack05), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState05<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState05<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack06), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState06<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState06<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState07<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack07), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState07<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState07<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void StrongAttackState08<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack08), false);
    owner->SetAttackNextFlag(false);
}

void  StrongAttackState08<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState08<Player>::Exit()
{
    owner->SetAttackNextFlag(false);
}

void DashToEnemyState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void  DashToEnemyState<Player>::Execute(float elapsedTime)
{

}

void DashToEnemyState<Player>::Exit()
{
    //書かなくてよい
}

void SkillSelectState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimUnequip), true);
}

void  SkillSelectState<Player>::Execute(float elapsedTime)
{
    if (!owner->InputArts(elapsedTime))
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void SkillSelectState<Player>::Exit()
{
    //書かなくてよい
}

void DamegeState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void  DamegeState<Player>::Execute(float elapsedTime)
{

}

void DamegeState<Player>::Exit()
{
    //書かなくてよい
}

void DeathState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimRun), true);
}

void  DeathState<Player>::Execute(float elapsedTime)
{

}

void DeathState<Player>::Exit()
{
    //書かなくてよい
}