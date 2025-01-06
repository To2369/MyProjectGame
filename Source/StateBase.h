#pragma once
#include <vector>
#include"memory"

class Player;

class State
{
public:
	// コンストラクタ
	State(Player* enemy) :owner(enemy) {}
	virtual ~State() {}
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
protected:
	Player* owner;
};

class HierarchicalState :public State
{
public:
	// コンストラクタ
	HierarchicalState(Player* enemy) :State(enemy) {}
	virtual ~HierarchicalState() {}
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter() = 0;
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime) = 0;
	// ステートから出ていくときのメソッド
	virtual void Exit() = 0;
	//サブステート登録
	virtual void SetSubState(int newState);
	//サブステート変更
	virtual void ChangeSubState(int newState);
	//サブステート登録
	virtual void RegisterSubState(State* state);
	//サブステート取得
	virtual State* GetSubState() { return subState; }
	//サブステート番号取得
	virtual int GetSubStateIndex();
protected:
	std::vector<State*> subStatePool;
	State* subState = nullptr;
};