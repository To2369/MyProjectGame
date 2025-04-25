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

    if (owner->InputWeekAttack())
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

void WeakAtkState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_1), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState01<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack01);


    //�@���̍U���Ɉڂ邽�߂̓��͎�t����
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01.inputAcceptStartTime, PlAttackDatas::weakAttack01.inputAcceptEndTime);
  

    // ���݂̃A�j���[�V�������w��̊��������񂾂�
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack01.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack02));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState01<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void WeakAtkState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_2), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState02<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack02);


    //�@���̍U���Ɉڂ邽�߂̓��͎�t����
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01.inputAcceptStartTime, PlAttackDatas::weakAttack01.inputAcceptEndTime);

    // ���݂̃A�j���[�V�������w��̊��������񂾂�
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack02.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack03));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState02<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void WeakAtkState03<Player>::Enter()
{

    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_3), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState03<Player>::Execute(float elapsedTime)
{

    owner->ActiveAttackCollider(PlAttackDatas::weakAttack03);


    //�@���̍U���Ɉڂ邽�߂̓��͎�t����
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01.inputAcceptStartTime, PlAttackDatas::weakAttack01.inputAcceptEndTime);

    // ���݂̃A�j���[�V�������w��̊��������񂾂�
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack03.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack04));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState03<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void WeakAtkState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_4), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState04<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack04);


    //�@���̍U���Ɉڂ邽�߂̓��͎�t����
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01.inputAcceptStartTime, PlAttackDatas::weakAttack01.inputAcceptEndTime);

    // ���݂̃A�j���[�V�������w��̊��������񂾂�
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack04.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack05));
        }
    }
    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState04<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void WeakAtkState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_5), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState05<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack05);

    //�@���̍U���Ɉڂ邽�߂̓��͎�t����
    owner->InputAttackNext(owner->GetModel()->currentAnimationSeconds, PlAttackDatas::weakAttack01.inputAcceptStartTime, PlAttackDatas::weakAttack01.inputAcceptEndTime);

    // ���݂̃A�j���[�V�������w��̊��������񂾂�
    if (owner->GetModel()->GetAnimationProgress() >= PlAttackDatas::weakAttack05.progressLimit)
    {
        if (owner->IsWeekAtkNextFlag())
        {
            owner->GetStateMachine()->ChangeSubState(static_cast<int>(Player::WeakAttack::WeakAttack06));
        }
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState05<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void WeakAtkState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimConbo01_6), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  WeakAtkState06<Player>::Execute(float elapsedTime)
{
    owner->ActiveAttackCollider(PlAttackDatas::weakAttack06);

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void WeakAtkState06<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}


void StrongAtkState01<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack01), false);
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void  StrongAtkState01<Player>::Execute(float elapsedTime)
{
    if (owner->GetModel()->currentAnimationSeconds >= PlAttackDatas::weakAttack06.hitStartTime
        && owner->GetModel()->currentAnimationSeconds <= PlAttackDatas::weakAttack06.hitEndTime)
    {
        owner->CollisionNodeVsEnemies(
            PlAttackDatas::weakAttack06.hitBoneName,
            PlAttackDatas::weakAttack06.hitRadius,
            owner->GetInvincibleTimer(),
            PlAttackDatas::weakAttack06.damage);
    }
    else
    {
        //owner->SetAttackCollisionFlag(false);
    }

    if (!owner->GetModel()->IsPlayAnimation())
    {
        owner->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Movement));
    }
}

void  StrongAtkState01<Player>::Exit()
{
    owner->SetWeekAtkNextFlag(false); // ���͏�����
}

void StrongAtkState02<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack02), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAtkState02<Player>::Execute(float elapsedTime)
{


}

void  StrongAtkState02<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState03<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack03), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState03<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState03<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState04<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack04), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState04<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState04<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState05<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack05), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState05<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState05<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState06<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack06), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState06<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState06<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState07<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack07), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState07<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState07<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
}

void StrongAttackState08<Player>::Enter()
{
    owner->GetModel()->PlayAnimation(static_cast<int>(AnimationNum::AnimAttack08), false);
    //owner->SetAttackNextFlag(false);
}

void  StrongAttackState08<Player>::Execute(float elapsedTime)
{


}

void  StrongAttackState08<Player>::Exit()
{
    //owner->SetAttackNextFlag(false);
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
    //�����Ȃ��Ă悢
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
    //�����Ȃ��Ă悢
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
    //�����Ȃ��Ă悢
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
    //�����Ȃ��Ă悢
}