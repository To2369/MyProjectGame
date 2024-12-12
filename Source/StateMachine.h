#pragma once
#include <vector>
#include "StateBase.h"
template<typename TypeCharacter>
class StateMachine
{
public:
    // コンストラクタ
    StateMachine() {}
    // デストラクタ
    ~StateMachine();
    // 更新処理
    void Update(float elapsedTime);
    // ステートセット
    void SetState(int setstate);
    // ステート変更
    void ChangeState(int newState);
	//指定ステート取得
	HierarchicalState<TypeCharacter>* GetStatePool(int state) { return statePool[state];}
    // ステート登録
    void RegisterState(HierarchicalState<TypeCharacter>* state);
    // 現在のステート番号取得
    int GetStateIndex();
    // TODO 03_04 新規で第２階層のステート関連を登録
    // ２層目ステート変更
    void ChangeSubState(int newState);
    // ２層目ステート登録
    void RegisterSubState(int index,State<TypeCharacter>* subState);
    // ステート取得
    HierarchicalState<TypeCharacter>* GetState() { return currentState; }
	
private:
   
       
    // 現在のステート
    HierarchicalState<TypeCharacter>* currentState = nullptr;
    // 各ステートを保持する配列
    std::vector<HierarchicalState<TypeCharacter>*> statePool;
};





template<typename TypeCharacter>
StateMachine<TypeCharacter>::~StateMachine()
{
	for (auto* state : this->statePool)
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
	this->currentState =statePool.at(setState);
	this->currentState->Enter();
}
// ステート変更
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeState(int newState)
{
	// TODO 02_02 ステートの切り替え。
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
void StateMachine<TypeCharacter>::RegisterState(HierarchicalState<TypeCharacter>*state)
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
void StateMachine<TypeCharacter>::RegisterSubState(int index,State<TypeCharacter>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}
