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

void AttackState<Player>::Enter()
{
    if (owner->InputWeekAttack())
    {
        SetSubState(static_cast<int>(Player::Attack::WeakAttack01_1));
    }
    else if (owner->InputStrongAttack())
    {
        SetSubState(static_cast<int>(Player::Attack::StrongAttack01));
    }
}

void AttackState<Player>::Execute(float elapsedTime)
{
    subState->Execute(elapsedTime);
}

void AttackState<Player>::Exit()
{

}

void UseSkillState<Player>::Enter()
{
    SetSubState(static_cast<int>(Player::Attack::WeakAttack01_1));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbatIdle), true);
}

void IdleState<Player>::Execute(float elapsedTime)
{
    if (owner->InputMove(elapsedTime))
    {
        owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Move));
    }

    if (owner->InputWeekAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Attack));
    }
    else if (owner->InputStrongAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Attack));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
}

void MoveState<Player>::Execute(float elapsedTime)
{
    if (!owner->InputMove(elapsedTime))
    {
        owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Idle));
    }

    if (owner->InputWeekAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Attack));
    }
    else if (owner->InputStrongAttack())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Attack));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimJumpStart), false);
}

void JumpState<Player>::Execute(float elapsedTime)
{
    owner->InputMove(elapsedTime);

    if (owner->InputJump())
    {
        owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Movement::Jump));
    }


    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetModel()->PlayAnimation(Player::AnimJumpFalling, true);
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimJumpLanding), false);
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

void WeakAtkState01_1<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack01), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_1<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_1);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_1);


    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_1.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_2));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_1<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState01_2<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack02), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_2<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_2);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_2);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_2.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_3));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_2<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState01_3<Player>::Enter()
{

    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack03), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_3<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_3);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_3);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_3.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_4));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_3<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState01_4<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack04), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_4<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_4);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_4);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_4.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_5));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_4<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState01_5<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack05), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_5<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_5);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_5);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_5.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_6));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_5<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void WeakAtkState01_6<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack06), false);
    owner->SetWeekAtkNextFlag(false); // 入力初期化
}

void  WeakAtkState01_6<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_6);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_6);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_6.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_7));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01_6<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState01_7<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack07), false);
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState01_7<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_7);

    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_7);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_7.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_8));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack02));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState01_7<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState01_8<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimAttack08), false);
    owner->SetWeekAtkNextFlag(false);
    owner->SetAwayFlag(true);
}

void WeakAtkState01_8<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_8);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState01_8<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
    owner->SetAwayFlag(false);
}


void WeakAtkState02_1<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo02_1), false);
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState02_1<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_4);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_4);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_4.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_5));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState02_1<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState02_2<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo02_2), false);
    owner->SetWeekAtkNextFlag(false);
}

void WeakAtkState02_2<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01_4);


    //　次の攻撃に移るための入力受付時間
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01_4);

    // 現在のアニメーションが指定の割合すすんだら
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01_4.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_5));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  WeakAtkState02_2<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false);
}

void StrongAtkState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_1), false);
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
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_5));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack02));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_2), false);
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
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_6));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack03));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_3), false);
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
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_6));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack04));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_4), false);
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
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_6));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack05));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_5), false);
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
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::WeakAttack01_6));
        }
        else if (owner->IsStrongAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::Attack::StrongAttack06));
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo01_6), false);
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

void StrongAttackState07<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo02_1), false);
    owner->SetStrongAtkNextFlag(false);
}

void  StrongAttackState07<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack06);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState07<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
}

void StrongAttackState08<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimConbo02_2), false);
    owner->SetStrongAtkNextFlag(false);
    owner->SetAwayFlag(true);
}

void  StrongAttackState08<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::strongAttack06);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAttackState08<Player>::Exit()
{
    owner->SetStrongAtkNextFlag(false);
    owner->SetAwayFlag(false);
}
void DashToEnemyState<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimUnequip), true);
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
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
    owner->GetModel()->PlayAnimation(static_cast<int>(Player::AnimationNum::AnimRun), true);
}

void  DeathState<Player>::Execute(float elapsedTime)
{

}

void DeathState<Player>::Exit()
{
    //書かなくてよい
}