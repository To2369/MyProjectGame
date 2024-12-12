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
	HierarchicalState<TypeCharacter>* GetStatePool(int state) { return statePool[state];}
    // �X�e�[�g�o�^
    void RegisterState(HierarchicalState<TypeCharacter>* state);
    // ���݂̃X�e�[�g�ԍ��擾
    int GetStateIndex();
    // TODO 03_04 �V�K�ő�Q�K�w�̃X�e�[�g�֘A��o�^
    // �Q�w�ڃX�e�[�g�ύX
    void ChangeSubState(int newState);
    // �Q�w�ڃX�e�[�g�o�^
    void RegisterSubState(int index,State<TypeCharacter>* subState);
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
	for (auto* state : this->statePool)
	{
		delete state;
	}
	this->statePool.clear();
}
// �X�V����
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::Update(float elapsedTime)
{
	this->currentState->Execute(elapsedTime);
}
// �X�e�[�g�Z�b�g
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::SetState(int setState)
{
	this->currentState =statePool.at(setState);
	this->currentState->Enter();
}
// �X�e�[�g�ύX
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeState(int newState)
{
	// TODO 02_02 �X�e�[�g�̐؂�ւ��B
	// ���݂̃X�e�[�g��Exit�֐������s�A�V�����X�e�[�g���Z�b�g�A�V�����X�e�[�g��Enter�֐����Ăяo���B
	/*currentState->Exit();
	currentState = statePool.at(newState);
	currentState->Enter();*/
	currentState->GetSubState()->Exit();
	currentState->Exit();
	currentState = statePool[newState];
	currentState->Enter();
}
//// �X�e�[�g�o�^
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::RegisterState(HierarchicalState<TypeCharacter>*state)
{
	// �e�X�e�[�g�o�^
	statePool.emplace_back(state);

}

// �X�e�[�g�ԍ��擾
template<typename TypeCharacter>
int StateMachine<TypeCharacter>::GetStateIndex()
{
	int i = 0;
	for (auto* state : statePool)
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
void StateMachine<TypeCharacter>::RegisterSubState(int index,State<TypeCharacter>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}
