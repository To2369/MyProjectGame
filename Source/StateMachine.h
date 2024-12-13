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