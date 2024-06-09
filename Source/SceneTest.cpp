#include"SceneTest.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//������
void SceneTest::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneTest::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

    camera = Camera::Instance();
    float x, y;
    x = static_cast<float>(graphics->GetScreenWidth());
    y = static_cast<float>(graphics->GetScreenHeight());
    camera->SetPerspectiveFov(
        DirectX::XMConvertToRadians(30),
        x / y,
        0.1f,
        100.0f
    );
    camera->SetLookAt(
        DirectX::XMFLOAT3(0, 0, -10),		//�J�����̎��_(�ʒu)
        DirectX::XMFLOAT3(0, 0, 0),			//�J�����̒����_(�^�[�Q�b�g)
        DirectX::XMFLOAT3(0, 1, 0)			//�J�����̏����
    );
    cameraCtrl = std::make_unique<CameraController>();

    spr[0] = std::make_unique<Sprite>(graphics->GetDevice(), filename[0]);

    sprite_batches[0] = std::make_unique<Sprite_batch>(graphics->GetDevice(), filename[2], 2048);
    DirectX::XMFLOAT3 test = { 0.5f, 0.5f, 0.5f };
    geometric_primitives[0] = std::make_unique<GeometricCapsule>(graphics->GetDevice(), 0.5f,test,6,3,3);
    geometric_primitives[1] = std::make_unique<GeometricSphere>(graphics->GetDevice(), 32,32);
}

//�I����
void SceneTest::Finalize()
{

}

//�X�V����
void SceneTest::Update(float elapsedTime)
{
    cameraCtrl->Update(elapsedTime);
#ifdef USE_IMGUI

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
    ImGui::Begin("ImGUI");
    ImGui::SliderFloat3("cameraPos", &camera_position.x, -100.0f, 100.0f);

    ImGui::SliderFloat3("light_direction", &light_direction.x, -1.0f, +1.0f);

    ImGui::SliderFloat3("translation", &tramslation.x, -10.0f, 10.0f);

    ImGui::SliderFloat3("scaling.", &scaling.x, -10.0f, 10.0f);

    ImGui::SliderFloat3("rotation", &rotation.x, -10.0f, 10.0f);

    ImGui::ColorEdit4("material_color", reinterpret_cast<float*>(&material_color));
    ImGui::End();
#endif
}

//�`��
void SceneTest::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    //�r���[�|�[�g�擾
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //�`��R���e�L�X�g
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;
    rc.camera = camera;
    rc.lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };	// ���C�g�����i�������j
    // 2D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
    // 2D �`��
    {


        sprite_batches[0]->Begin(dc, sprite_batches[0]->GetReplaced_pixel_shader(), sprite_batches[0]->GetReplaced_Shader_resource_view());
        sprite_batches[0]->Render(dc, 0, 0, 1280, 720);
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

#if 0
    //3D���f���̕`��ɕK�v�ȏ��
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, DirectX::XMMatrixMultiply(
        DirectX::XMLoadFloat4x4(rc.camera->GetView()),
        DirectX::XMLoadFloat4x4(rc.camera->GetProjection())));
    scene_data.lightDirection = rc.lightDirection;
#else//�Q�[���쐬�̎�����
    float aspect_ratio{ viewport.Width / viewport.Height };//����p�v�Z
    DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };//�����s��i�����m�F�j

    DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
    DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
    DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
    DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye,focus,up) };
    Scene_constants scene_data{};
    DirectX::XMStoreFloat4x4(&scene_data.viewProjection, V * P);
    scene_data.lightDirection = light_direction;
#endif

    // 3D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLBACK));

    //�g��k���s��
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
    // ��]�s��
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
    // ���s�ړ��s��
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x - 1.0f,tramslation.y,tramslation.z) };

    // ���[���h�ϊ��s��
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, S * R * T);

    // 3D �`��
    {
        //�萔�o�b�t�@�̓o�^
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);

        /* dc->UpdateSubresource(buffer.Get(), 0, 0, &sc, 0, 0);
         dc->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
         dc->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());*/

        geometric_primitives[0]->Render(dc, world, { material_color });


        dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::WIRE_CULLBACK));
        //�g��k���s��
        DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
        // ��]�s��
        DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
        // ���s�ړ��s��
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x + 1.0f,tramslation.y,tramslation.z) };
    
        DirectX::XMStoreFloat4x4(&world, S * R * T);
        geometric_primitives[1]->Render(dc, world, { material_color });
    }

    // 3D�f�o�b�O�`��
    {

    }
}