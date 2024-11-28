#include"..\misc.h"
#include"Model.h"
#include<sstream>
#include<functional>
#include<filesystem>
#include"Shader.h"
#include<fstream>

// FbxAMatrix����XMFLOAT�SX4�ɕϊ�
inline DirectX::XMFLOAT4X4 ToXmFloat4x4(const FbxAMatrix& fbxamatrix)
{
    DirectX::XMFLOAT4X4 xmfloat4x4;
    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
    return xmfloat4x4;
}

// FbxDouble3����XMFLOAT3�ɕϊ�
inline DirectX::XMFLOAT3 ToXmFloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

// FbxDouble4����XMFLOAT4�ɕϊ�
inline DirectX::XMFLOAT4 ToXmFloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

// �P�̒��_���e�����󂯂�{�[���̏��
struct bone_influence
{
    uint32_t bone_index;// �{�[���ԍ�
    float bone_weight;  // �E�F�C�g�l
};
// �P�̒��_�͕����̃{�[������e�����󂯂�ꍇ������̂ŉϒ��z��ŕ\��
using bone_influences_per_control_point = std::vector<bone_influence>;

void FetchBoneInfluences(const FbxMesh* fbx_mesh,
    std::vector<bone_influences_per_control_point>& bone_influences)
{
    // �{�[���e���x�̐� = FBX ���b�V���ɂ���R���g���[���|�C���g�̐��ɐݒ�
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };
    bone_influences.resize(control_points_count);
    // ���b�V���ɂ���X�L���̐����擾
    const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    // ���b�V���ɂ��邷�ׂẴX�L���̏����`�F�b�N
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
    {
        // ���݂̃X�L�����擾
        const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index,FbxDeformer::eSkin)) };

        // �X�L���ɂ���N���X�^�[�̐����擾
        const int cluster_count{ fbx_skin->GetClusterCount() };
        // �X�L���ɂ��邷�ׂẴN���X�^�[�̏����`�F�b�N
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // ���݂̃N���X�^�[�̏����`�F�b�N
            const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

            // �N���X�^�[�ɂ���R���g���[���|�C���g�̐����擾
            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            // �N���X�^�[�ɂ��邷�ׂẴR���g���[���|�C���g�̃E�F�C�g�̒l���擾
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
            {
                // ���݂̃R���g���[���|�C���g�̔ԍ�
                int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                // �R���g���[���|�C���g�̃E�F�C�g�l
                double control_point_weight{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                // �V�K�Ƀ{�[���e���x�𐶐����Ēǉ�
                bone_influence& bone_influence_{ bone_influences.at(control_point_index).emplace_back() };
                // ���݂̃N���X�^�[�̔ԍ����{�[���ԍ��Ƃ��Đݒ�
                bone_influence_.bone_index = static_cast<uint32_t>(cluster_index);
                // �R���g���[���̃E�F�C�g�l���{�[���̃E�F�C�g�l�Ƃ��Đݒ�
                bone_influence_.bone_weight = static_cast<float>(control_point_weight);
            }
        }
    }
}

