#include"..\Misc.h"
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
struct BoneInfluence
{
    uint32_t boneIndex;// �{�[���ԍ�
    float boneWeight;  // �E�F�C�g�l
};
// �P�̒��_�͕����̃{�[������e�����󂯂�ꍇ������̂ŉϒ��z��ŕ\��
using boneInfluencesPerControlPoint = std::vector<BoneInfluence>;

void FetchBoneInfluences(const FbxMesh* fbxMesh,
    std::vector<boneInfluencesPerControlPoint>& boneInfluences)
{
    // �{�[���e���x�̐� = FBX ���b�V���ɂ���R���g���[���|�C���g�̐��ɐݒ�
    const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
    boneInfluences.resize(controlPointsCount);
    // ���b�V���ɂ���X�L���̐����擾
    const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
    // ���b�V���ɂ��邷�ׂẴX�L���̏����`�F�b�N
    for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
    {
        // ���݂̃X�L�����擾
        const FbxSkin* fbxSkin{ static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex,FbxDeformer::eSkin)) };

        // �X�L���ɂ���N���X�^�[�̐����擾
        const int clusterCount{ fbxSkin->GetClusterCount() };
        // �X�L���ɂ��邷�ׂẴN���X�^�[�̏����`�F�b�N
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            // ���݂̃N���X�^�[�̏����`�F�b�N
            const FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };

            // �N���X�^�[�ɂ���R���g���[���|�C���g�̐����擾
            const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
            // �N���X�^�[�ɂ��邷�ׂẴR���g���[���|�C���g�̃E�F�C�g�̒l���擾
            for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
            {
                // ���݂̃R���g���[���|�C���g�̔ԍ�
                int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
                // �R���g���[���|�C���g�̃E�F�C�g�l
                double controlPointWeight{ fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
                // �V�K�Ƀ{�[���e���x�𐶐����Ēǉ�
                BoneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
                // ���݂̃N���X�^�[�̔ԍ����{�[���ԍ��Ƃ��Đݒ�
                boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
                // �R���g���[���̃E�F�C�g�l���{�[���̃E�F�C�g�l�Ƃ��Đݒ�
                boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
            }
        }
    }
}

Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate)
{
    // �ʏ�̃t�@�C�����̊g���q�� .fbx ���� .cereal �ɕύX����
    std::filesystem::path cerealFilename(fbxFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂���ꍇ
        // �V���A���C�Y����Ă���t�@�C����ǂݍ����
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂��Ȃ��ꍇ
        // �ʏ�� FBX �t�@�C���ǂݍ���
        
        // fbx�}�l�[�W���[�𐶐�
        FbxManager* fbxMgr
        { FbxManager::Create() };
        // fbx�V�[���𐶐�
        fbxScene = { FbxScene::Create(fbxMgr,"") };

        // fbx�C���|�[�^�[�̐���
        FbxImporter* fbxImporter{ FbxImporter::Create(fbxMgr,"") };
        bool importStatus{ false };

        // fbx�t�@�C����ǂݍ���
        importStatus = fbxImporter->Initialize(fbxFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        // �ǂݍ��񂾃t�@�C���̏��� fbx �V�[���ɗ�������
        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxMgr);
        if (triangulate)
        {
            // ���p�`�ō��ꂽ�|���S�������ׂĎO�p�`�ɂ���
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        // �m�[�h�̏�����͂���֐�
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbxNode)
            {
                // scene_view.nodes �ɐV�����m�[�h�����t���A���t�����m�[�h�����[�J���� node �ɑ�����l��ݒ肵�Ă���
                ModelScene::Node& node{sceneView.nodes.emplace_back()};
                node.attribute = fbxNode->GetNodeAttribute() ?
                    fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node.name = fbxNode->GetName();
                node.uniqueID = fbxNode->GetUniqueID();
                node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
                    fbxNode->GetParent()->GetUniqueID() : 0);
                // ���݉�͂��Ă���m�[�h�Ɏ��t�����Ă���̎q�m�[�h����͂��Ă���
                for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
                {
                    traverse(fbxNode->GetChild(childIndex));
                }
            }
        };
        // ���[�g�m�[�h�����͊J�n
        traverse(fbxScene->GetRootNode());
        for (const ModelScene::Node& node : sceneView.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbxNode={ fbxScene->FindNodeByName(node.name.c_str()) };
            //�o�̓E�B���h�E�Ƀf�o�b�O�f�[�^��\��
        }

