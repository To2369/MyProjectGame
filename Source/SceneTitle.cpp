#include"SceneTitle.h"
#include"Graphics/GraphicsManager.h"
//������
void SceneTitle::Initialize()
{
    spr[0] = std::make_unique<Sprite>(DeviceManager::Instance()->GetDevice(), filename[0]);
    spr[1]= std::make_unique<Sprite>(DeviceManager::Instance()->GetDevice(), filename[1]);
}

//�I����
void SceneTitle::Finalize()
{

}

void SceneTitle::Update(float elapsedTime)
{

#ifdef USE_IMGUI
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");

    //ImGui::SliderFloat("a", &a, 0, 1280);
    ImGui::End();
#endif
}

void SceneTitle::Render()
{
    DeviceManager* deviceMgr = DeviceManager::Instance();
    GraphicsManager* graphicsMgr = GraphicsManager::Instance();

    ID3D11DeviceContext* dc = deviceMgr->GetDeviceContext();
    ID3D11RenderTargetView* rtv = deviceMgr->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = deviceMgr->GetDepthStencilView();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 3D ���f���̕`��ɕK�v�ȏ��
    //SceneConstants sc;

    // 3D �`��ݒ�
    //graphicsMgr->SettingRenderContext([&](RenderContext& rc) {
    //    // �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
    //    //dc->PSSetSamplers(0, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
    //    // �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
    //    //dc->OMSetBlendState(rc.blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
    //    // �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
    //    //dc->OMSetDepthStencilState(rc.depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
    //    // ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
    //    //dc->RSSetState(rc.rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
    //    });
    
    // 3D �`��
    {
        // �萔�o�b�t�@�̓o�^
        //dc->UpdateSubresource(buffer.Get(), 0, 0, &sc, 0, 0);
        //dc->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
        //dc->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());

        ////�X�e�[�W�̕`��
        //StageManager::Instance()->render(dc);

        ////�v���C���[�̕`��
        //PlayerManager::Instance()->render(dc);

        ////�G�`��
        //EnemyManager::instance()->render(dc);
    }

    // 3D�f�o�b�O�`��
    {

    }
    // 2D �`��ݒ�
    graphicsMgr->SettingRenderContext([&](RenderContext& rc)
        {
            //�T���v���[�X�e�[�g�̐ݒ�i�|�C���g�j
            dc->PSSetSamplers(0, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::POINT)].GetAddressOf());
            //�T���v���[�X�e�[�g�̐ݒ�i���j�A�j
            //dc->PSSetSamplers(1, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::POINT)].GetAddressOf());
            //�T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
            //dc->PSSetSamplers(2, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::POINT)].GetAddressOf());
            //�u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
            dc->OMSetBlendState(rc.blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
            //�[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I�t�A�[�x�������݃I�t�j
            dc->OMSetDepthStencilState(rc.depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
            //���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
            dc->RSSetState(rc.rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
        });
    // 2D �`��
    {
        spr[0]->Render(deviceMgr->GetDeviceContext(), 100, 100, 500, 500, 1, 1, 1, 1, 45);
        spr[1]->Render(deviceMgr->GetDeviceContext(), 700, 200, 200, 200, 1, 1, 1, 1, 45, 140, 0, 140, 240);
    }
    // 2D�f�o�b�OGUI�`��
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }
}