Model::Model(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
{
    // �ʏ�̃t�@�C�����̊g���q�� .fbx ���� .cereal �ɕύX����
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂���ꍇ
        // �V���A���C�Y����Ă���t�@�C����ǂݍ����
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂��Ȃ��ꍇ
        // �ʏ�� FBX �t�@�C���ǂݍ���
        
        // fbx�}�l�[�W���[�𐶐�
        FbxManager* fbx_manager{ FbxManager::Create() };
        // fbx�V�[���𐶐�
        fbx_scene = { FbxScene::Create(fbx_manager,"") };

        // fbx�C���|�[�^�[�̐���
        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
        bool import_status{ false };

        // fbx�t�@�C����ǂݍ���
        import_status = fbx_importer->Initialize(fbx_filename);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        // �ǂݍ��񂾃t�@�C���̏��� fbx �V�[���ɗ�������
        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (triangulate)
        {
            // ���p�`�ō��ꂽ�|���S�������ׂĎO�p�`�ɂ���
            fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }

        // �m�[�h�̏�����͂���֐�
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbx_node)
            {
                // scene_view.nodes �ɐV�����m�[�h�����t���A���t�����m�[�h�����[�J���� node �ɑ�����l��ݒ肵�Ă���
                scene::node& node_{scene_view.nodes.emplace_back()};
                node_.attribute = fbx_node->GetNodeAttribute() ?
                    fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node_.name = fbx_node->GetName();
                node_.unique_id = fbx_node->GetUniqueID();
                node_.parent_index = scene_view.IndexOf(fbx_node->GetParent() ?
                    fbx_node->GetParent()->GetUniqueID() : 0);
                // ���݉�͂��Ă���m�[�h�Ɏ��t�����Ă���̎q�m�[�h����͂��Ă���
                for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
                {
                    traverse(fbx_node->GetChild(child_index));
                }
            }
        };
        // ���[�g�m�[�h�����͊J�n
        traverse(fbx_scene->GetRootNode());
        for (const scene::node& node : scene_view.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbx_node={ fbx_scene->FindNodeByName(node.name.c_str()) };
            //�o�̓E�B���h�E�Ƀf�o�b�O�f�[�^��\��
        }

#if 0
        //�V�[���Ɏ��t�������ׂẴm�[�h�̏���\��
        for (const scene::node& node : scene_view.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
            //�o�̓E�B���h�E�Ƀf�o�b�O�f�[�^��\��
            std::string node_name = fbx_node->GetName();
            uint64_t uid = fbx_node->GetUniqueID();
            uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
            int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debug_string;
            debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << '\n';
            OutputDebugStringA(debug_string.str().c_str());
        }
#endif

        FetchMeshes(fbx_scene, meshes);

        FetchMaterials(fbx_scene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbx_scene, animation_clips, sampling_rate);

        fbx_manager->Destroy();

        // FBX �t�@�C����ǂݍ��񂾂�
        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // �O���t�@�C���ɃV���A���C�Y
        serialization(scene_view, meshes, materials, animation_clips);
    }

    CreateComObjects(device, fbx_filename);
}

Model::Model(ID3D11Device* device, std::vector<std::string>& animation_filenames,
    const char* fbx_filename, bool triangulate, float sampling_rate)
{
    // �ʏ�̃t�@�C�����̊g���q�� .fbx ���� .cereal �ɕύX����
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂���ꍇ
        // �V���A���C�Y����Ă���t�@�C����ǂݍ����
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂��Ȃ��ꍇ
        // �ʏ�� FBX �t�@�C���ǂݍ���

        // fbx�}�l�[�W���[�𐶐�
        FbxManager* fbx_manager{ FbxManager::Create() };
        // fbx�V�[���𐶐�
        FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

        // fbx�C���|�[�^�[�̐���
        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
        bool import_status{ false };

        // fbx�t�@�C����ǂݍ���
        import_status = fbx_importer->Initialize(fbx_filename);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        // �ǂݍ��񂾃t�@�C���̏��� fbx �V�[���ɗ�������
        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (triangulate)
        {
            // ���p�`�ō��ꂽ�|���S�������ׂĎO�p�`�ɂ���
            fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }

        // �m�[�h�̏�����͂���֐�
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbx_node)
            {
                // scene_view.nodes �ɐV�����m�[�h�����t���A���t�����m�[�h�����[�J���� node �ɑ�����l��ݒ肵�Ă���
                scene::node& node_={scene_view.nodes.emplace_back()};
                node_.attribute = fbx_node->GetNodeAttribute() ?
                    fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node_.name = fbx_node->GetName();
                node_.unique_id = fbx_node->GetUniqueID();
                node_.parent_index = scene_view.IndexOf(fbx_node->GetParent() ?
                    fbx_node->GetParent()->GetUniqueID() : 0);
                // ���݉�͂��Ă���m�[�h�Ɏ��t�����Ă���̎q�m�[�h����͂��Ă���
                for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
                {
                    traverse(fbx_node->GetChild(child_index));
                }
            }
        };

