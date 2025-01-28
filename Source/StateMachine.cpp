#include "StateMachine.h"

StateMachine::~StateMachine()
{
	for (State* state : statePool)
	{
		delete state;
	}
	statePool.clear();
}
// 更新処理
void StateMachine::Update(float elapsedTime)
{
	currentState->Execute(elapsedTime);
}
// ステートセット
void StateMachine::SetState(int setState)
{
	currentState = statePool.at(setState);
	currentState->Enter();
}
// ステート変更
void StateMachine::ChangeState(int newState)
{
	currentState->GetSubState()->Exit();
	currentState->Exit();
	currentState = statePool.at(newState);
	currentState->Enter();


}
// ステート登録
void StateMachine::RegisterState(HierarchicalState* state)
{
	// 親ステート登録
	statePool.emplace_back(state);
}
// ステート番号取得
int StateMachine::GetStateIndex()
{
	int i = 0;
	for (State* state : statePool)
	{
		if (state == currentState)
		{
			// i番号目のステートをリターン
			return i;
		}
		++i;
	}
	// ステートが見つからなかった時
	return -1;
}

void StateMachine::ChangeSubState(int newState)
{
	currentState->ChangeSubState(newState);
}

void StateMachine::RegisterSubState(int index, State* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}