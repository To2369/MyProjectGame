#include "StateBase.h"

// 2層目ステートセット
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::SetSubState(int newState)
{
	subState = subStatePool.at(newState);
	subState->Enter();
}

// 2層目のステート切り替え
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::ChangeSubState(int newState)
{
	subState->Exit();
	subState = subStatePool.at(newState);
	subState->Enter();
}

// サブステート登録
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::RegisterSubState(State<TypeCharacter>* state)
{
	subStatePool.emplace_back(state);
}

// サブステートの番号取得
template<typename TypeCharacter>
int HierarchicalState<TypeCharacter>::GetSubStateIndex()
{
	int i = 0;
	for (int i = 0; i < subStatePool.size(); ++i) {
		if (subStatePool[i] == subState) {
			return i;
		}
	}
	return i;
}