#if 0
        //�V�[���Ɏ��t�������ׂẴm�[�h�̏���\��
        for (const scene::node& node : scene_view.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
            //�o�̓E�B���h�E�Ƀf�o�b�O�f�[�^��\��
            std::string node_name = fbx_node->GetName();
            uint64_t uid = fbx_node->GetUniqueID();
            uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
            int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debug_string;
            debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << '\n';
            OutputDebugStringA(debug_string.str().c_str());
        }
#endif
        // ���[�g�m�[�h�����͊J�n
        traverse(fbx_scene->GetRootNode());

        FetchMeshes(fbx_scene, meshes);

        FetchMaterials(fbx_scene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbx_scene, animation_clips, sampling_rate);

        fbx_manager->Destroy();

        // �ʏ�� FBX �t�@�C���̓ǂݍ��݂̌�ɃA�j���[�V���� FBX �t�@�C���̓ǂݍ��݂��s���悤�ɂ���
        for (const std::string animation_filename : animation_filenames)
        {
            AppendAnimations(animation_filename.c_str(), sampling_rate);
        }

        // FBX �t�@�C����ǂݍ��񂾂�
        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // �O���t�@�C���ɃV���A���C�Y
        serialization(scene_view, meshes, materials, animation_clips);
    }

    CreateComObjects(device, fbx_filename);
}

//�`�揈��
void Model::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
{
    //���b�V�����Ƃɕ`�揈�����s��
    for (const mesh& mesh_ : meshes)
    {

        // ���_�o�b�t�@�̐ݒ�
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
        // �C���f�b�N�X�o�b�t�@�̐ݒ�
        immediate_context->IASetIndexBuffer(mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        // �v���~�e�B�u�̃g�|���W�[�ݒ�i�O�p�`���X�g�j
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // ���̓��C�A�E�g�̐ݒ�
        immediate_context->IASetInputLayout(input_layout.Get());

        // ���_�V�F�[�_�[�̐ݒ�
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        // �s�N�Z���V�F�[�_�[�̐ݒ�
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        // �萔�o�b�t�@�̍X�V�i���[���h�s��ƃ}�e���A���J���[�̐ݒ�
        constants data;
        if (&keyframe && keyframe.nodes.size() > 0)
        {
            // ���b�V���̈ʒu�E�p���s����L�[�t���[������擾
            const animation::keyframe::node& mesh_node{ keyframe.nodes.at(mesh_.node_index) };
            // �擾�����L�[�t���[�����Ƃ̈ʒu�E�p���s������[���h�ϊ��s��ɍ�������
            DirectX::XMStoreFloat4x4(&data.world,
                DirectX::XMLoadFloat4x4(&mesh_node.global_transform) * DirectX::XMLoadFloat4x4(&world));
#if 0
            DirectX::XMStoreFloat4x4(&data.bone_transforms[0], DirectX::XMMatrixIdentity());
            DirectX::XMStoreFloat4x4(&data.bone_transforms[1], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)));
            DirectX::XMStoreFloat4x4(&data.bone_transforms[2], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)));
#endif

