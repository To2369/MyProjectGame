#include "EnemySlime.h"
#include "Graphics/Graphics.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Slime//Slime.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // ��
	height = 1.0f;  // ����
}

void EnemySlime::Update(float elapsedTime)
{
	// ���x�����X�V
	UpdateVelocity(elapsedTime);

	// ���G���Ԃ̍X�V
	UpdateInvincibleTimer(elapsedTime);

	UpdateTransform();
}

void EnemySlime::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

void EnemySlime::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Slime", nullptr, ImGuiWindowFlags_None))
    {
        //�g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //�ʒu
            ImGui::InputFloat3("Position", &position.x);
            //��]
            DirectX::XMFLOAT3 pos;
            pos.x = DirectX::XMConvertToDegrees(angle.x);
            pos.y = DirectX::XMConvertToDegrees(angle.y);
            pos.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &pos.x);
            angle.x = DirectX::XMConvertToRadians(pos.x);
            angle.y = DirectX::XMConvertToRadians(pos.y);
            angle.z = DirectX::XMConvertToRadians(pos.z);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
#endif

}

// ���S�����Ƃ��ɌĂ΂��
void EnemySlime::OnDead()
{
	Destroy();
}