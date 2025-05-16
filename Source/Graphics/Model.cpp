#include"..\Misc.h"
#include"Model.h"
#include<sstream>
#include<functional>
#include<filesystem>
#include"Shader.h"
#include<fstream>

// FbxAMatrixからXMFLOAT４X4に変換
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

// FbxDouble3からXMFLOAT3に変換
inline DirectX::XMFLOAT3 ToXmFloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

// FbxDouble4からXMFLOAT4に変換
inline DirectX::XMFLOAT4 ToXmFloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

// １つの頂点が影響を受けるボーンの情報
struct BoneInfluence
{
    uint32_t boneIndex;// ボーン番号
    float boneWeight;  // ウェイト値
};
// １つの頂点は複数のボーンから影響を受ける場合があるので可変長配列で表現
using boneInfluencesPerControlPoint = std::vector<BoneInfluence>;

void FetchBoneInfluences(const FbxMesh* fbxMesh,
    std::vector<boneInfluencesPerControlPoint>& boneInfluences)
{
    // ボーン影響度の数 = FBX メッシュにあるコントロールポイントの数に設定
    const int controlPointsCount{ fbxMesh->GetControlPointsCount() };
    boneInfluences.resize(controlPointsCount);
    // メッシュにあるスキンの数を取得
    const int skinCount{ fbxMesh->GetDeformerCount(FbxDeformer::eSkin) };
    // メッシュにあるすべてのスキンの情報をチェック
    for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
    {
        // 現在のスキンを取得
        const FbxSkin* fbxSkin{ static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex,FbxDeformer::eSkin)) };

        // スキンにあるクラスターの数を取得
        const int clusterCount{ fbxSkin->GetClusterCount() };
        // スキンにあるすべてのクラスターの情報をチェック
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            // 現在のクラスターの情報をチェック
            const FbxCluster* fbxCluster{ fbxSkin->GetCluster(clusterIndex) };

            // クラスターにあるコントロールポイントの数を取得
            const int controlPointIndicesCount{ fbxCluster->GetControlPointIndicesCount() };
            // クラスターにあるすべてのコントロールポイントのウェイトの値を取得
            for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
            {
                // 現在のコントロールポイントの番号
                int controlPointIndex{ fbxCluster->GetControlPointIndices()[controlPointIndicesIndex] };
                // コントロールポイントのウェイト値
                double controlPointWeight{ fbxCluster->GetControlPointWeights()[controlPointIndicesIndex] };
                // 新規にボーン影響度を生成して追加
                BoneInfluence& boneInfluence{ boneInfluences.at(controlPointIndex).emplace_back() };
                // 現在のクラスターの番号をボーン番号として設定
                boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
                // コントロールのウェイト値をボーンのウェイト値として設定
                boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
            }
        }
    }
}

Model::Model(ID3D11Device* device, const char* fbxFilename, bool triangulate, float samplingRate)
{
    // 通常のファイル名の拡張子を .fbx から .cereal に変更する
    std::filesystem::path cerealFilename(fbxFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        // シリアライズされたファイルが存在する場合
        // シリアライズされているファイルを読み込んで
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        // シリアライズされたファイルが存在しない場合
        // 通常の FBX ファイル読み込み
        
        // fbxマネージャーを生成
        FbxManager* fbxMgr
        { FbxManager::Create() };
        // fbxシーンを生成
        fbxScene = { FbxScene::Create(fbxMgr,"") };

        // fbxインポーターの生成
        FbxImporter* fbxImporter{ FbxImporter::Create(fbxMgr,"") };
        bool importStatus{ false };

        // fbxファイルを読み込む
        importStatus = fbxImporter->Initialize(fbxFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        // 読み込んだファイルの情報を fbx シーンに流し込む
        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxMgr);
        if (triangulate)
        {
            // 多角形で作られたポリゴンをすべて三角形にする
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        // ノードの情報を解析する関数
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbxNode)
            {
                // scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
                ModelScene::Node& node{sceneView.nodes.emplace_back()};
                node.attribute = fbxNode->GetNodeAttribute() ?
                    fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node.name = fbxNode->GetName();
                node.uniqueID = fbxNode->GetUniqueID();
                node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
                    fbxNode->GetParent()->GetUniqueID() : 0);
                // 現在解析しているノードに取り付けられているの子ノードを解析していく
                for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
                {
                    traverse(fbxNode->GetChild(childIndex));
                }
            }
        };
        // ルートノードから解析開始
        traverse(fbxScene->GetRootNode());
        for (const ModelScene::Node& node : sceneView.nodes)
        {
            //ノード名でノードを検索し取得
            FbxNode* fbxNode={ fbxScene->FindNodeByName(node.name.c_str()) };
            //出力ウィンドウにデバッグデータを表示
        }

