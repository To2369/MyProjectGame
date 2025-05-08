#include "EnemyHuman.h"
#include "Graphics/Graphics.h"

EnemyHuman::EnemyHuman()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\pl\\Character1.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // 幅
	height = 1.0f;  // 高さ

    maxHealth = 1000;
    health = maxHealth;

    position.y = 5.0f;
}

void EnemyHuman::Update(float elapsedTime)
{
	// 速度処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間の更新
	UpdateInvincibleTimer(elapsedTime);


    UpdateStatus(elapsedTime);
	UpdateTransform();
}

void EnemyHuman::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

void EnemyHuman::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Slime", nullptr, ImGuiWindowFlags_None))
    {
        //トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("Position", &position.x);
            //スケール
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
#endif

}

// 死亡したときに呼ばれる
void EnemyHuman::OnDead()
{
	Destroy();
}