#if 0
            DirectX::XMMATRIX B[3]; // �o�C���h�|�[�Y�ϊ�(�I�t�Z�b�g�s��)�F���f��(���b�V��)��Ԃ���{�[����Ԃɕϊ�
            B[0] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(0).offset_transform);
            B[1] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(1).offset_transform);
            B[2] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(2).offset_transform);

            DirectX::XMMATRIX A[3]; // �A�j���[�V�����{�[���ϊ��F�{�[����Ԃ��烂�f��(���b�V��)�܂��͐e�{�[����Ԃɕϊ�
            A[0] = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0);	                                //A0 ��Ԃ��烂�f����Ԃ�
            A[1] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A1�X�y�[�X����e�{�[��(A0)�X�y�[�X��
            A[2] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A2�X�y�[�X����e�{�[��(A1)�X�y�[�X��

            DirectX::XMStoreFloat4x4(&data.bone_transforms[0], B[0] * A[0]);
            DirectX::XMStoreFloat4x4(&data.bone_transforms[1], B[1] * A[1] * A[0]);
            DirectX::XMStoreFloat4x4(&data.bone_transforms[2], B[1] * A[2] * A[1] * A[0]);
#endif
            const size_t bone_count{ mesh_.bind_pose.bones.size() };
            for (int bone_index = 0; bone_index < bone_count; ++bone_index)
            {
                const skeleton::bone& bone{ mesh_.bind_pose.bones.at(bone_index) };
                const animation::keyframe::node& bone_node{ keyframe.nodes.at(bone.node_index) };
                DirectX::XMStoreFloat4x4(&data.bone_transforms[bone_index],
                    DirectX::XMLoadFloat4x4(&bone.offset_transform) *
                    DirectX::XMLoadFloat4x4(&bone_node.global_transform) *
                    DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh_.default_global_transform)));
            }
        }
        else
        {
            DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh_.default_global_transform) * DirectX::XMLoadFloat4x4(&world));
            for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
            {
                data.bone_transforms[bone_index] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            }
        }
        for (const mesh::subset& subset : mesh_.subsets)
        {
            // �}�e���A���̎���ID ����}�e���A�����擾���Q�ƂƂ��Đݒ�
            const material& material_{ materials.at(subset.material_unique_id) };

            DirectX::XMStoreFloat4(&data.material_color,
                DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&material_color), DirectX::XMLoadFloat4(&material_.Kd)));
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
            immediate_context->PSSetShaderResources(0, 1,
                material_.shader_resource_views[0].GetAddressOf());
            immediate_context->PSSetShaderResources(1, 1,
                material_.shader_resource_views[1].GetAddressOf());
            //immediate_context->PSSetShaderResources(1, 2,
            //    material_.shader_resource_views[2].GetAddressOf());
            // �C���f�b�N�X���g�p���ĕ`��
            //D3D11_BUFFER_DESC buffer_desc;
            //mesh_.index_buffer->GetDesc(&buffer_desc);
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}

// �A�j���[�V�����̍X�V
void Model::UpdateAnimation(float elapsedTime)
{
    //�Đ����łȂ��Ȃ珈�����Ȃ�
    if (!IsPlayAnimation())
        return;

    // �u�����h��Ԃɂ����鎞��
    float blendRate = 1.0f;
    if (animationBlendTime < animationBlendSeconds)
    {
        animationBlendTime += elapsedTime;
        if (animationBlendTime >= animationBlendSeconds)
        {
            animationBlendTime = animationBlendSeconds;
        }
        blendRate = animationBlendTime / animationBlendSeconds;
        blendRate *= blendRate;
    }

    // �A�j���[�V�����̔ԍ�
    int clip_index{ currentAnimationIndex };

    int frame_index{ 0 };
    animation& animation{ animation_clips.at(clip_index) };
    frame_index = static_cast<int>(currentAnimationSeconds * animation.sampling_rate);
    if (frame_index > animation.sequence.size() - 1)
    {
        frame_index = animation.sequence.size() - 1;
    }

    if (blendRate < 1.0f)
    {
        // �u�����h�Đ�
        const animation::keyframe* keyframes[2]{
            &keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &animation_clips.at(clip_index).sequence.at(frame_index),
        };

        // �u�����h���
        BlendAnimations(keyframes, blendRate, keyframe);
        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ keyframe.nodes.size() };
        //keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }
    else
    {
        // �ʏ�Đ�
        keyframe = animation.sequence.at(frame_index);

        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ keyframe.nodes.size() };
        keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

    {
        float oldAnimationSeconds = 0;
        if (oldAnimationSeconds > currentAnimationSeconds)
        {

        }
    }

    //�ŏI�t���[������
    if (animationEndFlag)
    {
        animationEndFlag = false;
        currentAnimationIndex = -1;
        return;
    }
    //���Ԍo��
    currentAnimationSeconds += elapsedTime;

    // �A�j���[�V�����S�̂̒������v�Z
    float totalAnimationTime = 
        animation_clips.at(currentAnimationIndex).sequence.size() *
        (1.0f / animation_clips.at(currentAnimationIndex).sampling_rate);
    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (currentAnimationSeconds >= totalAnimationTime)
    {
        //�Đ����Ԃ������߂�
        if (animLoop)
        {
            currentAnimationSeconds -= totalAnimationTime;
        }
        else
        {
            currentAnimationSeconds = totalAnimationTime;
            animationEndFlag = true;
        }
    }
}

// �A�j���[�V�����̒ǉ�
bool Model::AppendAnimations(const char* animation_filename, float sampling_rate)
{
    FbxManager* fbx_manager{ FbxManager::Create() };
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
    bool import_status{ false };
    import_status = fbx_importer->Initialize(animation_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    // 0...�f�t�H���g�l���g�p���܂��B0 ����...�t�F�b�`���܂���
    FetchAnimations(fbx_scene, animation_clips, sampling_rate);

    fbx_manager->Destroy();

    return true;
}

void Model::BlendAnimations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe)
{
    size_t node_count{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(node_count);
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        DirectX::XMVECTOR S[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2]{
            DirectX::XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
            DirectX::XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
        DirectX::XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(node_index).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
    }
}

// ���b�V�����̎��o��
void Model::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    // �V�[���̒��ɂ���m�[�h�̏������ׂČ���
    for (const scene::node& node_ : scene_view.nodes)
    {
        // �m�[�h�̒����烁�b�V���̑����������Ă���m�[�h���`�F�b�N
        if (node_.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // �V�[������ FbxNode ���擾
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node_.name.c_str()) };
        // ���b�V�������擾
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        // ���b�V����V�K�ō쐬�����t��
        mesh& mesh_{ meshes.emplace_back() };
        // ���b�V���̎���ID�̐ݒ�
        mesh_.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        // ���b�V�����̐ݒ�
        mesh_.name = fbx_mesh->GetNode()->GetName();
        // ���b�V���ɑ΂���m�[�hID�̊���U��
        mesh_.node_index = scene_view.IndexOf(mesh_.unique_id);
        
        // ���b�V���̃O���[�o���s����擾��XMFLOAT4X4�ɕϊ����đ��
        mesh_.default_global_transform = ToXmFloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

        // ���b�V������{�[���e���x���擾
        std::vector<bone_influences_per_control_point> bone_influences;
        FetchBoneInfluences(fbx_mesh, bone_influences);

        // ���b�V������o�C���h�|�[�Y(�����p��)�̏��̎��o��
        FetchSkeletons(fbx_mesh, mesh_.bind_pose);

        std::vector<mesh::subset>& subsets{ mesh_.subsets };
        // �}�e���A�������擾
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        // �T�u�Z�b�g�̐������T�C�Y
        subsets.resize(material_count > 0 ? material_count : 1);
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            //�}�e���A�������擾
            const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
            subsets.at(material_index).material_name = fbx_material->GetName();
            subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
        }

        if (material_count > 0)
        {
            const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
            {
                // ���b�V���̃|���S���̔ԍ�����C���f�b�N�X�ԍ����擾���}�e���A���̃C���f�b�N�X�̔ԍ��Ƃ��Ċ��蓖�Ă�
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                // �T�u�Z�b�g�ɑ΂��Ċ��蓖�Ă�ꂽ�}�e���A���̃C���f�b�N�X�̔ԍ��ɃC���f�b�N�X�̐��𒸓_����(3)���₵�Ă���
                subsets.at(material_index).index_count += 3;
            }
            // �C���f�b�N�X�̐����炻�ꂼ��̊J�n�̐����v�Z���Đݒ肵�Ă���
            uint32_t offset{ 0 };
            for (mesh::subset& subset : subsets)
            {
                subset.start_index_location = offset;
                offset += subset.index_count;
                subset.index_count = 0;
            }
        }

        const int polygon_count{ fbx_mesh->GetPolygonCount() };         //�|���S����
        mesh_.vertices.resize(polygon_count * 3LL);                     //���_���W��
        mesh_.indices.resize(polygon_count * 3LL);                      //���_�C���f�b�N�X��

        // uv���̎擾�B��X�e�N�X�`�����W�̎擾�ɗ��p�B
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        // �R���g���[���|�C���g�̎擾
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        // �|���S���̐��������_�f�[�^���擾
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };
            // �O�p�`�̐����̒��_�̏����擾����
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                // ���p����C���f�b�N�X�̔z����v�Z
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                // ���_���W�̎擾
                vertex vertex_;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index,position_in_polygon) };
                vertex_.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex_.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex_.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                // ���_���Ԗڂ̃{�[���e���x���擾
                const bone_influences_per_control_point& influces_per_control_point{ bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influces_per_control_point.size(); ++influence_index)
                {
                    // �{�[���e���x�͍ő�S��
                    if (influence_index < MAX_BONE_INFLUENCES)
                    {
                        // �擾�����{�[���e���x�𒸓_���̃{�[���e���x�ɐݒ肵�Ă���
                        vertex_.bone_weights[influence_index] =
                            influces_per_control_point.at(influence_index).bone_weight;
                        vertex_.bone_indices[influence_index] =
                            influces_per_control_point.at(influence_index).bone_index;
                    }
                    else
                    {
                        // �ŏ��̉e���x�����{�[����T��
                        size_t minimum_value_index = 0;
                        float minimum_value = FLT_MAX;
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            if (minimum_value > vertex_.bone_weights[i])
                            {
                                minimum_value = vertex_.bone_weights[i];
                                minimum_value_index = i;
                            }
                        }

                        // �ŏ��̉e���x�����{�[���̉e���x�����Z���A�C���f�b�N�X���X�V����
                        vertex_.bone_weights[minimum_value_index] += influces_per_control_point[influence_index].bone_weight;
                        vertex_.bone_indices[minimum_value_index] = influces_per_control_point[influence_index].bone_index;

                        // ���v�̉e���x���v�Z���A�S�Ẳe���x�𐳋K������
                        float total_weight = 0.0f;
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            total_weight += vertex_.bone_weights[i];
                        }

                        // �e���x�𐳋K������
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            if (total_weight > 0.0f) {
                                vertex_.bone_weights[i] /= total_weight;
                            }
                            else {
                                vertex_.bone_weights[i] = 0.0f;
                            }
                        }
                    }
                }

                // �@���̎擾
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex_.normal.x = static_cast<float>(normal[0]);
                    vertex_.normal.y = static_cast<float>(normal[1]);
                    vertex_.normal.z = static_cast<float>(normal[2]);
                }

                // �e�N�X�`�����W�̎擾
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex_.texcoord.x = static_cast<float>(uv[0]);
                    vertex_.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                // �ڐ��x�N�g���̎擾
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex_.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex_.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex_.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex_.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }


                // ���݂̃C���f�b�N�X�ԍ������ɒ��_�f�[�^��ݒ�
                mesh_.vertices.at(vertex_index) = std::move(vertex_);
                // ���݂̃C���f�b�N�X�ԍ���ݒ�
                mesh_.indices.at(static_cast<size_t>(offset)+position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
        // ���b�V�����Ƃ̃o�E���f�B���O�{�b�N�X�̍��W���v�Z
        for (const vertex& v : mesh_.vertices)
        {
            mesh_.bounding_box[0].x = std::min<float>(mesh_.bounding_box[0].x, v.position.x);
            mesh_.bounding_box[0].y = std::min<float>(mesh_.bounding_box[0].y, v.position.y);
            mesh_.bounding_box[0].z = std::min<float>(mesh_.bounding_box[0].z, v.position.z);
            mesh_.bounding_box[1].x = std::max<float>(mesh_.bounding_box[1].x, v.position.x);
            mesh_.bounding_box[1].y = std::max<float>(mesh_.bounding_box[1].y, v.position.y);
            mesh_.bounding_box[1].z = std::max<float>(mesh_.bounding_box[1].z, v.position.z);
        }
    }
}