#if 0
        //シーンに取り付けたすべてのノードの情報を表示
        for (const ModelScene::Node& node : sceneView.nodes)
        {
            //ノード名でノードを検索し取得
            FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
            //出力ウィンドウにデバッグデータを表示
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

        // FBX ファイルを読み込んだら
        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // 外部ファイルにシリアライズ
        serialization(sceneView, meshes, materials, animationClips);
    }

    CreateComObjects(device, fbxFilename);
}

Model::Model(ID3D11Device* device, std::vector<std::string>& animationFilenames,
    const char* fbxFilename, bool triangulate, float samplingRate)
{
    // 通常のファイル名の拡張子を .fbx から .cereal に変更する
    std::filesystem::path cerealFilename(fbxFilename);
    cerealFilename.replace_extension("cereal");
    if (std::filesystem::exists(cerealFilename.c_str()))
    {
        // シリアライズされたファイルが存在する場合
        // シリアライズされているファイルを読み込んで
        std::ifstream ifs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(sceneView, meshes, materials, animationClips);
    }
    else
    {
        // シリアライズされたファイルが存在しない場合
        // 通常の FBX ファイル読み込み

        // fbxマネージャーを生成
        FbxManager* fbxManager{ FbxManager::Create() };
        // fbxシーンを生成
        FbxScene* fbxScene{ FbxScene::Create(fbxManager,"") };

        // fbxインポーターの生成
        FbxImporter* fbxImporter{ FbxImporter::Create(fbxManager,"") };
        bool importStatus{ false };

        // fbxファイルを読み込む
        importStatus = fbxImporter->Initialize(fbxFilename);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        // 読み込んだファイルの情報を fbx シーンに流し込む
        importStatus = fbxImporter->Import(fbxScene);
        _ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

        FbxGeometryConverter fbxConverter(fbxManager);
        if (triangulate)
        {
            // 多角形で作られたポリゴンをすべて三角形にする
            fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
            fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
        }

        // ノードの情報を解析する関数
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbxNode)
            {
                // scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
                ModelScene::Node& node={sceneView.nodes.emplace_back()};
                node.attribute = fbxNode->GetNodeAttribute() ?
                    fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node.name = fbxNode->GetName();
                node.uniqueID = fbxNode->GetUniqueID();
                node.parentIndex = sceneView.IndexOf(fbxNode->GetParent() ?
                    fbxNode->GetParent()->GetUniqueID() : 0);
                // 現在解析しているノードに取り付けられているの子ノードを解析していく
                for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
                {
                    traverse(fbxNode->GetChild(childIndex));
                }
            }
        };

#if 0
        //シーンに取り付けたすべてのノードの情報を表示
        for (const Scene::Node& node : sceneView.nodes)
        {
            //ノード名でノードを検索し取得
            FbxNode* fbx_node{ fbxScene->FindNodeByName(node.name.c_str()) };
            //出力ウィンドウにデバッグデータを表示
            std::string node_name = fbx_node->GetName();
            uint64_t uid = fbx_node->GetUniqueID();
            uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
            int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;

            std::stringstream debug_string;
            debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << '\n';
            OutputDebugStringA(debug_string.str().c_str());
        }
#endif
        // ルートノードから解析開始
        traverse(fbxScene->GetRootNode());

        FetchMeshes(fbxScene, meshes);

        FetchMaterials(fbxScene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbxScene, animationClips, samplingRate);

        fbxManager->Destroy();

        // 通常の FBX ファイルの読み込みの後にアニメーション FBX ファイルの読み込みを行うようにする
        for (const std::string animation_filename : animationFilenames)
        {
            AppendAnimations(animation_filename.c_str(), samplingRate);
        }

        // FBX ファイルを読み込んだら
        std::ofstream ofs(cerealFilename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // 外部ファイルにシリアライズ
        serialization(sceneView, meshes, materials, animationClips);
    }

    CreateComObjects(device, fbxFilename);
}

