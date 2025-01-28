#include "Stage.h"
#include "Graphics/Graphics.h"
Stage::Stage()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(),
		".//Data//Model//Stage//ExampleStage.fbx");
}

void Stage::Update(float elapsedTime)
{

}

void Stage::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, { 1.0f,1.0f,1.0f,1.0f });
}

// ���C�L���X�g
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}