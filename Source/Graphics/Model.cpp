#include"..\misc.h"
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
struct bone_influence
{
    uint32_t bone_index;// ボーン番号
    float bone_weight;  // ウェイト値
};
// １つの頂点は複数のボーンから影響を受ける場合があるので可変長配列で表現
using bone_influences_per_control_point = std::vector<bone_influence>;

void FetchBoneInfluences(const FbxMesh* fbx_mesh,
    std::vector<bone_influences_per_control_point>& bone_influences)
{
    // ボーン影響度の数 = FBX メッシュにあるコントロールポイントの数に設定
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };
    bone_influences.resize(control_points_count);
    // メッシュにあるスキンの数を取得
    const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    // メッシュにあるすべてのスキンの情報をチェック
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
    {
        // 現在のスキンを取得
        const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index,FbxDeformer::eSkin)) };

        // スキンにあるクラスターの数を取得
        const int cluster_count{ fbx_skin->GetClusterCount() };
        // スキンにあるすべてのクラスターの情報をチェック
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // 現在のクラスターの情報をチェック
            const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

            // クラスターにあるコントロールポイントの数を取得
            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            // クラスターにあるすべてのコントロールポイントのウェイトの値を取得
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
            {
                // 現在のコントロールポイントの番号
                int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                // コントロールポイントのウェイト値
                double control_point_weight{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                // 新規にボーン影響度を生成して追加
                bone_influence& bone_influence_{ bone_influences.at(control_point_index).emplace_back() };
                // 現在のクラスターの番号をボーン番号として設定
                bone_influence_.bone_index = static_cast<uint32_t>(cluster_index);
                // コントロールのウェイト値をボーンのウェイト値として設定
                bone_influence_.bone_weight = static_cast<float>(control_point_weight);
            }
        }
    }
}

Model::Model(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
{
    // 通常のファイル名の拡張子を .fbx から .cereal に変更する
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        // シリアライズされたファイルが存在する場合
        // シリアライズされているファイルを読み込んで
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        // シリアライズされたファイルが存在しない場合
        // 通常の FBX ファイル読み込み
        
        // fbxマネージャーを生成
        FbxManager* fbx_manager{ FbxManager::Create() };
        // fbxシーンを生成
        fbx_scene = { FbxScene::Create(fbx_manager,"") };

        // fbxインポーターの生成
        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
        bool import_status{ false };

        // fbxファイルを読み込む
        import_status = fbx_importer->Initialize(fbx_filename);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        // 読み込んだファイルの情報を fbx シーンに流し込む
        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (triangulate)
        {
            // 多角形で作られたポリゴンをすべて三角形にする
            fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }

        // ノードの情報を解析する関数
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbx_node)
            {
                // scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
                scene::node& node_{scene_view.nodes.emplace_back()};
                node_.attribute = fbx_node->GetNodeAttribute() ?
                    fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node_.name = fbx_node->GetName();
                node_.unique_id = fbx_node->GetUniqueID();
                node_.parent_index = scene_view.IndexOf(fbx_node->GetParent() ?
                    fbx_node->GetParent()->GetUniqueID() : 0);
                // 現在解析しているノードに取り付けられているの子ノードを解析していく
                for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
                {
                    traverse(fbx_node->GetChild(child_index));
                }
            }
        };
        // ルートノードから解析開始
        traverse(fbx_scene->GetRootNode());

#if 0
        //シーンに取り付けたすべてのノードの情報を表示
        for (const scene::node& node : scene_view.nodes)
        {
            //ノード名でノードを検索し取得
            FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
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
        traverse(fbx_scene->GetRootNode());

        FetchMeshes(fbx_scene, meshes);

        FetchMaterials(fbx_scene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbx_scene, animation_clips, sampling_rate);

        fbx_manager->Destroy();

        // FBX ファイルを読み込んだら
        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // 外部ファイルにシリアライズ
        serialization(scene_view, meshes, materials, animation_clips);
    }

    CreateComObjects(device, fbx_filename);
}