//描画処理
void Model::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& materialColor)
{
    //メッシュごとに描画処理を行う
    for (const Mesh& mesh : meshes)
    {

        // 頂点バッファの設定
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        // インデックスバッファの設定
        dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        // プリミティブのトポロジー設定（三角形リスト）
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // 入力レイアウトの設定
        dc->IASetInputLayout(inputLayout.Get());

        // 頂点シェーダーの設定
        dc->VSSetShader(vertexShader.Get(), nullptr, 0);
        // ピクセルシェーダーの設定
        dc->PSSetShader(pixelShader.Get(), nullptr, 0);

        // 定数バッファの更新（ワールド行列とマテリアルカラーの設定
        Constants data;
        if (&keyframe && keyframe.nodes.size() > 0)
        {
            // メッシュの位置・姿勢行列をキーフレームから取得
            const Animation::Keyframe::Node& meshNode{ keyframe.nodes.at(mesh.nodeIndex) };
            // 取得したキーフレームごとの位置・姿勢行列をワールド変換行列に合成する
            DirectX::XMStoreFloat4x4(&data.world,
                DirectX::XMLoadFloat4x4(&meshNode.globalTransform) * DirectX::XMLoadFloat4x4(&world));
#if 0
            DirectX::XMStoreFloat4x4(&data.boneTransforms[0], DirectX::XMMatrixIdentity());
            DirectX::XMStoreFloat4x4(&data.boneTransforms[1], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)));
            DirectX::XMStoreFloat4x4(&data.boneTransforms[2], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)));
#endif

#if 0
            DirectX::XMMATRIX B[3]; // バインドポーズ変換(オフセット行列)：モデル(メッシュ)空間からボーン空間に変換
            B[0] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(0).offsetTransform);
            B[1] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(1).offsetTransform);
            B[2] = DirectX::XMLoadFloat4x4(&mesh.bindPose.bones.at(2).offsetTransform);

            DirectX::XMMATRIX A[3]; // アニメーションボーン変換：ボーン空間からモデル(メッシュ)または親ボーン空間に変換
            A[0] = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0);	                                //A0 空間からモデル空間へ
            A[1] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A1スペースから親ボーン(A0)スペースへ
            A[2] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A2スペースから親ボーン(A1)スペースへ

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
            // マテリアルの識別ID からマテリアルを取得し参照として設定
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

// アニメーションの追加
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

    // 0...デフォルト値を使用します。0 未満...フェッチしません
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

