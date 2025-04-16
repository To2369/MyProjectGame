#pragma once
#include <vector>
#include "StateBase.h"
template<typename TypeCharacter>
class StateMachine
{
public:
	// �R���X�g���N�^ 
	StateMachine() {}
	// �f�X�g���N�^ 
	~StateMachine();
	// �X�V���� 
	void Update(float elapsedTime);
	// �X�e�[�g�Z�b�g 
	void SetState(int setstate);
	// �X�e�[�g�ύX 
	void ChangeState(int newState);
	//�w��X�e�[�g�擾
	HierarchicalState<TypeCharacter>* GetStatePool(int state) { return statePool[state]; }
	// �X�e�[�g�o�^ 
	void RegisterState(HierarchicalState<TypeCharacter>* state);
	// ���݂̃X�e�[�g�ԍ��擾 
	int GetStateIndex();
	// �Q�w�ڃX�e�[�g�ύX 
	void ChangeSubState(int newState);
	// �Q�w�ڃX�e�[�g�o�^ 
	void RegisterSubState(int index, State<TypeCharacter>* subState);
	// �X�e�[�g�擾 
	HierarchicalState<TypeCharacter>* GetState() { return currentState; }
private:
	// ���݂̃X�e�[�g
	HierarchicalState<TypeCharacter>* currentState = nullptr;
	// �e�X�e�[�g��ێ�����z��
	std::vector<HierarchicalState<TypeCharacter>*> statePool;
};

template<typename TypeCharacter>
StateMachine<TypeCharacter>::~StateMachine()
{
	for (State<TypeCharacter>* state : statePool)
	{
		delete state;
	}
	statePool.clear();
}
// �X�V����
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::Update(float elapsedTime)
{
	currentState->Execute(elapsedTime);
}
// �X�e�[�g�Z�b�g
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::SetState(int setState)
{
	currentState = statePool.at(setState);
	currentState->Enter();
}
// �X�e�[�g�ύX
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeState(int newState)
{
	currentState->GetSubState()->Exit();
	currentState->Exit();
	currentState = statePool.at(newState);
	currentState->Enter();
}
// �X�e�[�g�o�^
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::RegisterState(HierarchicalState<TypeCharacter>* state)
{
	// �e�X�e�[�g�o�^
	statePool.emplace_back(state);
}
// �X�e�[�g�ԍ��擾
template<typename TypeCharacter>
int StateMachine<TypeCharacter>::GetStateIndex()
{
	int i = 0;
	for (State<TypeCharacter>* state : statePool)
	{
		if (state == currentState)
		{
			// i�ԍ��ڂ̃X�e�[�g�����^�[��
			return i;
		}
		++i;
	}
	// �X�e�[�g��������Ȃ�������
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