Model::Model(ID3D11Device* device, std::vector<std::string>& animation_filenames,
    const char* fbx_filename, bool triangulate, float sampling_rate)
{
    // 通常のファイル名の拡張子を .fbx から .cereal に変更する
    std::filesystem::path cereal_filename(fbx_filename);
    cereal_filename.replace_extension("cereal");
    if (std::filesystem::exists(cereal_filename.c_str()))
    {
        // シリアライズされたファイルが存在する場合
        // シリアライズされているファイルを読み込んで
        std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryInputArchive deserialization(ifs);
        deserialization(scene_view, meshes, materials, animation_clips);
    }
    else
    {
        // シリアライズされたファイルが存在しない場合
        // 通常の FBX ファイル読み込み

        // fbxマネージャーを生成
        FbxManager* fbx_manager{ FbxManager::Create() };
        // fbxシーンを生成
        FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

        // fbxインポーターの生成
        FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
        bool import_status{ false };

        // fbxファイルを読み込む
        import_status = fbx_importer->Initialize(fbx_filename);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        // 読み込んだファイルの情報を fbx シーンに流し込む
        import_status = fbx_importer->Import(fbx_scene);
        _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

        FbxGeometryConverter fbx_converter(fbx_manager);
        if (triangulate)
        {
            // 多角形で作られたポリゴンをすべて三角形にする
            fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
            fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
        }

        // ノードの情報を解析する関数
        std::function<void(FbxNode*)> traverse
        {
            [&](FbxNode* fbx_node)
            {
                // scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
                scene::node& node_{scene_view.nodes.emplace_back()};
                node_.attribute = fbx_node->GetNodeAttribute() ?
                    fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
                node_.name = fbx_node->GetName();
                node_.unique_id = fbx_node->GetUniqueID();
                node_.parent_index = scene_view.IndexOf(fbx_node->GetParent() ?
                    fbx_node->GetParent()->GetUniqueID() : 0);
                // 現在解析しているノードに取り付けられているの子ノードを解析していく
                for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
                {
                    traverse(fbx_node->GetChild(child_index));
                }
            }
        };
        // ルートノードから解析開始
        traverse(fbx_scene->GetRootNode());

#if 0
        //シーンに取り付けたすべてのノードの情報を表示
        for (const scene::node& node : scene_view.nodes)
        {
            //ノード名でノードを検索し取得
            FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
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
        traverse(fbx_scene->GetRootNode());

        FetchMeshes(fbx_scene, meshes);

        FetchMaterials(fbx_scene, materials);

        //float sampling_rate{ 0 };
        FetchAnimations(fbx_scene, animation_clips, sampling_rate);

        fbx_manager->Destroy();

        // 通常の FBX ファイルの読み込みの後にアニメーション FBX ファイルの読み込みを行うようにする
        for (const std::string animation_filename : animation_filenames)
        {
            AppendAnimations(animation_filename.c_str(), sampling_rate);
        }

        // FBX ファイルを読み込んだら
        std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
        cereal::BinaryOutputArchive serialization(ofs);
        // 外部ファイルにシリアライズ
        serialization(scene_view, meshes, materials, animation_clips);
    }

    CreateComObjects(device, fbx_filename);
}

//描画処理
void Model::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
{
    //メッシュごとに描画処理を行う
    for (const mesh& mesh_ : meshes)
    {

        // 頂点バッファの設定
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
        // インデックスバッファの設定
        immediate_context->IASetIndexBuffer(mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        // プリミティブのトポロジー設定（三角形リスト）
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // 入力レイアウトの設定
        immediate_context->IASetInputLayout(input_layout.Get());

        // 頂点シェーダーの設定
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        // ピクセルシェーダーの設定
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        // 定数バッファの更新（ワールド行列とマテリアルカラーの設定
        constants data;
        if (&keyframe && keyframe.nodes.size() > 0)
        {
            // メッシュの位置・姿勢行列をキーフレームから取得
            const animation::keyframe::node& mesh_node{ keyframe.nodes.at(mesh_.node_index) };
            // 取得したキーフレームごとの位置・姿勢行列をワールド変換行列に合成する
            DirectX::XMStoreFloat4x4(&data.world,
                DirectX::XMLoadFloat4x4(&mesh_node.global_transform) * DirectX::XMLoadFloat4x4(&world));
#if 0
            DirectX::XMStoreFloat4x4(&data.bone_transforms[0], DirectX::XMMatrixIdentity());
            DirectX::XMStoreFloat4x4(&data.bone_transforms[1], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)));
            DirectX::XMStoreFloat4x4(&data.bone_transforms[2], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)));
#endif

#if 0
            DirectX::XMMATRIX B[3]; // バインドポーズ変換(オフセット行列)：モデル(メッシュ)空間からボーン空間に変換
            B[0] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(0).offset_transform);
            B[1] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(1).offset_transform);
            B[2] = DirectX::XMLoadFloat4x4(&mesh_.bind_pose.bones.at(2).offset_transform);

            DirectX::XMMATRIX A[3]; // アニメーションボーン変換：ボーン空間からモデル(メッシュ)または親ボーン空間に変換
            A[0] = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90), 0, 0);	                                //A0 空間からモデル空間へ
            A[1] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A1スペースから親ボーン(A0)スペースへ
            A[2] = DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)) * DirectX::XMMatrixTranslation(0, 2, 0);	//A2スペースから親ボーン(A1)スペースへ

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
            // マテリアルの識別ID からマテリアルを取得し参照として設定
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
            // インデックスを使用して描画
            //D3D11_BUFFER_DESC buffer_desc;
            //mesh_.index_buffer->GetDesc(&buffer_desc);
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}

