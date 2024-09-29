#include"SceneTest.h"
#include"Graphics/RenderContext.h"
#include"Graphics/RenderState.h"
//������
void SceneTest::Initialize()
{
    Graphics* graphics = Graphics::Instance();

    //�萔�o�b�t�@�̍쐬
    CreateBuffer<SceneTest::Scene_constants>(graphics->GetDevice(), buffer.GetAddressOf());

    camera = &Camera::Instance();
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

    sprite_batches[0] = std::make_unique<SpriteBatch>(graphics->GetDevice(), L".\\Data\\resources\\screenshot.jpg", 1);
    DirectX::XMFLOAT3 test = { 0.5f, 0.5f, 0.5f };
    geometric_primitives[0] = std::make_unique<GeometricCube>(graphics->GetDevice());
    geometric_primitives[1] = std::make_unique<GeometricCube>(graphics->GetDevice());

    static_mesh[0] = std::make_unique<StaticMesh>(graphics->GetDevice(),modelfilename[0],true);
    //�o�E���f�B���O�{�b�N�X
    static_mesh[1] = std::make_unique<StaticMesh>(graphics->GetDevice(), modelfilename[1], false);

    model[0] = std::make_unique<Model>(graphics->GetDevice(), ".\\Data\\resources\\nico.fbx",0,true);
    //model[0]->AppendAnimations(".\\Data\\resources\\AimTest\\Aim_Space.fbx", 0);

    framebuffers[0]= std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280,720);
    // �I�t�X�N���[���`��p�̃V�F�[�_�[���\�[�X�r���[�`��p�̃X�v���C�g�̍쐬
    bit_block_transfer = std::make_unique<FullScreenQuad>(graphics->GetDevice());

    gltf_models[0] = std::make_unique<GltfModel>(graphics->GetDevice(), ".\\Data\\glTF-Sample-Models-main\\2.0\\2CylinderEngine\\glTF\\2CylinderEngine.gltf");
}

//�I����
void SceneTest::Finalize()
{

}

//�X�V����
void SceneTest::Update(float elapsedTime)
{
    elapsedTime_ = elapsedTime;
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
    
    ImGui::SliderFloat("neckangle", &factor[0], -1.5f, 1.5f);
    ImGui::SliderFloat("necklong", &factor[1], 0, 500.0f);
    ImGui::SliderFloat("animation", &factor[2], -1.0f,1.0f);
    ImGui::End();
#endif
}

//�`��
void SceneTest::Render()
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();

    ID3D11DeviceContext* dc = graphics->GetDeviceContext();

    //�r���[�|�[�g�擾
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //�`��R���e�L�X�g
    RenderContext rc;
    rc.deviceContext = dc;
    rc.renderState = renderState;
    rc.view = camera->GetView();
    rc.projection = camera->GetProjection();
    rc.lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f };	// ���C�g�����i�������j

    framebuffers[0]->Clear(dc);
    framebuffers[0]->Activate(dc);

    // 2D �`��ݒ�
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    // 2D �`��
    {
        rc.renderState->GetSamplerState(SAMPLER_STATE::POINT);
      /*  sprite_batches[0]->Begin(dc, sprite_batches[0]->GetReplaced_pixel_shader(), sprite_batches[0]->GetReplaced_Shader_resource_view());
        sprite_batches[0]->Render(dc, 0, 0, 1280, 720);
        sprite_batches[0]->End(dc);*/

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
    scene_data.camera_position = camera_position;
#endif

    // 3D �`��ݒ�
    rc.renderState->GetSamplerState(SAMPLER_STATE::ANISOTROPIC);
    dc->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));

    // 3D �`��
    {
        //�萔�o�b�t�@�̓o�^
        BindBuffer(dc, 1, buffer.GetAddressOf(), &scene_data);
#if 1
        //���W�n�ϊ��p�̍s��
        const DirectX::XMFLOAT4X4 coordinate_system_transform[]
        {
            { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:�E��n Y-UP
            { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:����n Y-UP
            { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:�E��n Z-UP
            { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:����n Z-UP
        };
            //�f�t�H���g�̃X�P�[���t�@�N�^��ݒ肵�čs��ɔ��f
        const float scale_factor = 0.01f;
        DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
            //�g��k���s��
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
            // ��]�s��
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
            // ���s�ړ��s��
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x,tramslation.y,tramslation.z) };

            // ���[���h�ϊ��s��
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, C * S * R * T);
#if 1
            int clip_index{ 0 };
            int frame_index{ 0 };
            static float animation_tick{ 0 };

            animation& animation{ model[0]->animation_clips.at(clip_index) };
            frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
            if (frame_index > animation.sequence.size() - 1)
            {
                frame_index = 0;
                animation_tick = 0;
            }
            else
            {
                animation_tick += elapsedTime_;
            }
            animation::keyframe& keyframe{ animation.sequence.at(frame_index) };
#if 1
            // �f�o�b�O�p(����������č��ɉ�])
            DirectX::XMStoreFloat4(&keyframe.nodes.at(30).rotation, DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1, 0, 0, 0), factor[0]));
            keyframe.nodes.at(30).translation.x = factor[1];
            //model[0]->UpdateAnimation();
