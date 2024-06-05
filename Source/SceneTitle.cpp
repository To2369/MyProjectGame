#include"SceneTitle.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//������
void SceneTitle::Initialize()
{
    // �V�[���萔�o�b�t�@�̍쐬
    spr[0] = std::make_unique<Sprite>(Graphics::Instance()->GetDevice(), filename[0]);

    sprite_batches[0] = std::make_unique<Sprite_batch>(Graphics::Instance()->GetDevice(), filename[2],2048);
}

//�I����
void SceneTitle::Finalize()
{

}
int a = 0;
//�X�V����
void SceneTitle::Update(float elapsedTime)
{

#ifdef USE_IMGUI
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");

    ImGui::SliderInt("a", &a, 0.0f, 1.0f);
    ImGui::End();
#endif
}

//�`��
void SceneTitle::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = Graphics::Instance()->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));


    // 3D ���f���̕`��ɕK�v�ȏ��
    //SceneConstants sc;

    // 3D �`��ݒ�
    //graphicsMgr->SettingRenderContext([&](RenderContext& rc) {
    //    // �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
    //    dc->PSSetSamplers(0, 1, rc.sampler_states[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
    //    // �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
    //    dc->OMSetBlendState(rc.blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
    //    // �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
    //    dc->OMSetDepthStencilState(rc.depth_stencil_states[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
    //    // ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
    //    dc->RSSetState(rc.rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
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
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {
       

        sprite_batches[0]->Begin(dc,sprite_batches[0]->GetReplaced_pixel_shader(),sprite_batches[0]->GetReplaced_Shader_resource_view());
        sprite_batches[0]->Render(dc,0,0,1280,720);
        sprite_batches[0]->End(dc);

        dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
        spr[0]->Textout(dc, "ECC", 0, 0, 16, 16, 1, 1, 1, 1);
    }
    // 2D�f�o�b�OGUI�`��
    {
#ifdef USE_IMGUI
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
    }
}