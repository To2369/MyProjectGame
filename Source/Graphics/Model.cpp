#include"..\misc.h"
#include"Model.h"
#include<sstream>
#include<functional>
#include<filesystem>
#include"Shader.h"

//FbxAMatrixからXMFLOAT４X4に変換
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

//FbxDouble3からXMFLOAT3に変換
inline DirectX::XMFLOAT3 ToXmFloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

//FbxDouble4からXMFLOAT4に変換
inline DirectX::XMFLOAT4 ToXmFloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

//１つの頂点が影響を受けるボーンの情報
struct bone_influence
{
    uint32_t bone_index;//ボーン番号
    float bone_weight;  //ウェイト値
};
//１つの頂点は複数のボーンから影響を受ける場合があるので可変長配列で表現
using bone_influences_per_control_point = std::vector<bone_influence>;

void FetchBoneInfluences(const FbxMesh* fbx_mesh,
    std::vector<bone_influences_per_control_point>& bone_influences)
{
    //ボーン影響度の数 = FBX メッシュにあるコントロールポイントの数に設定
    const int control_points_count{ fbx_mesh->GetControlPointsCount() };
    bone_influences.resize(control_points_count);
    //メッシュにあるスキンの数を取得
    const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
    //メッシュにあるすべてのスキンの情報をチェック
    for (int skin_index = 0; skin_index < skin_count; ++skin_index)
    {
        //現在のスキンを取得
        const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index,FbxDeformer::eSkin)) };

        //スキンにあるクラスターの数を取得
        const int cluster_count{ fbx_skin->GetClusterCount() };
        //スキンにあるすべてのクラスターの情報をチェック
        for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
        {
            //現在のクラスターの情報をチェック
            const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

            //クラスターにあるコントロールポイントの数を取得
            const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
            //クラスターにあるすべてのコントロールポイントのウェイトの値を取得
            for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
            {
                //現在のコントロールポイントの番号
                int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
                //コントロールポイントのウェイト値
                double control_point_weight{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
                //新規にボーン影響度を生成して追加
                bone_influence& bone_influence_{ bone_influences.at(control_point_index).emplace_back() };
                //現在のクラスターの番号をボーン番号として設定
                bone_influence_.bone_index = static_cast<uint32_t>(cluster_index);
                //コントロールのウェイト値をボーンのウェイト値として設定
                bone_influence_.bone_weight = static_cast<float>(control_point_weight);
            }
        }
    }
}

Model::Model(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
    //fbxマネージャーを生成
    FbxManager* fbx_manager{ FbxManager::Create() };
    //fbxシーンを生成
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

    //fbxインポーターの生成
    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
    bool import_status{ false };

    //fbxファイルを読み込む
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    //読み込んだファイルの情報を fbx シーンに流し込む
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        //多角形で作られたポリゴンをすべて三角形にする
        fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    //ノードの情報を解析する関数
    std::function<void(FbxNode*)> traverse
    {
        [&](FbxNode* fbx_node)
        {
            //scene_view.nodes に新しくノードを取り付け、取り付けたノードをローカルの node に代入し値を設定していく
            scene::node& node_{scene_view.nodes.emplace_back()};
            node_.attribute = fbx_node->GetNodeAttribute() ?
                fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
            node_.name = fbx_node->GetName();
            node_.unique_id = fbx_node->GetUniqueID();
            node_.parent_index = scene_view.indexof(fbx_node->GetParent() ?
                fbx_node->GetParent()->GetUniqueID() : 0);
            //現在解析しているノードに取り付けられているの子ノードを解析していく
            for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
            {
                traverse(fbx_node->GetChild(child_index));
            }
        }
    };
    //ルートノードから解析開始
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

    fbx_manager->Destroy();

    CreateComObjects(device, fbx_filename);
}