// アニメーションの更新
void Model::UpdateAnimation(float elapsedTime)
{
    //再生中でないなら処理しない
    if (!IsPlayAnimation())return;

    // ブレンド補間にかかる時間（0.5f）を 0.0 から 1.0 に変更
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

    if (blendRate < 1.0f)
    {
        // ブレンド再生

        animation::keyframe outkeyframe;
        const animation::keyframe* keyframes[2]{
            &keyframe,
            // 今回アニメーションの最初のフレームを最後として補完
            &animation_clips.at(currentAnimationIndex).sequence.at(0),
        };

        // ブレンド補間
        BlendAnimations(keyframes, blendRate, outkeyframe);
        // キーフレームに存在するすべてのノードを更新する
        size_t node_count{ keyframe.nodes.size() };
        keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ローカル行列を設定
            animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // 親のグローバル行列を取得
            int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

            // ローカル行列 * 親のグローバル行列
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }
    else
    {
        // 通常再生

        // アニメーションの番号
        int clip_index{ currentAnimationIndex };

        int frame_index{ 0 };
        static float animation_tick{ 0 };
        animation& animation{ animation_clips.at(clip_index) };
        frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
        if (frame_index > animation.sequence.size() - 1)
        {
            frame_index = 0;
            animation_tick = 0;
        }
        else
        {
            animation_tick += elapsedTime;
        }
        keyframe = animation.sequence.at(frame_index);

        // キーフレームに存在するすべてのノードを更新する
        size_t node_count{ keyframe.nodes.size() };
        keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ローカル行列を設定
            animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // 親のグローバル行列を取得
            int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

            // ローカル行列 * 親のグローバル行列
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

    //最終フレーム処理
    if (animationEndFlag)
    {
        animationEndFlag = false;
        currentAnimationIndex = -1;
        return;
    }
    //時間経過
    currentAnimationSeconds += elapsedTime;

    // アニメーション全体の長さを計算
    float totalAnimationTime = 
        animation_clips.at(currentAnimationIndex).sequence.size() *
        (1.0f / animation_clips.at(currentAnimationIndex).sampling_rate);
    //再生時間が終端時間を超えたら
    if (currentAnimationSeconds >= totalAnimationTime)
    {
        //再生時間を巻き戻す
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

// アニメーションの追加
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

    // 0...デフォルト値を使用します。0 未満...フェッチしません
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

// メッシュ情報の取り出し
void Model::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    // シーンの中にあるノードの情報をすべて検索
    for (const scene::node& node_ : scene_view.nodes)
    {
        // ノードの中からメッシュの属性を持っているノードをチェック
        if (node_.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        // シーンから FbxNode を取得
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node_.name.c_str()) };
        // メッシュ情報を取得
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        // メッシュを新規で作成し取り付け
        mesh& mesh_{ meshes.emplace_back() };
        // メッシュの識別IDの設定
        mesh_.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        // メッシュ名の設定
        mesh_.name = fbx_mesh->GetNode()->GetName();
        // メッシュに対するノードIDの割り振り
        mesh_.node_index = scene_view.IndexOf(mesh_.unique_id);
        
        // メッシュのグローバル行列を取得しXMFLOAT4X4に変換して代入
        mesh_.default_global_transform = ToXmFloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

        // メッシュからボーン影響度を取得
        std::vector<bone_influences_per_control_point> bone_influences;
        FetchBoneInfluences(fbx_mesh, bone_influences);

        // メッシュからバインドポーズ(初期姿勢)の情報の取り出し
        FetchSkeletons(fbx_mesh, mesh_.bind_pose);

        std::vector<mesh::subset>& subsets{ mesh_.subsets };
        // マテリアル数を取得
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        // サブセットの数をリサイズ
        subsets.resize(material_count > 0 ? material_count : 1);
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            //マテリアル情報を取得
            const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
            subsets.at(material_index).material_name = fbx_material->GetName();
            subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
        }

        if (material_count > 0)
        {
            const int polygon_count{ fbx_mesh->GetPolygonCount() };
            for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
            {
                // メッシュのポリゴンの番号からインデックス番号を取得しマテリアルのインデックスの番号として割り当てる
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                // サブセットに対して割り当てられたマテリアルのインデックスの番号にインデックスの数を頂点数分(3)増やしていく
                subsets.at(material_index).index_count += 3;
            }
            // インデックスの数からそれぞれの開始の数を計算して設定していく
            uint32_t offset{ 0 };
            for (mesh::subset& subset : subsets)
            {
                subset.start_index_location = offset;
                offset += subset.index_count;
                subset.index_count = 0;
            }
        }

        const int polygon_count{ fbx_mesh->GetPolygonCount() };         //ポリゴン数
        mesh_.vertices.resize(polygon_count * 3LL);                     //頂点座標数
        mesh_.indices.resize(polygon_count * 3LL);                      //頂点インデックス数

        // uv名の取得。後々テクスチャ座標の取得に利用。
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        // コントロールポイントの取得
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        // ポリゴンの数だけ頂点データを取得
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };
            // 三角形の数分の頂点の情報を取得する
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                // 利用するインデックスの配列を計算
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                // 頂点座標の取得
                vertex vertex_;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index,position_in_polygon) };
                vertex_.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex_.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex_.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                // 頂点数番目のボーン影響度を取得
                const bone_influences_per_control_point& influces_per_control_point{ bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influces_per_control_point.size(); ++influence_index)
                {
                    // ボーン影響度は最大４つ
                    if (influence_index < MAX_BONE_INFLUENCES)
                    {
                        // 取得したボーン影響度を頂点側のボーン影響度に設定していく
                        vertex_.bone_weights[influence_index] =
                            influces_per_control_point.at(influence_index).bone_weight;
                        vertex_.bone_indices[influence_index] =
                            influces_per_control_point.at(influence_index).bone_index;
                    }
                    else
                    {
                        // 最小の影響度を持つボーンを探す
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

                        // 最小の影響度を持つボーンの影響度を加算し、インデックスを更新する
                        vertex_.bone_weights[minimum_value_index] += influces_per_control_point[influence_index].bone_weight;
                        vertex_.bone_indices[minimum_value_index] = influces_per_control_point[influence_index].bone_index;

                        // 合計の影響度を計算し、全ての影響度を正規化する
                        float total_weight = 0.0f;
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            total_weight += vertex_.bone_weights[i];
                        }

                        // 影響度を正規化する
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

                // 法線の取得
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex_.normal.x = static_cast<float>(normal[0]);
                    vertex_.normal.y = static_cast<float>(normal[1]);
                    vertex_.normal.z = static_cast<float>(normal[2]);
                }

                // テクスチャ座標の取得
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex_.texcoord.x = static_cast<float>(uv[0]);
                    vertex_.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                // 接線ベクトルの取得
                if (fbx_mesh->GenerateTangentsData(0, false))
                {
                    const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
                    vertex_.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
                    vertex_.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
                    vertex_.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
                    vertex_.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
                }


                // 現在のインデックス番号部分に頂点データを設定
                mesh_.vertices.at(vertex_index) = std::move(vertex_);
                // 現在のインデックス番号を設定
                mesh_.indices.at(static_cast<size_t>(offset)+position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
        // メッシュごとのバウンディングボックスの座標を計算
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

// マテリアル情報の取り出し
void Model::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
    // シーンの中にあるノードの情報をすべて検索
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        // ノードの情報１つ１つをチェック
        const scene::node& node{ scene_view.nodes.at(node_index) };
        // シーンからFbxNodeを取得
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        // ノードの中にあるマテリアルの情報をすべて検索
        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            // マテリアルの情報１つ１つをチェック
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            // マテリアルの設定
            material material_;
            material_.name = fbx_material->GetName();
            material_.unique_id = fbx_material->GetUniqueID();

            FbxProperty fbx_property;
            // ディフューズ(拡散反射光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            // ディフューズがあったらディフューズの情報を設定していく
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Kd.x = static_cast<float>(color[0]);
                material_.Kd.y = static_cast<float>(color[1]);
                material_.Kd.z = static_cast<float>(color[2]);
                material_.Kd.w = 1.0f;

                // テクスチャのファイル名を取得
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // 相対パス込みでのファイル名を設定する
                material_.texture_filenames[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            // ンビエント(環境光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ka.x = static_cast<float>(color[0]);
                material_.Ka.y = static_cast<float>(color[1]);
                material_.Ka.z = static_cast<float>(color[2]);
                material_.Ka.w = 1.0f;
            }
            // スペキュラ(鏡面反射)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ks.x = static_cast<float>(color[0]);
                material_.Ks.y = static_cast<float>(color[1]);
                material_.Ks.z = static_cast<float>(color[2]);
                material_.Ks.w = 1.0f;
            }

            // 法線マップのファイル名
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                // ファイル名が存在したら相対パス込みでのファイル名を取得
                material_.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // EMISSIVE
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
            if (fbx_property.IsValid())
            {
                const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                material_.texture_filenames[2] = file_texture ? file_texture->GetRelativeFileName() : "";
            }
            // 取得したマテリアルの情報を設定する
            materials.emplace(material_.unique_id, std::move(material_));
        }
    }
