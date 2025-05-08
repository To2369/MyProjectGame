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

    if (owner->InputWeekAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
    }
    else if (owner->InputStrongAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::StrongAttack));
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

    if (owner->InputWeekAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
    }
    else if (owner->InputStrongAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::StrongAttack));
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

 /*   if (owner->InputWeekAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::WeakAttack));
    }
    else if (owner->InputStrongAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::StrongAttack));
    }*/

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

void WeakAtkState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack01), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01);


    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack02));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack02), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState02<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack02);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack02);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack02.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack03));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState02<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState03<Player>::Enter()
{

    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack03), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState03<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack03);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack03);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack03.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack04));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState03<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack04), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState04<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack04);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack04);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack04.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack05));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState04<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack05), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState05<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack05);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack05);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack05.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState05<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack06), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState06<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack06);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack06);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack06.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack07));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState06<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState07<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack07), false);
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState07<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack07);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack07);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack07.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack08));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState07<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState08<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack08), false);
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState08<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack08);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState08<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}


void StrongAtkState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_1), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAtkState01<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack01);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::strongAttack01);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::strongAttack01.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAtkState01<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAtkState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_2), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAtkState02<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack02);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::strongAttack02);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::strongAttack02.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack03));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }

}

void  StrongAtkState02<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAttackState03<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_3), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAttackState03<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack03);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::strongAttack03);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::strongAttack03.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack04));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState03<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAttackState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_4), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAttackState04<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack04);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::strongAttack04);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::strongAttack04.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack05));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState04<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAttackState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_5), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAttackState05<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack05);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::strongAttack05);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::strongAttack05.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::StrongAttack::StrongAttack06));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState05<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAttackState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_6), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAttackState06<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack06);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState06<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
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