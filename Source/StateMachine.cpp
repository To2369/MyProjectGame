#include "StateMachine.h"

template<typename TypeCharacter>
StateMachine<TypeCharacter>::~StateMachine()
{
	for (State* state : this->statePool)
	{
		delete state;
	}
	this->statePool.clear();
}
// 更新処理
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::Update(float elapsedTime)
{
	this->currentState->Execute(elapsedTime);
}
// ステートセット
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::SetState(int setState)
{
	this->currentState = statePool.at(setState);
	this->currentState->Enter();
}
// ステート変更
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeState(int newState)
{
	// 現在のステートのExit関数を実行、新しいステートをセット、新しいステートのEnter関数を呼び出す。
	/*currentState->Exit();
	currentState = statePool.at(newState);
	currentState->Enter();*/
	currentState->GetSubState()->Exit();
	currentState->Exit();
	currentState = statePool[newState];
	currentState->Enter();
}
//// ステート登録
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::RegisterState(HierarchicalState<TypeCharacter>* state)
{
	// 親ステート登録
	statePool.emplace_back(state);
}

// ステート番号取得
template<typename TypeCharacter>
int StateMachine<TypeCharacter>::GetStateIndex()
{
	int i = 0;
	for (auto* state : statePool)
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
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeSubState(int newState)
{
	currentState->ChangeSubState(newState);
}
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::RegisterSubState(int index, State<TypeCharacter>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}