#if 0
        //�V�[���Ɏ��t�������ׂẴm�[�h�̏���\��
        for (const ModelScene::Node& node : sceneView.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
            //�o�̓E�B���h�E�Ƀf�o�b�O�f�[�^��\��
            std::string nodeName = fbxNode->GetName();
            uint64_t uniqueID = fbxNode->GetUniqueID();
            uint64_t parentUID = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
            int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debugString;
            debugString << nodeName << ":" << uniqueID << ":" << parentUID << ":" << type << '\n';
            OutputDebugStringA(debugString.str().c_str());
        }
#endif

        FetchMeshes(fbxScene, meshes);

        FetchMaterials(fbxScene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbxScene, animationClips, samplingRate);

        fbxMgr->Destroy();

        // FBX �t�@�C����ǂݍ��񂾂�
        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // �O���t�@�C���ɃV���A���C�Y
        serialization(sceneView, meshes, materials, animationClips);
    }

    CreateComObjects(device, fbxFilename);
}

Model::Model(ID3D11Device* device, std::vector<std::string>& animationFilenames,
    const char* fbxFilename, bool triangulate, float samplingRate)
{
    // �ʏ�̃t�@�C�����̊g���q�� .fbx ���� .cereal �ɕύX����
    std::filesystem::path cerealFilename(fbxFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂���ꍇ
        // �V���A���C�Y����Ă���t�@�C����ǂݍ����
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        // �V���A���C�Y���ꂽ�t�@�C�������݂��Ȃ��ꍇ
        // �ʏ�� FBX �t�@�C���ǂݍ���

        // fbx�}�l�[�W���[�𐶐�
        FbxManager* fbxManager{ FbxManager::Create() };
        // fbx�V�[���𐶐�
        FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };

        // fbx�C���|�[�^�[�̐���
        FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager,"") };
        bool importStatus{ false };

        // fbx�t�@�C����ǂݍ���
        importStatus = fbxImporter->Initialize(fbxFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        // �ǂݍ��񂾃t�@�C���̏��� fbx �V�[���ɗ�������
        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxManager);
        if (triangulate)
        {
            // ���p�`�ō��ꂽ�|���S�������ׂĎO�p�`�ɂ���
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        // �m�[�h�̏�����͂���֐�
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbxNode)
            {
                // scene_view.nodes �ɐV�����m�[�h�����t���A���t�����m�[�h�����[�J���� node �ɑ�����l��ݒ肵�Ă���
                ModelScene::Node& node={sceneView.nodes.emplace_back()};
                node.attribute = fbxNode->GetNodeAttribute() ?
                    fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node.name = fbxNode->GetName();
                node.uniqueID = fbxNode->GetUniqueID();
                node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
                    fbxNode->GetParent()->GetUniqueID() : 0);
                // ���݉�͂��Ă���m�[�h�Ɏ��t�����Ă���̎q�m�[�h����͂��Ă���
                for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
                {
                    traverse(fbxNode->GetChild(childIndex));
                }
            }
        };

#if 0
        //�V�[���Ɏ��t�������ׂẴm�[�h�̏���\��
        for (const Scene::Node& node : sceneView.nodes)
        {
            //�m�[�h���Ńm�[�h���������擾
            FbxNode* fbx_node{ fbxScene->FindNodeByName(node.name.c_str()) };
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
        traverse(fbxScene->GetRootNode());

        FetchMeshes(fbxScene, meshes);

        FetchMaterials(fbxScene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbxScene, animationClips, samplingRate);

        fbxManager->Destroy();

        // �ʏ�� FBX �t�@�C���̓ǂݍ��݂̌�ɃA�j���[�V���� FBX �t�@�C���̓ǂݍ��݂��s���悤�ɂ���
        for (const std::string animation_filename : animationFilenames)
        {
            AppendAnimations(animation_filename.c_str(), samplingRate);
        }

        // FBX �t�@�C����ǂݍ��񂾂�
        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // �O���t�@�C���ɃV���A���C�Y
        serialization(sceneView, meshes, materials, animationClips);
    }

    CreateComObjects(device, fbxFilename);
}

