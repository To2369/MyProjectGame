#include "StateMachine.h"

template<typename TypeCharacter>
StateMachine<TypeCharacter>::~StateMachine()
{
	for (State* state : this->statePool)
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
	this->currentState = statePool.at(setState);
	this->currentState->Enter();
}
// �X�e�[�g�ύX
template<typename TypeCharacter>
void StateMachine<TypeCharacter>::ChangeState(int newState)
{
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
void StateMachine<TypeCharacter>::RegisterSubState(int index, State<TypeCharacter>* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}