// �}�e���A�����̎��o��
void Model::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
    // �V�[���̒��ɂ���m�[�h�̏������ׂČ���
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        // �m�[�h�̏��P�P���`�F�b�N
        const scene::node& node{ scene_view.nodes.at(node_index) };
        // �V�[������FbxNode���擾
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        // �m�[�h�̒��ɂ���}�e���A���̏������ׂČ���
        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            // �}�e���A���̏��P�P���`�F�b�N
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            // �}�e���A���̐ݒ�
            material material_;
            material_.name = fbx_material->GetName();
            material_.unique_id = fbx_material->GetUniqueID();

            FbxProperty fbx_property;
            // �f�B�t���[�Y(�g�U���ˌ�)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // �f�B�t���[�Y����������f�B�t���[�Y�̏���ݒ肵�Ă���
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Kd.x = static_cast<float>(color[0]);
                material_.Kd.y = static_cast<float>(color[1]);
                material_.Kd.z = static_cast<float>(color[2]);
                material_.Kd.w = 1.0f;

                // �e�N�X�`���̃t�@�C�������擾
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // ���΃p�X���݂ł̃t�@�C������ݒ肷��
                material_.texture_filenames[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            // ���r�G���g(����)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ka.x = static_cast<float>(color[0]);
                material_.Ka.y = static_cast<float>(color[1]);
                material_.Ka.z = static_cast<float>(color[2]);
                material_.Ka.w = 1.0f;
            }
            // �X�y�L����(���ʔ���)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ks.x = static_cast<float>(color[0]);
                material_.Ks.y = static_cast<float>(color[1]);
                material_.Ks.z = static_cast<float>(color[2]);
                material_.Ks.w = 1.0f;
            }

            // �@���}�b�v�̃t�@�C����
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // �t�@�C���������݂����瑊�΃p�X���݂ł̃t�@�C�������擾
                material_.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // EMISSIVE
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material_.texture_filenames[2] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // �擾�����}�e���A���̏���ݒ肷��
            materials.emplace(material_.unique_id, std::move(material_));
        }
    }