#endif
#else
            // �T�ԖڂƂX�Ԗڂ̃A�j���[�V������ݒ�
            animation::keyframe keyframe;
            const animation::keyframe* keyframes[2]{
                &model[0]->animation_clips.at(0).sequence.at(40),
                &model[0]->animation_clips.at(0).sequence.at(80)
            };
            // �A�j���[�V����������
            model[0]->BlendAnimations(keyframes, factor[2], keyframe);
            // ���������A�j���[�V�����Ń��b�V�����X�V
            model[0]->UpdateAnimation(keyframe);
#endif
#endif
            ////model[0]->Render(dc, world, material_color, &keyframe);
            gltf_models[0]->Render(dc, world);
            framebuffers[0]->Deactivate(dc);
#if 1
            dc->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
            dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
            bit_block_transfer->Blit(dc, framebuffers[0]->shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf(), 0, 1);
#endif
#if 0
        //�W�I���g���b�N�v���~�e�B�u�`��
        {

             //�g��k���s��
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
            // ��]�s��
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
            // ���s�ړ��s��
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x - 1.0f,tramslation.y,tramslation.z) };

            // ���[���h�ϊ��s��
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, S * R * T);
            geometric_primitives[0]->Render(dc, world, { material_color });
        }
        {
            dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::WIRE_CULLBACK));
            //�g��k���s��
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
            // ��]�s��
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
            // ���s�ړ��s��
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x + 1.0f,tramslation.y,tramslation.z) };

            // ���[���h�ϊ��s��
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, S * R * T);

            DirectX::XMStoreFloat4x4(&world, S * R * T);
            geometric_primitives[1]->Render(dc, world, { material_color });
        }
#endif
        //�X�^�e�B�b�N���b�V���`��
        {
            ////�g��k���s��
            //DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x,scaling.y,scaling.z) };
            //// ��]�s��
            //DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
            //// ���s�ړ��s��
            //DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x,tramslation.y,tramslation.z) };

            //// ���[���h�ϊ��s��
            //DirectX::XMFLOAT4X4 world;
            //DirectX::XMStoreFloat4x4(&world, S* R* T);
            //static_mesh[0]->Render(dc, world, { 1.0f,1.0f,1.0f,1.0f },PIXEL_SHADER_STATE::DEFAULT);
            //{
            //    dc->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::WIRE_CULLNONE));
            //    // �o�E���f�B���O�{�b�N�X�̑傫�����X�V
            //    const DirectX::XMFLOAT3 min[2]{ static_mesh[0]->bounding_box[0], static_mesh[1]->bounding_box[0] };
            //    const DirectX::XMFLOAT3 max[2]{ static_mesh[0]->bounding_box[1], static_mesh[1]->bounding_box[1] };
            //    DirectX::XMFLOAT3 dimensions[2]
            //    {
            //    { max[0].x - min[0].x, max[0].y - min[0].y, max[0].z - min[0].z },
            //    { max[1].x - min[1].x, max[1].y - min[1].y, max[1].z - min[1].z }
            //    };
            //    // �o�E���f�B���O�{�b�N�X�̈ʒu���X�V
            //    DirectX::XMFLOAT3 barycenters[2]
            //    {
            //    { (max[0].x + min[0].x) * 0.5f, (max[0].y + min[0].y) * 0.5f, (max[0].z + min[0].z) * 0.5f },
            //    { (max[1].x + min[1].x) * 0.5f, (max[1].y + min[1].y) * 0.5f, (max[1].z + min[1].z) * 0.5f }
            //    };
            //    DirectX::XMFLOAT3 ratio{ dimensions[0].x / dimensions[1].x, dimensions[0].y / dimensions[1].y, dimensions[0].z / dimensions[1].z };
            //    DirectX::XMFLOAT3 offset{ barycenters[0].x - barycenters[1].x, barycenters[0].y - barycenters[1].y, barycenters[0].z - barycenters[1].z };
            //    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(ratio.x * scaling.x, ratio.y * scaling.y, ratio.z * scaling.z) };
            //    DirectX::XMMATRIX O{ DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z) };
            //    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
            //    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(tramslation.x, tramslation.y, tramslation.z) };
            //    DirectX::XMFLOAT4X4 world;
            //    DirectX::XMStoreFloat4x4(&world, S* O* R* T);
            //    static_mesh[1]->Render(dc, world, { 1, 0, 0, 1 },PIXEL_SHADER_STATE::GEOMETRICPRIMITEVE);
            //}
            //
        }
    }

    // 3D�f�o�b�O�`��
    {

    }
}