//描画処理
void Model::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
{
    //メッシュごとに描画処理を行う
    for (const mesh& mesh_ : meshes)
    {

        //頂点バッファの設定
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
        //インデックスバッファの設定
        immediate_context->IASetIndexBuffer(mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //プリミティブのトポロジー設定（三角形リスト）
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //入力レイアウトの設定
        immediate_context->IASetInputLayout(input_layout.Get());

        //頂点シェーダーの設定
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        //ピクセルシェーダーの設定
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        //定数バッファの更新（ワールド行列とマテリアルカラーの設定
        constants data;
        DirectX::XMStoreFloat4x4(&data.world, 
            DirectX::XMLoadFloat4x4(&mesh_.default_global_transform) * DirectX::XMLoadFloat4x4(&world));
#if 1
        DirectX::XMStoreFloat4x4(&data.bone_transforms[0], DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&data.bone_transforms[1], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(45)));
        DirectX::XMStoreFloat4x4(&data.bone_transforms[2], DirectX::XMMatrixRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(-45)));
#endif
        for (const mesh::subset& subset : mesh_.subsets)
        {
            //マテリアルの識別ID からマテリアルを取得し参照として設定
            const material& material_{ materials.at(subset.material_unique_id) };

            DirectX::XMStoreFloat4(&data.material_color,
                DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&material_color), DirectX::XMLoadFloat4(&material_.Kd)));
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
            immediate_context->PSSetShaderResources(0, 1,
                material_.shader_resource_views[0].GetAddressOf());
            //インデックスを使用して描画
            //D3D11_BUFFER_DESC buffer_desc;
            //mesh_.index_buffer->GetDesc(&buffer_desc);
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}

//メッシュ情報の取り出し
void Model::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    //シーンの中にあるノードの情報をすべて検索
    for (const scene::node& node_ : scene_view.nodes)
    {
        //ノードの中からメッシュの属性を持っているノードをチェック
        if (node_.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        //シーンから FbxNode を取得
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node_.name.c_str()) };
        //メッシュ情報を取得
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        //メッシュを新規で作成し取り付け
        mesh& mesh_{ meshes.emplace_back() };
        //メッシュの識別IDの設定
        mesh_.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        //メッシュ名の設定
        mesh_.name = fbx_mesh->GetNode()->GetName();
        //メッシュに対するノードIDの割り振り
        mesh_.node_index = scene_view.indexof(mesh_.unique_id);
        
        //メッシュのグローバル行列を取得しXMFLOAT4X4に変換して代入
        mesh_.default_global_transform = ToXmFloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

        //メッシュからボーン影響度を取得
        std::vector<bone_influences_per_control_point> bone_influences;
        FetchBoneInfluences(fbx_mesh, bone_influences);

        std::vector<mesh::subset>& subsets{ mesh_.subsets };
        //マテリアル数を取得
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        //サブセットの数をリサイズ
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
                //メッシュのポリゴンの番号からインデックス番号を取得しマテリアルのインデックスの番号として割り当てる
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                //サブセットに対して割り当てられたマテリアルのインデックスの番号にインデックスの数を頂点数分(3)増やしていく
                subsets.at(material_index).index_count += 3;
            }
            //インデックスの数からそれぞれの開始の数を計算して設定していく
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

        //uv名の取得。後々テクスチャ座標の取得に利用。
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        //コントロールポイントの取得
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        //ポリゴンの数だけ頂点データを取得
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };
            //三角形の数分の頂点の情報を取得する
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                //利用するインデックスの配列を計算
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                //頂点座標の取得
                vertex vertex_;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index,position_in_polygon) };
                vertex_.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex_.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex_.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                //頂点数番目のボーン影響度を取得
                const bone_influences_per_control_point& influces_per_control_point{ bone_influences.at(polygon_vertex) };
                for (size_t influence_index = 0; influence_index < influces_per_control_point.size(); ++influence_index)
                {
                    //ボーン影響度は最大４つ
                    if (influence_index < MAX_BONE_INFLUENCES)
                    {
                        //取得したボーン影響度を頂点側のボーン影響度に設定していく
                        vertex_.bone_weights[influence_index] =
                            influces_per_control_point.at(influence_index).bone_weight;
                        vertex_.bone_indices[influence_index] =
                            influces_per_control_point.at(influence_index).bone_index;
                    }
                    else
                    {
                        //最小の影響度を持つボーンを探す
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

                        //最小の影響度を持つボーンの影響度を加算し、インデックスを更新する
                        vertex_.bone_weights[minimum_value_index] += influces_per_control_point[influence_index].bone_weight;
                        vertex_.bone_indices[minimum_value_index] = influces_per_control_point[influence_index].bone_index;

                        //合計の影響度を計算し、全ての影響度を正規化する
                        float total_weight = 0.0f;
                        for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
                        {
                            total_weight += vertex_.bone_weights[i];
                        }

                        //影響度を正規化する
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

                //法線の取得
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex_.normal.x = static_cast<float>(normal[0]);
                    vertex_.normal.y = static_cast<float>(normal[1]);
                    vertex_.normal.z = static_cast<float>(normal[2]);
                }

                //テクスチャ座標の取得
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex_.texcoord.x = static_cast<float>(uv[0]);
                    vertex_.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                //現在のインデックス番号部分に頂点データを設定
                mesh_.vertices.at(vertex_index) = std::move(vertex_);
                //現在のインデックス番号を設定
                mesh_.indices.at(static_cast<size_t>(offset)+position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
    }
}