//�`�揈��
void Model::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& materialColor)
{
    //���b�V�����Ƃɕ`�揈�����s��
    for (const Mesh& mesh : meshes)
    {

        // ���_�o�b�t�@�̐ݒ�
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        // �C���f�b�N�X�o�b�t�@�̐ݒ�
        dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        // �v���~�e�B�u�̃g�|���W�[�ݒ�i�O�p�`���X�g�j
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // ���̓��C�A�E�g�̐ݒ�
        dc->IASetInputLayout(inputLayout.Get());

        // ���_�V�F�[�_�[�̐ݒ�
        dc->VSSetShader(vertexShader.Get(), nullptr, 0);
        // �s�N�Z���V�F�[�_�[�̐ݒ�
        dc->PSSetShader(pixelShader.Get(), nullptr, 0);

        // �萔�o�b�t�@�̍X�V�i���[���h�s��ƃ}�e���A���J���[�̐ݒ�
        Constants data;
        if (&keyframe && keyframe.nodes.size() > 0)
        {
            // ���b�V���̈ʒu�E�p���s����L�[�t���[������擾
            const Animation::Keyframe::Node& meshNode{ keyframe.nodes.at(mesh.nodeIndex) };
            // �擾�����L�[�t���[�����Ƃ̈ʒu�E�p���s������[���h�ϊ��s��ɍ�������
            DirectX::XMStoreFloat4x4(&data.world,
                DirectX::XMLoadFloat4x4(&meshNode.globalTransform) * DirectX::XMLoadFloat4x4(&world));
#if 0
            DirectX::XMStoreFloat4x4(&data.boneTransforms[0], DirectX::XMMatrixIdentity());
            DirectX::XMStoreFloat4x4(&data.boneTransforms[1], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)));
            DirectX::XMStoreFloat4x4(&data.boneTransforms[2], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)));
#endif

#if 0
            DirectX::XMMATRIX B[3]; // �o�C���h�|�[�Y�ϊ�(�I�t�Z�b�g�s��)�F���f��(���b�V��)��Ԃ���{�[����Ԃɕϊ�
            B[0] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
            B[1] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
            B[2] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);

            DirectX::XMMATRIX A[3]; // �A�j���[�V�����{�[���ϊ��F�{�[����Ԃ��烂�f��(���b�V��)�܂��͐e�{�[����Ԃɕϊ�
            A[0] = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0);	                                //A0 ��Ԃ��烂�f����Ԃ�
            A[1] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A1�X�y�[�X����e�{�[��(A0)�X�y�[�X��
            A[2] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A2�X�y�[�X����e�{�[��(A1)�X�y�[�X��

            DirectX::XMStoreFloat4x4(&data.boneTransforms[0], B[0] * A[0]);
            DirectX::XMStoreFloat4x4(&data.boneTransforms[1], B[1] * A[1] * A[0]);
            DirectX::XMStoreFloat4x4(&data.boneTransforms[2], B[1] * A[2] * A[1] * A[0]);
#endif
            const size_t boneCount{ mesh.bindPose.bones.size() };
            for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
            {
                const Skeleton::Bone& bone{ mesh.bindPose.bones.at(boneIndex) };
                const Animation::Keyframe::Node& bone_node{ keyframe.nodes.at(bone.nodeIndex) };
                DirectX::XMStoreFloat4x4(&data.boneTransforms[boneIndex],
                    DirectX::XMLoadFloat4x4(&bone.offsetTransform) *
                    DirectX::XMLoadFloat4x4(&bone_node.globalTransform) *
                    DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform)));
            }
        }
        else
        {
            DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform) * DirectX::XMLoadFloat4x4(&world));
            for (size_t boneIndex = 0; boneIndex < MaxBones; ++boneIndex)
            {
                data.boneTransforms[boneIndex] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
            }
        }
        for (const Mesh::Subset& subset : mesh.subsets)
        {
            // �}�e���A���̎���ID ����}�e���A�����擾���Q�ƂƂ��Đݒ�
            const Material& material{ materials.at(subset.materialUniqueID) };

            DirectX::XMStoreFloat4(&data.materialColor,
                DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&materialColor), DirectX::XMLoadFloat4(&material.Kd)));
            dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
            dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
            dc->PSSetShaderResources(0, 1,
                material.shaderResourceViews[0].GetAddressOf());
            dc->PSSetShaderResources(1, 1,
                material.shaderResourceViews[1].GetAddressOf());
            dc->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
        }
    }
}