// メッシュ情報の取り出し
void Model::FetchMeshes(FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
    // シーンの中にあるノードの情報をすべて検索
    for (const ModelScene::Node& node : sceneView.nodes)
    {
        // ノードの中からメッシュの属性を持っているノードをチェック
        if (node.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // シーンから FbxNode を取得
        FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };
        // メッシュ情報を取得
        FbxMesh* fbxMesh{ fbxNode->GetMesh() };

        // メッシュを新規で作成し取り付け
        Mesh& mesh{ meshes.emplace_back() };
        // メッシュの識別IDの設定
        mesh.uniqueID = fbxMesh->GetNode()->GetUniqueID();
        // メッシュ名の設定
        mesh.name = fbxMesh->GetNode()->GetName();
        // メッシュに対するノードIDの割り振り
        mesh.nodeIndex = sceneView.IndexOf(mesh.uniqueID);
        
        // メッシュのグローバル行列を取得しXMFLOAT4X4に変換して代入
        mesh.defaultGlobalTransform = ToXmFloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());

        // メッシュからボーン影響度を取得
        std::vector<boneInfluencesPerControlPoint> boneInfluences;
        FetchBoneInfluences(fbxMesh, boneInfluences);

        // メッシュからバインドポーズ(初期姿勢)の情報の取り出し
        FetchSkeletons(fbxMesh, mesh.bindPose);

        std::vector<Mesh::Subset>& subsets{ mesh.subsets };
        // マテリアル数を取得
        const int materialCount{ fbxMesh->GetNode()->GetMaterialCount() };
        // サブセットの数をリサイズ
        subsets.resize(materialCount > 0 ? materialCount : 1);
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            //マテリアル情報を取得
            const FbxSurfaceMaterial* fbxMaterial{ fbxMesh->GetNode()->GetMaterial(materialIndex) };
            subsets.at(materialIndex).materialName = fbxMaterial->GetName();
            subsets.at(materialIndex).materialUniqueID = fbxMaterial->GetUniqueID();
        }

        if (materialCount > 0)
        {
            const int polygonCount{ fbxMesh->GetPolygonCount() };
            for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
            {
                // メッシュのポリゴンの番号からインデックス番号を取得しマテリアルのインデックスの番号として割り当てる
                const int materialIndex{ fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) };
                // サブセットに対して割り当てられたマテリアルのインデックスの番号にインデックスの数を頂点数分(3)増やしていく
                subsets.at(materialIndex).indexCount += 3;
            }
            // インデックスの数からそれぞれの開始の数を計算して設定していく
            uint32_t offset{ 0 };
            for (Mesh::Subset& subset : subsets)
            {
                subset.startIndexLocation = offset;
                offset += subset.indexCount;
                subset.indexCount = 0;
            }
        }

        const int polygonCount{ fbxMesh->GetPolygonCount() };         //ポリゴン数
        mesh.vertices.resize(polygonCount * 3LL);                     //頂点座標数
        mesh.indices.resize(polygonCount * 3LL);                      //頂点インデックス数

        // uv名の取得。後々テクスチャ座標の取得に利用。
        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);

        // コントロールポイントの取得
        const FbxVector4* controlPoints{ fbxMesh->GetControlPoints() };
        // ポリゴンの数だけ頂点データを取得
        for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
        {
            const int materialIndex{ materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0 };
            Mesh::Subset& subset{ subsets.at(materialIndex) };
            const uint32_t offset{ subset.startIndexLocation + subset.indexCount };
            // 三角形の数分の頂点の情報を取得する
            for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
            {
                // 利用するインデックスの配列を計算
                const int vertexIndex{ polygonIndex * 3 + positionInPolygon };

                // 頂点座標の取得
                vertex vertex;
                const int polygonVertex{ fbxMesh->GetPolygonVertex(polygonIndex,positionInPolygon) };
                vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);
                vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
                vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

                // 頂点数番目のボーン影響度を取得
                const boneInfluencesPerControlPoint& influcesPerControlPoint{ boneInfluences.at(polygonVertex) };
                for (size_t influenceIndex = 0; influenceIndex < influcesPerControlPoint.size(); ++influenceIndex)
                {
                    // ボーン影響度は最大４つ
                    if (influenceIndex < maxBoneInfluences)
                    {
                        // 取得したボーン影響度を頂点側のボーン影響度に設定していく
                        vertex.boneWeights[influenceIndex] =
                            influcesPerControlPoint.at(influenceIndex).boneWeight;
                        vertex.boneIndices[influenceIndex] =
                            influcesPerControlPoint.at(influenceIndex).boneIndex;
                    }
                    else
                    {
                        // 最小の影響度を持つボーンを探す
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

                        // 最小の影響度を持つボーンの影響度を加算し、インデックスを更新する
                        vertex.boneWeights[minimumValueIndex] += influcesPerControlPoint[influenceIndex].boneWeight;
                        vertex.boneIndices[minimumValueIndex] = influcesPerControlPoint[influenceIndex].boneIndex;

                        // 合計の影響度を計算し、全ての影響度を正規化する
                        float totalWeight = 0.0f;
                        for (size_t i = 0; i < maxBoneInfluences; ++i)
                        {
                            totalWeight += vertex.boneWeights[i];
                        }

                        // 影響度を正規化する
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

                // 法線の取得
                if (fbxMesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
                    vertex.normal.x = static_cast<float>(normal[0]);
                    vertex.normal.y = static_cast<float>(normal[1]);
                    vertex.normal.z = static_cast<float>(normal[2]);
                }

                // テクスチャ座標の取得
                if (fbxMesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmappedUV;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon,
                        uvNames[0], uv, unmappedUV);
                    vertex.texcoord.x = static_cast<float>(uv[0]);
                    vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                // 接線ベクトルの取得
                if (fbxMesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
                    vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
                    vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
                    vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
                    vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
                }


                // 現在のインデックス番号部分に頂点データを設定
                mesh.vertices.at(vertexIndex) = std::move(vertex);
                // 現在のインデックス番号を設定
                mesh.indices.at(static_cast<size_t>(offset)+positionInPolygon) = vertexIndex;
                subset.indexCount++;
            }
        }
        // メッシュごとのバウンディングボックスの座標を計算
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

// マテリアル情報の取り出し
void Model::FetchMaterials(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
    // シーンの中にあるノードの情報をすべて検索
    const size_t nodeCount{ sceneView.nodes.size() };
    for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        // ノードの情報１つ１つをチェック
        const ModelScene::Node& node{ sceneView.nodes.at(nodeIndex) };
        // シーンからFbxNodeを取得
        const FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

        // ノードの中にあるマテリアルの情報をすべて検索
        const int materialCount{ fbxNode->GetMaterialCount() };
        for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
        {
            // マテリアルの情報１つ１つをチェック
            const FbxSurfaceMaterial* fbxMaterial{ fbxNode->GetMaterial(materialIndex) };

            // マテリアルの設定
            Material material;
            material.name = fbxMaterial->GetName();
            material.uniqueID = fbxMaterial->GetUniqueID();

            FbxProperty fbxProperty;
            // ディフューズ(拡散反射光)の情報を取得
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // ディフューズがあったらディフューズの情報を設定していく
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Kd.x = static_cast<float>(color[0]);
                material.Kd.y = static_cast<float>(color[1]);
                material.Kd.z = static_cast<float>(color[2]);
                material.Kd.w = 1.0f;

                // テクスチャのファイル名を取得
                const FbxFileTexture* fbxTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                // 相対パス込みでのファイル名を設定する
                material.textureFilenames[0] =
                    fbxTexture ? fbxTexture->GetRelativeFileName() : "";
            }
            // ンビエント(環境光)の情報を取得
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ka.x = static_cast<float>(color[0]);
                material.Ka.y = static_cast<float>(color[1]);
                material.Ka.z = static_cast<float>(color[2]);
                material.Ka.w = 1.0f;
            }
            // スペキュラ(鏡面反射)の情報を取得
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbxProperty.IsValid())
            {
                const FbxDouble3 color{ fbxProperty.Get<FbxDouble3>() };
                material.Ks.x = static_cast<float>(color[0]);
                material.Ks.y = static_cast<float>(color[1]);
                material.Ks.z = static_cast<float>(color[2]);
                material.Ks.w = 1.0f;
            }

            // 法線マップのファイル名
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                // ファイル名が存在したら相対パス込みでのファイル名を取得
                material.textureFilenames[1] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }
            // EMISSIVE
            fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbxProperty.IsValid())
            {
                const FbxFileTexture* fileTexture{ fbxProperty.GetSrcObject<FbxFileTexture>() };
                material.textureFilenames[2] = fileTexture ? fileTexture->GetRelativeFileName() : "";
            }
            // 取得したマテリアルの情報を設定する
            materials.emplace(material.uniqueID, std::move(material));
        }
    }