//マテリアル情報の取り出し
void Model::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
    //シーンの中にあるノードの情報をすべて検索
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        //ノードの情報１つ１つをチェック
        const scene::node& node{ scene_view.nodes.at(node_index) };
        //シーンからFbxNodeを取得
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        //ノードの中にあるマテリアルの情報をすべて検索
        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            //マテリアルの情報１つ１つをチェック
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            // マテリアルの設定
            material material_;
            material_.name = fbx_material->GetName();
            material_.unique_id = fbx_material->GetUniqueID();

            FbxProperty fbx_property;
            //ディフューズ(拡散反射光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            //ディフューズがあったらディフューズの情報を設定していく
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Kd.x = static_cast<float>(color[0]);
                material_.Kd.y = static_cast<float>(color[1]);
                material_.Kd.z = static_cast<float>(color[2]);
                material_.Kd.w = 1.0f;

                //テクスチャのファイル名を取得
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                //相対パス込みでのファイル名を設定する
                material_.texture_filename[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            //アンビエント(環境光)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ka.x = static_cast<float>(color[0]);
                material_.Ka.y = static_cast<float>(color[1]);
                material_.Ka.z = static_cast<float>(color[2]);
                material_.Ka.w = 1.0f;
            }
            //スペキュラ(鏡面反射)の情報を取得
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ks.x = static_cast<float>(color[0]);
                material_.Ks.y = static_cast<float>(color[1]);
                material_.Ks.z = static_cast<float>(color[2]);
                material_.Ks.w = 1.0f;
            }
            //取得したマテリアルの情報を設定する
            materials.emplace(material_.unique_id, std::move(material_));
        }
    }
#if 0
    //ダミーのマテリアルを挿入
    material material_;
    materials.emplace(material_.unique_id, std::move(material_));
#endif
}

//バッファの生成
void Model::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    //各メッシュに対してバッファを生成するループ
    for (mesh& mesh_ : meshes)
    {
        HRESULT hr{ S_OK };

        //頂点バッファの生成
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

        //インデックスバッファの生成
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh_.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh_.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh_.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        //メッシュの頂点とインデックスをクリアする
        mesh_.vertices.clear();
        mesh_.indices.clear();
#endif
    }
    //マテリアルの中にあるテクスチャファイル名の数だけテクスチャを生成する
    for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
    {
        if (iterator->second.texture_filename[0].size() > 0)
        {
            std::filesystem::path path(fbx_filename);
            path.replace_filename(iterator->second.texture_filename[0]);
            D3D11_TEXTURE2D_DESC texture2d_desc;
            ShaderManager::Instance()->LoadTextureFromFile(device, path.c_str(),
                iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, iterator->second.shader_resource_views[0].GetAddressOf(),
                0xFFFFFFFF, 16);
        }
    }
    HRESULT hr = S_OK;
    //入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"BONES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    //頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
            input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixel_shader.ReleaseAndGetAddressOf());
    }
    
    //定数バッファの生成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}