// �A�j���[�V�����̒ǉ�
bool Model::AppendAnimations(const char* animationFilename, float samplingRate)
{
    FbxManager* fbxMgr{ FbxManager::Create() };
    FbxScene* fbxScene{ FbxScene::Create(fbxMgr,"") };

    FbxImporter* fbxImporter{ FbxImporter::Create(fbxMgr,"") };
    bool importStatus{ false };
    importStatus = fbxImporter->Initialize(animationFilename);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
    importStatus = fbxImporter->Import(fbxScene);
    _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

    // 0...�f�t�H���g�l���g�p���܂��B0 ����...�t�F�b�`���܂���
    FetchAnimations(fbxScene, animationClips, samplingRate);

    fbxMgr->Destroy();

    return true;
}

void Model::BlendAnimations(const Animation::Keyframe* keyframes[2], float factor, Animation::Keyframe& keyframe)
{
    size_t nodeCount{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).scaling),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).scaling) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], factor));

        DirectX::XMVECTOR R[2]{
            DirectX::XMLoadFloat4(&keyframes[0]->nodes.at(nodeIndex).rotation),
            DirectX::XMLoadFloat4(&keyframes[1]->nodes.at(nodeIndex).rotation) };
        DirectX::XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], factor));

        DirectX::XMVECTOR T[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).translation),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).translation) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], factor));
    }
}

void Model::BlendAnimations(const Animation::Keyframe* keyframes[2], int nodeIndex, float rate, Animation::Keyframe& keyframe)
{
    size_t nodeCount{ keyframes[0]->nodes.size() };
    keyframe.nodes.resize(nodeCount);
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        DirectX::XMVECTOR S[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).scaling),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).scaling) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).scaling, DirectX::XMVectorLerp(S[0], S[1], rate));

        DirectX::XMVECTOR R[2]{
            DirectX::XMLoadFloat4(&keyframes[0]->nodes.at(nodeIndex).rotation),
            DirectX::XMLoadFloat4(&keyframes[1]->nodes.at(nodeIndex).rotation) };
        DirectX::XMStoreFloat4(&keyframe.nodes.at(nodeIndex).rotation, DirectX::XMQuaternionSlerp(R[0], R[1], rate));

        DirectX::XMVECTOR T[2]{
            DirectX::XMLoadFloat3(&keyframes[0]->nodes.at(nodeIndex).translation),
            DirectX::XMLoadFloat3(&keyframes[1]->nodes.at(nodeIndex).translation) };
        DirectX::XMStoreFloat3(&keyframe.nodes.at(nodeIndex).translation, DirectX::XMVectorLerp(T[0], T[1], rate));
    }
}

