#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
// �e�Ǘ��̑O���錾
class ArtsManager;

enum BalletType
{
    Normal,
    SkillStraight,
    SpiritExplosion,
};
// �e
class  Arts
{
public:
    Arts(ArtsManager* manager);
    virtual ~Arts() {}

    virtual void Update(float elapsedTIme) = 0;

    virtual void Render(ID3D11DeviceContext* dc) = 0;

    // �f�o�b�O�v���~�e�B�u�`��
    //virtual void DrawDebugPrimitive() {};

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

    void SetGrowSpeed(const float speed) { this->growSpeed = speed; }
    // ���a�擾
    float GetRadius() const { return radius; }
    float GetHeight() const { return height; }
    float GetLifeTimer() const { return lifeTimer; }
    int GetUseSpiritEnergy() const { return useSpiritEnergy; }
    int GetUseSkillEnergy() const { return useSkillEnergy; }
    int GetDamage() const { return damage; }
    virtual BalletType GetType() = 0;
    float GetCurrentCapsuleHeight()const { return currentCapsuleHeight; }
protected:
    // �s��X�V����
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3	position = { 0,0,0 };	// �ʒu
    DirectX::XMFLOAT3	direction = { 0,1,0 };	// ����
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3	scale = { 1,1,1 };		// �g��k��
    // �p���s��
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    ArtsManager* artsMgr = nullptr;

    float radius = 1.0f;	// ���a

    float height = 1.0f;
    float growSpeed = 15.0f;
    int useSpiritEnergy = 0;
    int useSkillEnergy = 0;
    int damage = 0;
    float currentCapsuleHeight = 0;
    float lifeTimer = 3.0f; // �����i�R�b�j
};