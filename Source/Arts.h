#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
// �e�Ǘ��̑O���錾
class ArtsManager;

// �e
class  Arts
{
public:
    Arts(ArtsManager* manager);
    virtual ~Arts() {}

    virtual void Update(float elapsedTIme) = 0;

    virtual void Render(ID3D11DeviceContext* dc) = 0;

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();

    void Destroy();

public:
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // �����擾
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }

    // �����̐ݒ�
    void SetDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }

    // �g��k���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // ���a�擾
    float GetRadius() const { return radius; }

    int GetUseSpiritEnergy() const { return useSpiritEnergy; }
    int GetUseSkillEnergy() const { return useSkillEnergy; }
    int GetDamage() const { return damage; }
protected:
    // �s��X�V����
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3	position = { 0,0,0 };	// �ʒu
    DirectX::XMFLOAT3	direction = { 0,0,1 };	// ����
    DirectX::XMFLOAT3	scale = { 1,1,1 };		// �g��k��
    // �p���s��
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    ArtsManager* artsMgr = nullptr;

    float radius = 0.5f;	// ���a

    int useSpiritEnergy = 0;
    int useSkillEnergy = 0;
    int damage = 0;
};