#if 0
    // ダミーのマテリアルを挿入
    material material_;
    materials.emplace(material_.unique_id, std::move(material_));
#endif
}

// バインドポーズ情報の取り出し
void Model::FetchSkeletons(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
    // メッシュにあるスキンの数を取得
    const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
    // メッシュにあるすべてのスキンの情報をチェック
    for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
    {
        // 現在のスキンを取得
        FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
        // スキンにあるクラスターの数を取得
        const int cluster_count = skin->GetClusterCount();
        // メッシュのボーンの数 = メッシュのクラスターの数
        bind_pose.bones.resize(cluster_count);
        // スキンにあるすべてのクラスターの情報をチェック
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            // 現在のクラスターの情報をチェック
            FbxCluster* cluster = skin->GetCluster(cluster_index);

            skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
            bone.name = cluster->GetLink()->GetName();
            bone.unique_id = cluster->GetLink()->GetUniqueID();
            bone.parent_index = bind_pose.IndexOf(cluster->GetLink()->GetParent()->GetUniqueID());
            bone.node_index = scene_view.IndexOf(bone.unique_id);

            // リンクを含むノードに関連付けられた行列を取得
            // モデル(メッシュ)のローカル空間からシーンのグローバル空間に変換するために使用
            FbxAMatrix reference_global_init_position;
            cluster->GetTransformMatrix(reference_global_init_position);

            // リンクノードに関連付けられた行列を取得
            // ボーンのローカル空間からシーンのグローバル空間に変換するために使用
            // ローカル座標空間でのボーンの絶対位置
            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);

            bone.offset_transform = ToXmFloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);
        }
    }
}

