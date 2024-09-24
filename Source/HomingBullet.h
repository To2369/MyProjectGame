#pragma once

#include "Graphics/Model.h"
#include "Bullet.h"

// �ǔ��e
class HomingBullet : public Bullet
{
public:
    HomingBullet(BulletManager* manager);
    ~HomingBullet() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    // �ǔ��̃^�[�Q�b�g�̐ݒ�
    void LockonTarget(DirectX::XMFLOAT3 target)
    {
        this->target = target;

        // �^�[�Q�b�g���ύX������Ɏp�����ύX�����̂ł��̃^�C�~���O�Ŏp���s��̍X�V���K�v
        UpdateTransform();
    }

    //����
    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& target);

private:
    std::unique_ptr<Model> model;
    DirectX::XMFLOAT3 target = { 0,0,0 };	// �ǔ��^�[�Q�b�g
    float speed = 10.0f; // �ړ����x
    float turnSpeed = DirectX::XMConvertToRadians(180); // �ő��]���x
    float lifeTimer = 3.0f; // ����(�R�b)
};