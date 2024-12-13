#pragma once
#include <vector>
#include"memory"

template<typename TypeCharacter>
class State
{
public:
	// コンストラクタ
	State(TypeCharacter *character) :owner(character) {}
	virtual ~State() {}
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
protected:
	TypeCharacter* owner;
};

template<typename TypeCharacter>
class HierarchicalState :public State<TypeCharacter>
{
public:
	HierarchicalState(TypeCharacter* character) :State<TypeCharacter>(character) {}
	virtual ~HierarchicalState() {}
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedtime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
	// サブステート登録
	virtual void SetSubState(int newState);
	// サブステート変更
	virtual void ChangeSubState(int newState);
	// サブステート登録
	virtual void RegisterSubState(State<TypeCharacter>* state);
	// サブステート取得
	virtual State<TypeCharacter>* GetSubState() { return subState; }
	//指定サブステート取得
	State<TypeCharacter>* GetSubSetPool(int state) { return subStatePool[state]; }
	// サブステート番号取得
	virtual int GetSubStateIndex();

protected:
	std::vector<State<TypeCharacter>*>subStatePool;
	State<TypeCharacter>*subState;
};