// ���b�V�����̎��o��
void Model::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
    // �V�[���̒��ɂ���m�[�h�̏������ׂČ���
    for (const ModelScene::Node& node : sceneView.nodes)
    {
        // �m�[�h�̒����烁�b�V���̑����������Ă���m�[�h���`�F�b�N
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // �V�[������ FbxNode ���擾
        FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
        // ���b�V�������擾
        FbxMesh* fbxMesh{ fbxNode->GetMesh() };

        // ���b�V����V�K�ō쐬�����t��
        Mesh& mesh{ meshes.emplace_back() };
        // ���b�V���̎���ID�̐ݒ�
        mesh.uniqueID = fbxMesh->GetNode()->GetUniqueID();
        // ���b�V�����̐ݒ�
        mesh.name = fbxMesh->GetNode()->GetName();
        // ���b�V���ɑ΂���m�[�hID�̊���U��
        mesh.nodeIndex = sceneView.IndexOf(mesh.uniqueID);
        
        // ���b�V���̃O���[�o���s����擾��XMFLOAT4X4�ɕϊ����đ��
        mesh.defaultGlobalTransform = ToXmFloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

        // ���b�V������{�[���e���x���擾
        std::vector<boneInfluencesPerControlPoint> boneInfluences;
        FetchBoneInfluences(fbxMesh, boneInfluences);

        // ���b�V������o�C���h�|�[�Y(�����p��)�̏��̎��o��
        FetchSkeletons(fbxMesh, mesh.bindPose);

        std::vector<Mesh::Subset>& subsets{ mesh.subsets };
        // �}�e���A�������擾
        const int materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
        // �T�u�Z�b�g�̐������T�C�Y
        subsets.resize(materialCount > 0 ? materialCount : 1);
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            //�}�e���A�������擾
            const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
            subsets.at(materialIndex).materialName = fbxMaterial->GetName();
            subsets.at(materialIndex).materialUniqueID = fbxMaterial->GetUniqueID();
        }

        if (materialCount > 0)
        {
            const int polygonCount{ fbxMesh->GetPolygonCount() };
            for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
            {
                // ���b�V���̃|���S���̔ԍ�����C���f�b�N�X�ԍ����擾���}�e���A���̃C���f�b�N�X�̔ԍ��Ƃ��Ċ��蓖�Ă�
                const int materialIndex{ fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
                // �T�u�Z�b�g�ɑ΂��Ċ��蓖�Ă�ꂽ�}�e���A���̃C���f�b�N�X�̔ԍ��ɃC���f�b�N�X�̐��𒸓_����(3)���₵�Ă���
                subsets.at(materialIndex).indexCount += 3;
            }
            // �C���f�b�N�X�̐����炻�ꂼ��̊J�n�̐����v�Z���Đݒ肵�Ă���
            uint32_t offset{ 0 };
            for (Mesh::Subset& subset : subsets)
            {
                subset.startIndexLocation = offset;
                offset += subset.indexCount;
                subset.indexCount = 0;
            }
        }

        const int polygonCount{ fbxMesh->GetPolygonCount() };         //�|���S����
        mesh.vertices.resize(polygonCount * 3LL);                     //���_���W��
        mesh.indices.resize(polygonCount * 3LL);                      //���_�C���f�b�N�X��

        // uv���̎擾�B��X�e�N�X�`�����W�̎擾�ɗ��p�B
        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);

        // �R���g���[���|�C���g�̎擾
        const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
        // �|���S���̐��������_�f�[�^���擾
        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
        {
            const int materialIndex{ materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
            Mesh::Subset& subset{ subsets.at(materialIndex) };
            const uint32_t offset{ subset.startIndexLocation + subset.indexCount };
            // �O�p�`�̐����̒��_�̏����擾����
            for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
            {
                // ���p����C���f�b�N�X�̔z����v�Z
                const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

                // ���_���W�̎擾
                vertex vertex;
                const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex,positionInPolygon) };
                vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
                vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
                vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

                // ���_���Ԗڂ̃{�[���e���x���擾
                const boneInfluencesPerControlPoint& influcesPerControlPoint{ boneInfluences.at(polygonVertex) };
                for (size_t influenceIndex = 0; influenceIndex < influcesPerControlPoint.size(); ++influenceIndex)
                {
                    // �{�[���e���x�͍ő�S��
                    if (influenceIndex < maxBoneInfluences)
                    {
                        // �擾�����{�[���e���x�𒸓_���̃{�[���e���x�ɐݒ肵�Ă���
                        vertex.boneWeights[influenceIndex] =
                            influcesPerControlPoint.at(influenceIndex).boneWeight;
                        vertex.boneIndices[influenceIndex] =
                            influcesPerControlPoint.at(influenceIndex).boneIndex;
                    }
                    else
                    {
                        // �ŏ��̉e���x�����{�[����T��
                        size_t minimumValueIndex = 0;
                        float minimumValue = FLT_MAX;
                        for (size_t i = 0; i < maxBoneInfluences; ++i)
                        {
                            if (minimumValue > vertex.boneWeights[i])
                            {
                                minimumValue = vertex.boneWeights[i];
                                minimumValueIndex = i;
                            }
                        }

                        // �ŏ��̉e���x�����{�[���̉e���x�����Z���A�C���f�b�N�X���X�V����
                        vertex.boneWeights[minimumValueIndex] += influcesPerControlPoint[influenceIndex].boneWeight;
                        vertex.boneIndices[minimumValueIndex] = influcesPerControlPoint[influenceIndex].boneIndex;

                        // ���v�̉e���x���v�Z���A�S�Ẳe���x�𐳋K������
                        float totalWeight = 0.0f;
                        for (size_t i = 0; i < maxBoneInfluences; ++i)
                        {
                            totalWeight += vertex.boneWeights[i];
                        }

                        // �e���x�𐳋K������
                        for (size_t i = 0; i < maxBoneInfluences; ++i)
                        {
                            if (totalWeight > 0.0f) {
                                vertex.boneWeights[i] /= totalWeight;
                            }
                            else {
                                vertex.boneWeights[i] = 0.0f;
                            }
                        }
                    }
                }

                // �@���̎擾
                if (fbxMesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }

                // �e�N�X�`�����W�̎擾
                if (fbxMesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmappedUV;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon,
                        uvNames[0], uv, unmappedUV);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                // �ڐ��x�N�g���̎擾
                if (fbxMesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
                }


                // ���݂̃C���f�b�N�X�ԍ������ɒ��_�f�[�^��ݒ�
                mesh.vertices.at(vertexIndex) = std::move(vertex);
                // ���݂̃C���f�b�N�X�ԍ���ݒ�
                mesh.indices.at(static_cast<size_t>(offset)+positionInPolygon) = vertexIndex;
                subset.indexCount++;
            }
        }
        // ���b�V�����Ƃ̃o�E���f�B���O�{�b�N�X�̍��W���v�Z
        for (const vertex& v : mesh.vertices)
        {
            mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
            mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
            mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
            mesh.boundingBox[1].x = std::max<float>(mesh.boundingBox[1].x, v.position.x);
            mesh.boundingBox[1].y = std::max<float>(mesh.boundingBox[1].y, v.position.y);
            mesh.boundingBox[1].z = std::max<float>(mesh.boundingBox[1].z, v.position.z);
        }
    }
}

