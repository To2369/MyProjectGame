#include "Stage.h"
#include "Graphics/Graphics.h"
Stage::Stage()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(),
		".//Data//Model//Stage//ExampleStage.fbx");
    //scale.x = scale.y = scale.z = 0.05f;
}

void Stage::Update(float elapsedTime)
{
    UpdateTransform();
}

void Stage::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, { transform }, { 1.0f,1.0f,1.0f,1.0f });
}

// レイキャスト
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(),transform, hit);
}

bool Stage::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float r, HitResult& hit)
{
    return Collision::IntersectSphereVsModel(start, end, r,model.get(), transform, hit);
}

void Stage::UpdateTransform()
{
    // 座標系変換用の行列
    const DirectX::XMFLOAT4X4 coordinate_system_transform[] =
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:右手系 Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:左手系 Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:右手系 Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:左手系 Z-UP
    };
    // デフォルトのスケールファクタを設定して行列に反映
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

    // スケール行列作成
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };


    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    // 位置行列作成
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };

    // 行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = C * S * R * T;

    // 計算したワールド行列をtransformに取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}