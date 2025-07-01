#include "Stage.h"
#include "Graphics/Graphics.h"
Stage::Stage()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(),
		".//Data//Model//Stage//Colosseum.fbx");
    scale.x = scale.y = scale.z = 0.05f;
}

void Stage::Update(float elapsedTime)
{
    UpdateTransform();
}

void Stage::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, { transform }, { 1.0f,1.0f,1.0f,1.0f });
}

// ���C�L���X�g
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(),transform, hit);
}

void Stage::UpdateTransform()
{
    // ���W�n�ϊ��p�̍s��
    const DirectX::XMFLOAT4X4 coordinate_system_transform[] =
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:�E��n Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:����n Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:�E��n Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:����n Z-UP
    };
    // �f�t�H���g�̃X�P�[���t�@�N�^��ݒ肵�čs��ɔ��f
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

    // �X�P�[���s��쐬
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };


    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    // �ʒu�s��쐬
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };

    // �s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = C * S * R * T;

    // �v�Z�������[���h�s���transform�Ɏ��o��
    DirectX::XMStoreFloat4x4(&transform, W);
}