#if 0
    // �_�~�[�̃}�e���A����}��
    material material_;
    materials.emplace(material_.unique_id, std::move(material_));
#endif
}

// �o�C���h�|�[�Y���̎��o��
void Model::FetchSkeletons(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
    // ���b�V���ɂ���X�L���̐����擾
    const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    // ���b�V���ɂ��邷�ׂẴX�L���̏����`�F�b�N
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
    {
        // ���݂̃X�L�����擾
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        // �X�L���ɂ���N���X�^�[�̐����擾
        const int cluster_count = skin->GetClusterCount();
        // ���b�V���̃{�[���̐� = ���b�V���̃N���X�^�[�̐�
        bind_pose.bones.resize(cluster_count);
        // �X�L���ɂ��邷�ׂẴN���X�^�[�̏����`�F�b�N
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // ���݂̃N���X�^�[�̏����`�F�b�N
            FbxCluster* cluster = skin->GetCluster(cluster_index);

            skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
            bone.name = cluster->GetLink()->GetName();
            bone.unique_id = cluster->GetLink()->GetUniqueID();
            bone.parent_index = bind_pose.IndexOf(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.node_index = scene_view.IndexOf(bone.unique_id);

            // �����N���܂ރm�[�h�Ɋ֘A�t����ꂽ�s����擾
            // ���f��(���b�V��)�̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg�p
            FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);

            // �����N�m�[�h�Ɋ֘A�t����ꂽ�s����擾
            // �{�[���̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg�p
            // ���[�J�����W��Ԃł̃{�[���̐�Έʒu
            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);

            bone.offset_transform = ToXmFloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
        }
    }
}

