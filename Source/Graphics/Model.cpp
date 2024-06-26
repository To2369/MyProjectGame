#include"..\misc.h"
#include"Model.h"
#include<sstream>
#include<functional>

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
        //Display node data in the output window as debug
        std::string node_name = fbx_node->GetName();
        uint64_t uid = fbx_node->GetUniqueID();
        uint64_t parent_uid = fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0;
        int32_t type = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : 0;
        
        std::stringstream debug_string;
        debug_string << node_name << ":" << uid << ":" << parent_uid << ":" << type << '\n';
        OutputDebugStringA(debug_string.str().c_str());
    }
#endif
    fbx_manager->Destroy();
}