// �}�e���A�����̎��o��
void Model::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
    // �V�[���̒��ɂ���m�[�h�̏������ׂČ���
    const size_t nodeCount{ sceneView.nodes.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        // �m�[�h�̏��P�P���`�F�b�N
        const ModelScene::Node& node{ sceneView.nodes.at(nodeIndex) };
        // �V�[������FbxNode���擾
        const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

        // �m�[�h�̒��ɂ���}�e���A���̏������ׂČ���
        const int materialCount{ fbxNode->GetMaterialCount() };
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            // �}�e���A���̏��P�P���`�F�b�N
            const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

            // �}�e���A���̐ݒ�
            Material material;
            material.name = fbxMaterial->GetName();
            material.uniqueID = fbxMaterial->GetUniqueID();

            FbxProperty fbxProperty;
            // �f�B�t���[�Y(�g�U���ˌ�)�̏����擾
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // �f�B�t���[�Y����������f�B�t���[�Y�̏���ݒ肵�Ă���
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                // �e�N�X�`���̃t�@�C�������擾
                const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                // ���΃p�X���݂ł̃t�@�C������ݒ肷��
                material.textureFilenames[0] =
                    fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            // ���r�G���g(����)�̏����擾
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            // �X�y�L����(���ʔ���)�̏����擾
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }

            // �@���}�b�v�̃t�@�C����
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                // �t�@�C���������݂����瑊�΃p�X���݂ł̃t�@�C�������擾
                material.textureFilenames[1] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }
            // EMISSIVE
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material.textureFilenames[2] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }
            // �擾�����}�e���A���̏���ݒ肷��
            materials.emplace(material.uniqueID, std::move(material));
        }
    }
#if 0
    // �_�~�[�̃}�e���A����}��
    Material material;
    materials.emplace(material.uniqueID, std::move(material));
