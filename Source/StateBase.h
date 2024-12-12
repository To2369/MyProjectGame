#pragma once
#include <vector>
#include"memory"

template<typename TypeCharacter>
class State
{
public:
	// コンストラクタ
	
	State(TypeCharacter *enemy) :owner(enemy) {}
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
	HierarchicalState(TypeCharacter* enemy) :State<TypeCharacter>(enemy) {}
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




// TODO 03_02 HierarchicalStateクラスの関数定義を行う
// 2層目ステートセット
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::SetSubState(int newState)
{
	// TODO 03_02 前回のSetState関数を参考に記述しなさい
	subState =subStatePool.at(newState);
	subState->Enter();
}

// 2層目のステート切り替え
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::ChangeSubState(int newState)
{
	// TODO 03_02 前回のChangeState関数を参考に記述しなさい
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


