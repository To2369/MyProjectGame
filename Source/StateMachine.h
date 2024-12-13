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