#if 0
    // ダミーのマテリアルを挿入
    Material material;
    materials.emplace(material.uniqueID, std::move(material));
#endif
}

// バインドポーズ情報の取り出し
void Model::FetchSkeletons(FbxMesh* fbxMesh, Skeleton& bindPose)
{
    // メッシュにあるスキンの数を取得
    const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    // メッシュにあるすべてのスキンの情報をチェック
    for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
    {
        // 現在のスキンを取得
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
        // スキンにあるクラスターの数を取得
        const int clusterCount = skin->GetClusterCount();
        // メッシュのボーンの数 = メッシュのクラスターの数
        bindPose.bones.resize(clusterCount);
        // スキンにあるすべてのクラスターの情報をチェック
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {
            // 現在のクラスターの情報をチェック
            FbxCluster* cluster = skin->GetCluster(clusterIndex);

            Skeleton::Bone& bone{ bindPose.bones.at(clusterIndex) };
            bone.name = cluster->GetLink()->GetName();
            bone.uniqueID = cluster->GetLink()->GetUniqueID();
            bone.parentIndex = bindPose.IndexOf(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.nodeIndex = sceneView.IndexOf(bone.uniqueID);

            // リンクを含むノードに関連付けられた行列を取得
            // モデル(メッシュ)のローカル空間からシーンのグローバル空間に変換するために使用
            FbxAMatrix referenceGlobalInitPosition;
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            // リンクノードに関連付けられた行列を取得
            // ボーンのローカル空間からシーンのグローバル空間に変換するために使用
            // ローカル座標空間でのボーンの絶対位置
            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            bone.offsetTransform = ToXmFloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);
        }
    }
}