// �A�j���[�V�������̎��o��
void Model::FetchAnimations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
    float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    // �V�[������A�j���[�V�����ꗗ���擾
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    // �A�j���[�V�����ꗗ�̐����擾
    const int animation_stack_count{ animation_stack_names.GetCount() };
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        // �A�j���[�V�������擾
        animation& animation_clip{ animation_clips.emplace_back() };
        animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

        // �擾�����A�j���[�V�������A�j���[�V�����X�^�b�N�Ƃ��Đݒ�
        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
        // ���݂̃A�j���[�V�������A�j���[�V�����X�^�b�N�̃A�j���[�V�����ɕύX
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);
        animation_clip.sampling_rate = sampling_rate > 0 ?
            sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));

        //�T���v�����O���[�g����T���v�����O�̊Ԋu���Ԃ�ݒ�
        const FbxTime sampling_interval
        { static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
        const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };

        //�A�j���[�V�����̊J�n�ƏI�����擾
        const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            //�T���v�����O�Ԋu���Ƃ̃L�[�t���[�����擾
            animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

            const size_t node_count{ scene_view.nodes.size() };
            keyframe.nodes.resize(node_count);
            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
                if (fbx_node)
                {
                    animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
                    //�A�j���[�V�������Ԃ���A�j���[�V�����s����擾
                    node.global_transform = ToXmFloat4x4(fbx_node->EvaluateGlobalTransform(time));
                
                    // 'local_transform'�́A�e�̃��[�J�����W�n�Ɋւ���m�[�h�̕ϊ��s��ł�
                    const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
                    node.scaling = ToXmFloat3(local_transform.GetS());
                    node.rotation = ToXmFloat4(local_transform.GetQ());
                    node.translation = ToXmFloat3(local_transform.GetT());
                }
            }
        }
    }
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        delete animation_stack_names[animation_stack_index];
    }
}

