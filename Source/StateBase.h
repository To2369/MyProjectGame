#pragma once
#include <vector>
#include <memory>

template<typename TypeCharacter>
class State
{
public:
	// �R���X�g���N�^
	State(TypeCharacter* character) :owner(character) {}
	virtual ~State() {}
	// �S�Čp����Ŏ���������K�v�����邽�ߏ������z�֐��Ŏ���
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() = 0;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) = 0;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit() = 0;
protected:
	TypeCharacter* owner;
};

template<typename TypeCharacter>
class HierarchicalState :public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	HierarchicalState(TypeCharacter* character) :State<TypeCharacter>(character) {}
	virtual ~HierarchicalState() {}
	// �S�Čp����Ŏ���������K�v�����邽�ߏ������z�֐��Ŏ���
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() = 0;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) = 0;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit() = 0;
	//�T�u�X�e�[�g�o�^
	virtual void SetSubState(int newState);
	//�T�u�X�e�[�g�ύX
	virtual void ChangeSubState(int newState);
	//�T�u�X�e�[�g�o�^
	virtual void RegisterSubState(State<TypeCharacter>* state);
	//�T�u�X�e�[�g�擾
	virtual State<TypeCharacter>* GetSubState() { return subState; }
	//�w��T�u�X�e�[�g�擾
	State<TypeCharacter>* GetSubSetPool(int state) { return subStatePool[state]; }
	//�T�u�X�e�[�g�ԍ��擾
	virtual int GetSubStateIndex();
protected:
	std::vector<State<TypeCharacter>*> subStatePool;
	State<TypeCharacter>* subState = nullptr;
};

// 2�w�ڃX�e�[�g�Z�b�g
template<typename TypeCharacter>
void HierarchicalState<TypeCharacter>::SetSubState(int newState)
{
	// �O���SetState�֐����Q�l�ɋL�q���Ȃ���
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
	for (State<TypeCharacter>* state : subStatePool)
	{
		if (subState == state)return i;
		++i;
	}
	return i;
}
