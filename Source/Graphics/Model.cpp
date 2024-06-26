#include"..\misc.h"
#include"Model.h"
#include<sstream>
#include<functional>

Model::Model(ID3D11Device* device, const char* fbx_filename, bool triangulate)
{
    //fbx�}�l�[�W���[�𐶐�
    FbxManager* fbx_manager{ FbxManager::Create() };
    //fbx�V�[���𐶐�
    FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };

    //fbx�C���|�[�^�[�̐���
    FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
    bool import_status{ false };

    //fbx�t�@�C����ǂݍ���
    import_status = fbx_importer->Initialize(fbx_filename);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    //�ǂݍ��񂾃t�@�C���̏��� fbx �V�[���ɗ�������
    import_status = fbx_importer->Import(fbx_scene);
    _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

    FbxGeometryConverter fbx_converter(fbx_manager);
    if (triangulate)
    {
        //���p�`�ō��ꂽ�|���S�������ׂĎO�p�`�ɂ���
        fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
        fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
    }

    //�m�[�h�̏�����͂���֐�
    std::function<void(FbxNode*)> traverse
    {
        [&](FbxNode* fbx_node)
        {
            //scene_view.nodes �ɐV�����m�[�h�����t���A���t�����m�[�h�����[�J���� node �ɑ�����l��ݒ肵�Ă���
            scene::node& node_{scene_view.nodes.emplace_back()};
            node_.attribute = fbx_node->GetNodeAttribute() ?
                fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
            node_.name = fbx_node->GetName();
            node_.unique_id = fbx_node->GetUniqueID();
            node_.parent_index = scene_view.indexof(fbx_node->GetParent() ?
                fbx_node->GetParent()->GetUniqueID() : 0);
            //���݉�͂��Ă���m�[�h�Ɏ��t�����Ă���̎q�m�[�h����͂��Ă���
            for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
            {
                traverse(fbx_node->GetChild(child_index));
            }
        }
    };
    //���[�g�m�[�h�����͊J�n
    traverse(fbx_scene->GetRootNode());

#if 0
    //�V�[���Ɏ��t�������ׂẴm�[�h�̏���\��
    for (const scene::node& node : scene_view.nodes)
    {
        //�m�[�h���Ńm�[�h���������擾
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