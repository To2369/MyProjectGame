#include "StateBase.h"

// 2層目ステートセット
void HierarchicalState::SetSubState(int newState)
{
	// TODO 03_02 前回のSetState関数を参考に記述しなさい
	subState = subStatePool.at(newState);
	subState->Enter();
}

// 2層目のステート切り替え
void HierarchicalState::ChangeSubState(int newState)
{
	subState->Exit();
	subState = subStatePool.at(newState);
	subState->Enter();
}
// サブステート登録
void HierarchicalState::RegisterSubState(State* state)
{
	subStatePool.emplace_back(state);
}

// サブステートの番号取得
int HierarchicalState::GetSubStateIndex()
{
	int i = 0;
	for (State* state : subStatePool)
	{
		if (subState == state)return i;
		++i;
	}
	return i;
}
