#pragma once
#include <vector>
#include "StateBase.h"

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
	// ステート登録 
	void RegisterState(HierarchicalState* state);
	// 現在のステート番号取得 
	int GetStateIndex();
	// ２層目ステート変更 
	void ChangeSubState(int newState);
	// ２層目ステート登録 
	void RegisterSubState(int index, State* subState);
	// ステート取得 
	HierarchicalState* GetState() { return currentState; }
private:
	// 現在のステート
	HierarchicalState* currentState = nullptr;
	// 各ステートを保持する配列
	std::vector<HierarchicalState*> statePool;
};