// �o�b�t�@�̐���
void Model::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    // �e���b�V���ɑ΂��ăo�b�t�@�𐶐����郋�[�v
    for (mesh& mesh_ : meshes)
    {
        HRESULT hr{ S_OK };

        // ���_�o�b�t�@�̐���
        D3D11_BUFFER_DESC buffer_desc{};
        D3D11_SUBRESOURCE_DATA subresource_data{};
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh_.vertices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        subresource_data.pSysMem = mesh_.vertices.data();
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh_.vertex_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        // �C���f�b�N�X�o�b�t�@�̐���
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh_.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh_.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh_.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        // ���b�V���̒��_�ƃC���f�b�N�X���N���A����
        mesh_.vertices.clear();
        mesh_.indices.clear();
#endif
    }
    // �}�e���A���̒��ɂ���e�N�X�`���t�@�C�����̐������e�N�X�`���𐶐�����
    for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
#if 0
        for (size_t texture_index = 0; texture_index < 2; ++texture_index)
        {
            if (iterator->second.texture_filenames[0].size() > 0)
            {
                std::filesystem::path path(fbx_filename);
                path.replace_filename(iterator->second.texture_filenames[texture_index]);
                D3D11_TEXTURE2D_DESC texture2d_desc;
                ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(),
                    iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shader_resource_views[texture_index].GetAddressOf(),
                    texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
            }
        }
#else
        D3D11_TEXTURE2D_DESC texture2d_desc;
        // Diffuse
        if (iterator->second.texture_filenames[0].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[0]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 4);
        }
        // Normal
        if (iterator->second.texture_filenames[1].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[1]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 4);
        }
        // Emissive
        if (iterator->second.texture_filenames[2].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filenames[2]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shader_resource_views[2].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shader_resource_views[2].GetAddressOf(), 0xFF000000, 4);
        }
#endif
    }
    HRESULT hr = S_OK;
    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
       {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"BONES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
            input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixel_shader.ReleaseAndGetAddressOf());
    }
    
    // �萔�o�b�t�@�̐���
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
    currentAnimationIndex = index;
    currentAnimationSeconds = 0.0f;
    animLoop = loop;
    animationEndFlag = false;
    // �u�����h�p�g�[�^�����Ԃ����Z�b�g
    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
    animationBlendSeconds = 1;
}

//�A�j���[�V�����Đ�����
bool Model::IsPlayAnimation()const
{
    if (currentAnimationIndex < 0)return false;
    if (currentAnimationIndex >= animation_clips.size())return false;
    return true;
}

skeleton::bone* Model::FindNode(const char* name)
{
    // ���ׂẴ��b�V�������[�v
    for (auto& mesh : meshes)
    {
        // �e���b�V���̃{�[�����X�g�����[�v
        for (auto& bone : mesh.bind_pose.bones)
        {
            if (bone.name == name) // ���O����v���邩�m�F
            {
                return &bone; // �{�[���̃|�C���^��Ԃ�
            }
        }
    }
    return nullptr; // ������Ȃ������ꍇ
}