#endif
}

// �o�C���h�|�[�Y���̎��o��
void Model::FetchSkeletons(FbxMesh* fbxMesh, Skeleton& bindPose)
{
    // ���b�V���ɂ���X�L���̐����擾
    const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    // ���b�V���ɂ��邷�ׂẴX�L���̏����`�F�b�N
    for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
    {
        // ���݂̃X�L�����擾
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
        // �X�L���ɂ���N���X�^�[�̐����擾
        const int clusterCount = skin->GetClusterCount();
        // ���b�V���̃{�[���̐� = ���b�V���̃N���X�^�[�̐�
        bindPose.bones.resize(clusterCount);
        // �X�L���ɂ��邷�ׂẴN���X�^�[�̏����`�F�b�N
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            // ���݂̃N���X�^�[�̏����`�F�b�N
            FbxCluster* cluster = skin->GetCluster(clusterIndex);

            Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
            bone.name = cluster->GetLink()->GetName();
            bone.uniqueID = cluster->GetLink()->GetUniqueID();
            bone.parentIndex = bindPose.IndexOf(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.nodeIndex = sceneView.IndexOf(bone.uniqueID);

            // �����N���܂ރm�[�h�Ɋ֘A�t����ꂽ�s����擾
            // ���f��(���b�V��)�̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg�p
            FbxAMatrix referenceGlobalInitPosition;
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            // �����N�m�[�h�Ɋ֘A�t����ꂽ�s����擾
            // �{�[���̃��[�J����Ԃ���V�[���̃O���[�o����Ԃɕϊ����邽�߂Ɏg�p
            // ���[�J�����W��Ԃł̃{�[���̐�Έʒu
            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            bone.offsetTransform = ToXmFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
        }
    }
}

// �A�j���[�V�������̎��o��
void Model::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
    float samplingRate)
{
    FbxArray<FbxString*> animationStackNames;
    // �V�[������A�j���[�V�����ꗗ���擾
    fbxScene->FillAnimStackNameArray(animationStackNames);
    // �A�j���[�V�����ꗗ�̐����擾
    const int animationStackCount{ animationStackNames.GetCount() };
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        // �A�j���[�V�������擾
        Animation& animationClip{ animationClips.emplace_back() };
        animationClip.name = animationStackNames[animationStackIndex]->Buffer();

        // �擾�����A�j���[�V�������A�j���[�V�����X�^�b�N�Ƃ��Đݒ�
        FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str()) };
        // ���݂̃A�j���[�V�������A�j���[�V�����X�^�b�N�̃A�j���[�V�����ɕύX
        fbxScene->SetCurrentAnimationStack(animationStack);

        const FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
        FbxTime oneSecond;
        oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
        animationClip.samplingRate = samplingRate > 0 ?
            samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));

        //�T���v�����O���[�g����T���v�����O�̊Ԋu���Ԃ�ݒ�
        const FbxTime samplingInterval
        { static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate) };
        const FbxTakeInfo* take_info{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };

        //�A�j���[�V�����̊J�n�ƏI�����擾
        const FbxTime startTime{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stopTime{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
        {
            //�T���v�����O�Ԋu���Ƃ̃L�[�t���[�����擾
            Animation::Keyframe& keyframe{ animationClip.sequence.emplace_back() };

            const size_t nodeCount{ sceneView.nodes.size() };
            keyframe.nodes.resize(nodeCount);
            for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };
                if (fbxNode)
                {
                    Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
                    //�A�j���[�V�������Ԃ���A�j���[�V�����s����擾
                    node.globalTransform = ToXmFloat4x4(fbxNode->EvaluateGlobalTransform(time));
                
                    // 'local_transform'�́A�e�̃��[�J�����W�n�Ɋւ���m�[�h�̕ϊ��s��ł�
                    const FbxAMatrix& localTransform{ fbxNode->EvaluateLocalTransform(time) };
                    node.scaling = ToXmFloat3(localTransform.GetS());
                    node.rotation = ToXmFloat4(localTransform.GetQ());
                    node.translation = ToXmFloat3(localTransform.GetT());
                }
            }
        }
    }
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        delete animationStackNames[animationStackIndex];
    }
}