// アニメーション情報の取り出し
void Model::FetchAnimations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
    float sampling_rate)
{
    FbxArray<FbxString*> animation_stack_names;
    // シーンからアニメーション一覧を取得
    fbx_scene->FillAnimStackNameArray(animation_stack_names);
    // アニメーション一覧の数を取得
    const int animation_stack_count{ animation_stack_names.GetCount() };
    for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
    {
        // アニメーションを取得
        animation& animation_clip{ animation_clips.emplace_back() };
        animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

        // 取得したアニメーションをアニメーションスタックとして設定
        FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
        // 現在のアニメーションをアニメーションスタックのアニメーションに変更
        fbx_scene->SetCurrentAnimationStack(animation_stack);

        const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
        FbxTime one_second;
        one_second.SetTime(0, 0, 1, 0, 0, time_mode);
        animation_clip.sampling_rate = sampling_rate > 0 ?
            sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));

        //サンプリングレートからサンプリングの間隔時間を設定
        const FbxTime sampling_interval
        { static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };
        const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };

        //アニメーションの開始と終了を取得
        const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };
        const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
        for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
        {
            //サンプリング間隔ごとのキーフレームを取得
            animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

            const size_t node_count{ scene_view.nodes.size() };
            keyframe.nodes.resize(node_count);
            for (size_t node_index = 0; node_index < node_count; ++node_index)
            {
                FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
                if (fbx_node)
                {
                    animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
                    //アニメーション時間からアニメーション行列を取得
                    node.global_transform = ToXmFloat4x4(fbx_node->EvaluateGlobalTransform(time));
                
                    // 'local_transform'は、親のローカル座標系に関するノードの変換行列です
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

// バッファの生成
void Model::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    // 各メッシュに対してバッファを生成するループ
    for (mesh& mesh_ : meshes)
    {
        HRESULT hr{ S_OK };

        // 頂点バッファの生成
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

        // インデックスバッファの生成
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh_.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh_.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh_.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        // メッシュの頂点とインデックスをクリアする
        mesh_.vertices.clear();
        mesh_.indices.clear();
#endif
    }
    // マテリアルの中にあるテクスチャファイル名の数だけテクスチャを生成する
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
        // EMISSIVE
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
    // 入力レイアウトオブジェクトの生成
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

    // 頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
            input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixel_shader.ReleaseAndGetAddressOf());
    }
    
    // 定数バッファの生成
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
    // ブレンド用トータル時間をリセット
    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

//アニメーション再生中か
bool Model::IsPlayAnimation()const
{
    if (currentAnimationIndex < 0)return false;
    if (currentAnimationIndex >= animation_clips.size())return false;
    return true;
}

fbxsdk::FbxNode* Model::FindNode(const char* name)
{
    //const size_t nodeCount{ scene_view.nodes.size() };
    //for (size_t index = 0; index < nodeCount; index++)
    //{
    //    FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
    //    if(std::strcmp(FbxNode*fbxnode))
    //}

    for (const scene::node& node : scene_view.nodes)
    {
        //ノード名でノードを検索し取得
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
        if (std::strcmp(fbx_node->GetName(), name) == 0)
        {
            return fbx_node;
    }

    //const size_t node_count{ scene_view.nodes.size() };
    //keyframe.nodes.resize(node_count);
    //for (size_t node_index = 0; node_index < node_count; ++node_index)
    //{
    //    scene::nodes
    //    FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
    //    if (fbx_node)
    //    {
    //        animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
    //        //アニメーション時間からアニメーション行列を取得
    //        node.global_transform = ToXmFloat4x4(fbx_node->EvaluateGlobalTransform(time));

    //        // 'local_transform'は、親のローカル座標系に関するノードの変換行列です
    //        const FbxAMatrix& local_transform{ fbx_node->EvaluateLocalTransform(time) };
    //        node.scaling = ToXmFloat3(local_transform.GetS());
    //        node.rotation = ToXmFloat4(local_transform.GetQ());
    //        node.translation = ToXmFloat3(local_transform.GetT());
    //    }
    //}
}