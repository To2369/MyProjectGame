#include "StateBase.h"

// 2�w�ڃX�e�[�g�Z�b�g
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::SetSubState(int newState)
{
	subState = subStatePool.at(newState);
	subState->Enter();
}

// 2�w�ڂ̃X�e�[�g�؂�ւ�
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::ChangeSubState(int newState)
{
	subState->Exit();
	subState = subStatePool.at(newState);
	subState->Enter();
}

// �T�u�X�e�[�g�o�^
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::RegisterSubState(State<TypeCharacter>* state)
{
	subStatePool.emplace_back(state);
}

// �T�u�X�e�[�g�̔ԍ��擾
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