// �o�b�t�@�̐���
void Model::CreateComObjects(ID3D11Device* device, const char* fbxFilename)
{
    // �e���b�V���ɑ΂��ăo�b�t�@�𐶐����郋�[�v
    for (Mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };

        // ���_�o�b�t�@�̐���
        D3D11_BUFFER_DESC bufferDesc{};
        D3D11_SUBRESOURCE_DATA subresourceData{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        subresourceData.pSysMem = mesh.vertices.data();
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh.vertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        // �C���f�b�N�X�o�b�t�@�̐���
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresourceData.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh.indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        // ���b�V���̒��_�ƃC���f�b�N�X���N���A����
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }
    // �}�e���A���̒��ɂ���e�N�X�`���t�@�C�����̐������e�N�X�`���𐶐�����
    for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
#if 0
        for (size_t texture_index = 0; texture_index < 2; ++texture_index)
        {
            if (iterator->second.textureFilenames[0].size() > 0)
            {
                std::filesystem::path path(fbxFilename);
                path.replace_filename(iterator->second.textureFilenames[texture_index]);
                D3D11_TEXTURE2D_DESC texture2d_desc;
                ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(),
                    iterator->second.shaderResourceViews[texture_index].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shaderResourceViews[texture_index].GetAddressOf(),
                    texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
            }
        }
#else
        D3D11_TEXTURE2D_DESC texture2dDesc;
        // Diffuse
        if (iterator->second.textureFilenames[0].size() > 0)
        {
            std::filesystem::path path(fbxFilename);
            path.replace_filename(iterator->second.textureFilenames[0]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[0].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shaderResourceViews[0].GetAddressOf(), 0xFFFFFFFF, 4);
        }
        // Normal
        if (iterator->second.textureFilenames[1].size() > 0)
        {
            std::filesystem::path path(fbxFilename);
            path.replace_filename(iterator->second.textureFilenames[1]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[1].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shaderResourceViews[1].GetAddressOf(), 0xFFFF7F7F, 4);
        }
        // Emissive
        if (iterator->second.textureFilenames[2].size() > 0)
        {
            std::filesystem::path path(fbxFilename);
            path.replace_filename(iterator->second.textureFilenames[2]);
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[2].GetAddressOf(), &texture2dDesc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shaderResourceViews[2].GetAddressOf(), 0xFF000000, 4);
        }
#endif
    }
    HRESULT hr = S_OK;
    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
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
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertexShader.ReleaseAndGetAddressOf(),
            inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixelShader.ReleaseAndGetAddressOf());
    }
    
    // �萔�o�b�t�@�̐���
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(Constants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
    currentAnimationIndex = index;
    currentAnimationSeconds = 0.0f;
    animationLoop = loop;
    animationEndFlag = false;
    // �u�����h�p�g�[�^�����Ԃ����Z�b�g
    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

//�A�j���[�V�����Đ�����
bool Model::IsPlayAnimation()const
{
    if (currentAnimationIndex < 0)return false;
    if (currentAnimationIndex >= animationClips.size())return false;
    return true;
}

Skeleton::Bone* Model::FindNode(const char* name)
{
    // ���ׂẴ��b�V�������[�v
    for (auto& mesh : meshes)
    {
        // �e���b�V���̃{�[�����X�g�����[�v
        for (auto& bone : mesh.bindPose.bones)
        {
            if (bone.name == name) // ���O����v���邩�m�F
            {
                return &bone; // �{�[���̃|�C���^��Ԃ�
            }
        }
    }
    return nullptr; // ������Ȃ������ꍇ
}

Model::Mesh* Model::FindMesh(const char* name)
{
    // ���ׂẴ��b�V�������[�v
    for (auto& mesh : meshes)
    {
        // �e���b�V���̃{�[�����X�g�����[�v
        for (auto& bone : mesh.bindPose.bones)
        {
            if (bone.name == name) // ���O����v���邩�m�F
            {
                return &mesh; // ���b�V���̃|�C���^��Ԃ�
            }
        }
    }
    return nullptr; // ������Ȃ������ꍇ
}