// アニメーション情報の取り出し
void Model::FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
    float samplingRate)
{
    FbxArray<FbxString*> animationStackNames;
    // シーンからアニメーション一覧を取得
    fbxScene->FillAnimStackNameArray(animationStackNames);
    // アニメーション一覧の数を取得
    const int animationStackCount{ animationStackNames.GetCount() };
    for (int animationStackIndex = 0; animationStackIndex < animationStackCount; ++animationStackIndex)
    {
        // アニメーションを取得
        Animation& animationClip{ animationClips.emplace_back() };
        animationClip.name = animationStackNames[animationStackIndex]->Buffer();

        // 取得したアニメーションをアニメーションスタックとして設定
        FbxAnimStack* animationStack{ fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str()) };
        // 現在のアニメーションをアニメーションスタックのアニメーションに変更
        fbxScene->SetCurrentAnimationStack(animationStack);

        const FbxTime::EMode timeMode{ fbxScene->GetGlobalSettings().GetTimeMode() };
        FbxTime oneSecond;
        oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
        animationClip.samplingRate = samplingRate > 0 ?
            samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));

        //サンプリングレートからサンプリングの間隔時間を設定
        const FbxTime samplingInterval
        { static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate) };
        const FbxTakeInfo* take_info{ fbxScene->GetTakeInfo(animationClip.name.c_str()) };

        //アニメーションの開始と終了を取得
        const FbxTime startTime{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stopTime{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
        {
            //サンプリング間隔ごとのキーフレームを取得
            Animation::Keyframe& keyframe{ animationClip.sequence.emplace_back() };

            const size_t nodeCount{ sceneView.nodes.size() };
            keyframe.nodes.resize(nodeCount);
            for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                FbxNode* fbxNode{ fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str()) };
                if (fbxNode)
                {
                    Animation::Keyframe::Node& node{ keyframe.nodes.at(nodeIndex) };
                    //アニメーション時間からアニメーション行列を取得
                    node.globalTransform = ToXmFloat4x4(fbxNode->EvaluateGlobalTransform(time));
                
                    // 'local_transform'は、親のローカル座標系に関するノードの変換行列です
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

// バッファの生成
void Model::CreateComObjects(ID3D11Device* device, const char* fbxFilename)
{
    // 各メッシュに対してバッファを生成するループ
    for (Mesh& mesh : meshes)
    {
        HRESULT hr{ S_OK };

        // 頂点バッファの生成
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

        // インデックスバッファの生成
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresourceData.pSysMem = mesh.indices.data();
        hr = device->CreateBuffer(&bufferDesc, &subresourceData,
            mesh.indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        // メッシュの頂点とインデックスをクリアする
        mesh.vertices.clear();
        mesh.indices.clear();
#endif
    }
    // マテリアルの中にあるテクスチャファイル名の数だけテクスチャを生成する
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
    // 入力レイアウトオブジェクトの生成
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

    // 頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertexShader.ReleaseAndGetAddressOf(),
            inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixelShader.ReleaseAndGetAddressOf());
    }
    
    // 定数バッファの生成
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
    // ブレンド用トータル時間をリセット
    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

//アニメーション再生中か
bool Model::IsPlayAnimation()const
{
    if (currentAnimationIndex < 0)return false;
    if (currentAnimationIndex >= animationClips.size())return false;
    return true;
}

Skeleton::Bone* Model::FindNode(const char* name)
{
    // すべてのメッシュをループ
    for (auto& mesh : meshes)
    {
        // 各メッシュのボーンリストをループ
        for (auto& bone : mesh.bindPose.bones)
        {
            if (bone.name == name) // 名前が一致するか確認
            {
                return &bone; // ボーンのポインタを返す
            }
        }
    }
    return nullptr; // 見つからなかった場合
}

Model::Mesh* Model::FindMesh(const char* name)
{
    // すべてのメッシュをループ
    for (auto& mesh : meshes)
    {
        // 各メッシュのボーンリストをループ
        for (auto& bone : mesh.bindPose.bones)
        {
            if (bone.name == name) // 名前が一致するか確認
            {
                return &mesh; // メッシュのポインタを返す
            }
        }
    }
    return nullptr; // 見つからなかった場合
}