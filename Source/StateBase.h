#pragma once
#include <vector>
#include"memory"

template<typename TypeCharacter>
class State
{
public:
	// �R���X�g���N�^
	State(TypeCharacter *character) :owner(character) {}
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
	HierarchicalState(TypeCharacter* character) :State<TypeCharacter>(character) {}
	virtual ~HierarchicalState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() = 0;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedtime) = 0;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit() = 0;
	// �T�u�X�e�[�g�o�^
	virtual void SetSubState(int newState);
	// �T�u�X�e�[�g�ύX
	virtual void ChangeSubState(int newState);
	// �T�u�X�e�[�g�o�^
	virtual void RegisterSubState(State<TypeCharacter>* state);
	// �T�u�X�e�[�g�擾
	virtual State<TypeCharacter>* GetSubState() { return subState; }
	//�w��T�u�X�e�[�g�擾
	State<TypeCharacter>* GetSubSetPool(int state) { return subStatePool[state]; }
	// �T�u�X�e�[�g�ԍ��擾
	virtual int GetSubStateIndex();

protected:
	std::vector<State<TypeCharacter>*>subStatePool;
	State<TypeCharacter>*subState;
};