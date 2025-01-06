#pragma once
#include <vector>
#include "StateBase.h"

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
	// �X�e�[�g�o�^ 
	void RegisterState(HierarchicalState* state);
	// ���݂̃X�e�[�g�ԍ��擾 
	int GetStateIndex();
	// �Q�w�ڃX�e�[�g�ύX 
	void ChangeSubState(int newState);
	// �Q�w�ڃX�e�[�g�o�^ 
	void RegisterSubState(int index, State* subState);
	// �X�e�[�g�擾 
	HierarchicalState* GetState() { return currentState; }
private:
	// ���݂̃X�e�[�g
	HierarchicalState* currentState = nullptr;
	// �e�X�e�[�g��ێ�����z��
	std::vector<HierarchicalState*> statePool;
};