#include "StateMachine.h"

StateMachine::~StateMachine()
{
	for (State* state : statePool)
	{
		delete state;
	}
	statePool.clear();
}
// �X�V����
void StateMachine::Update(float elapsedTime)
{
	currentState->Execute(elapsedTime);
}
// �X�e�[�g�Z�b�g
void StateMachine::SetState(int setState)
{
	currentState = statePool.at(setState);
	currentState->Enter();
}
// �X�e�[�g�ύX
void StateMachine::ChangeState(int newState)
{
	currentState->GetSubState()->Exit();
	currentState->Exit();
	currentState = statePool.at(newState);
	currentState->Enter();


}
// �X�e�[�g�o�^
void StateMachine::RegisterState(HierarchicalState* state)
{
	// �e�X�e�[�g�o�^
	statePool.emplace_back(state);
}
// �X�e�[�g�ԍ��擾
int StateMachine::GetStateIndex()
{
	int i = 0;
	for (State* state : statePool)
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

void StateMachine::ChangeSubState(int newState)
{
	currentState->ChangeSubState(newState);
}

void StateMachine::RegisterSubState(int index, State* subState)
{
	statePool.at(index)->RegisterSubState(subState);
}