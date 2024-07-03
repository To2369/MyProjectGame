#include"..\misc.h"
#include"Model.h"
#include<sstream>
#include<functional>
#include<filesystem>
#include"Shader.h"

//FbxAMatrix����XMFLOAT�SX4�ɕϊ�
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

//FbxDouble3����XMFLOAT3�ɕϊ�
inline DirectX::XMFLOAT3 ToXmFloat3(const FbxDouble3& fbxdouble3)
{
    DirectX::XMFLOAT3 xmfloat3;
    xmfloat3.x = static_cast<float>(fbxdouble3[0]);
    xmfloat3.y = static_cast<float>(fbxdouble3[1]);
    xmfloat3.z = static_cast<float>(fbxdouble3[2]);
    return xmfloat3;
}

//FbxDouble4����XMFLOAT4�ɕϊ�
inline DirectX::XMFLOAT4 ToXmFloat4(const FbxDouble4& fbxdouble4)
{
    DirectX::XMFLOAT4 xmfloat4;
    xmfloat4.x = static_cast<float>(fbxdouble4[0]);
    xmfloat4.y = static_cast<float>(fbxdouble4[1]);
    xmfloat4.z = static_cast<float>(fbxdouble4[2]);
    xmfloat4.w = static_cast<float>(fbxdouble4[3]);
    return xmfloat4;
}

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
    traverse(fbx_scene->GetRootNode());

    FetchMeshes(fbx_scene, meshes);

    FetchMaterials(fbx_scene, materials);

    fbx_manager->Destroy();

    CreateComObjects(device, fbx_filename);
}

//�`�揈��
void Model::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
{
    //���b�V�����Ƃɕ`�揈�����s��
    for (const mesh& mesh_ : meshes)
    {
        //���_�o�b�t�@�̐ݒ�
        uint32_t stride{ sizeof(vertex) };
        uint32_t offset{ 0 };
        immediate_context->IASetVertexBuffers(0, 1, mesh_.vertex_buffer.GetAddressOf(), &stride, &offset);
        //�C���f�b�N�X�o�b�t�@�̐ݒ�
        immediate_context->IASetIndexBuffer(mesh_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        //�v���~�e�B�u�̃g�|���W�[�ݒ�i�O�p�`���X�g�j
        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //���̓��C�A�E�g�̐ݒ�
        immediate_context->IASetInputLayout(input_layout.Get());

        //���_�V�F�[�_�[�̐ݒ�
        immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
        //�s�N�Z���V�F�[�_�[�̐ݒ�
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

        //�萔�o�b�t�@�̍X�V�i���[���h�s��ƃ}�e���A���J���[�̐ݒ�
        constants data;
        DirectX::XMStoreFloat4x4(&data.world, 
            DirectX::XMLoadFloat4x4(&mesh_.default_global_transform) * DirectX::XMLoadFloat4x4(&world));
        for (const mesh::subset& subset : mesh_.subsets)
        {
            //�}�e���A���̎���ID ����}�e���A�����擾���Q�ƂƂ��Đݒ�
            const material& material_{ materials.at(subset.material_unique_id) };

            DirectX::XMStoreFloat4(&data.material_color,
                DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&material_color), DirectX::XMLoadFloat4(&material_.Kd)));
            immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
            immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
            immediate_context->PSSetShaderResources(0, 1,
                material_.shader_resource_views[0].GetAddressOf());
            //�C���f�b�N�X���g�p���ĕ`��
            //D3D11_BUFFER_DESC buffer_desc;
            //mesh_.index_buffer->GetDesc(&buffer_desc);
            immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
        }
    }
}

//���b�V�����̎��o��
void Model::FetchMeshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
    //�V�[���̒��ɂ���m�[�h�̏������ׂČ���
    for (const scene::node& node_ : scene_view.nodes)
    {
        //�m�[�h�̒����烁�b�V���̑����������Ă���m�[�h���`�F�b�N
        if (node_.attribute != FbxNodeAttribute::EType::eMesh)
        {
            continue;
        }

        //�V�[������ FbxNode ���擾
        FbxNode* fbx_node{ fbx_scene->FindNodeByName(node_.name.c_str()) };
        //���b�V�������擾
        FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

        //���b�V����V�K�ō쐬�����t��
        mesh& mesh_{ meshes.emplace_back() };
        //���b�V���̎���ID�̐ݒ�
        mesh_.unique_id = fbx_mesh->GetNode()->GetUniqueID();
        //���b�V�����̐ݒ�
        mesh_.name = fbx_mesh->GetNode()->GetName();
        //���b�V���ɑ΂���m�[�hID�̊���U��
        mesh_.node_index = scene_view.indexof(mesh_.unique_id);
        
        //���b�V���̃O���[�o���s����擾��XMFLOAT4X4�ɕϊ����đ��
        mesh_.default_global_transform = ToXmFloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());

        std::vector<mesh::subset>& subsets{ mesh_.subsets };
        //�}�e���A�������擾
        const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
        //�T�u�Z�b�g�̐������T�C�Y
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
                //���b�V���̃|���S���̔ԍ�����C���f�b�N�X�ԍ����擾���}�e���A���̃C���f�b�N�X�̔ԍ��Ƃ��Ċ��蓖�Ă�
                const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
                //�T�u�Z�b�g�ɑ΂��Ċ��蓖�Ă�ꂽ�}�e���A���̃C���f�b�N�X�̔ԍ��ɃC���f�b�N�X�̐��𒸓_����(3)���₵�Ă���
                subsets.at(material_index).index_count += 3;
            }
            //�C���f�b�N�X�̐����炻�ꂼ��̊J�n�̐����v�Z���Đݒ肵�Ă���
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

        //uv���̎擾�B��X�e�N�X�`�����W�̎擾�ɗ��p�B
        FbxStringList uv_names;
        fbx_mesh->GetUVSetNames(uv_names);

        //�R���g���[���|�C���g�̎擾
        const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
        //�|���S���̐��������_�f�[�^���擾
        for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
        {
            const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
            mesh::subset& subset{ subsets.at(material_index) };
            const uint32_t offset{ subset.start_index_location + subset.index_count };
            //�O�p�`�̐����̒��_�̏����擾����
            for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
            {
                //���p����C���f�b�N�X�̔z����v�Z
                const int vertex_index{ polygon_index * 3 + position_in_polygon };

                //���_���W�̎擾
                vertex vertex_;
                const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index,position_in_polygon) };
                vertex_.position.x = static_cast<float>(control_points[polygon_vertex][0]);
                vertex_.position.y = static_cast<float>(control_points[polygon_vertex][1]);
                vertex_.position.z = static_cast<float>(control_points[polygon_vertex][2]);

                //�@���̎擾
                if (fbx_mesh->GetElementNormalCount() > 0)
                {
                    FbxVector4 normal;
                    fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
                    vertex_.normal.x = static_cast<float>(normal[0]);
                    vertex_.normal.y = static_cast<float>(normal[1]);
                    vertex_.normal.z = static_cast<float>(normal[2]);
                }

                //�e�N�X�`�����W�̎擾
                if (fbx_mesh->GetElementUVCount() > 0)
                {
                    FbxVector2 uv;
                    bool unmapped_uv;
                    fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
                        uv_names[0], uv, unmapped_uv);
                    vertex_.texcoord.x = static_cast<float>(uv[0]);
                    vertex_.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                }

                //���݂̃C���f�b�N�X�ԍ������ɒ��_�f�[�^��ݒ�
                mesh_.vertices.at(vertex_index) = std::move(vertex_);
                //���݂̃C���f�b�N�X�ԍ���ݒ�
                mesh_.indices.at(static_cast<size_t>(offset)+position_in_polygon) = vertex_index;
                subset.index_count++;
            }
        }
    }
}

//�}�e���A�����̎��o��
void Model::FetchMaterials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
    //�V�[���̒��ɂ���m�[�h�̏������ׂČ���
    const size_t node_count{ scene_view.nodes.size() };
    for (size_t node_index = 0; node_index < node_count; ++node_index)
    {
        //�m�[�h�̏��P�P���`�F�b�N
        const scene::node& node{ scene_view.nodes.at(node_index) };
        //�V�[������FbxNode���擾
        const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

        //�m�[�h�̒��ɂ���}�e���A���̏������ׂČ���
        const int material_count{ fbx_node->GetMaterialCount() };
        for (int material_index = 0; material_index < material_count; ++material_index)
        {
            //�}�e���A���̏��P�P���`�F�b�N
            const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

            // �}�e���A���̐ݒ�
            material material_;
            material_.name = fbx_material->GetName();
            material_.unique_id = fbx_material->GetUniqueID();

            FbxProperty fbx_property;
            //�f�B�t���[�Y(�g�U���ˌ�)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            //�f�B�t���[�Y����������f�B�t���[�Y�̏���ݒ肵�Ă���
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Kd.x = static_cast<float>(color[0]);
                material_.Kd.y = static_cast<float>(color[1]);
                material_.Kd.z = static_cast<float>(color[2]);
                material_.Kd.w = 1.0f;

                //�e�N�X�`���̃t�@�C�������擾
                const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
                //���΃p�X���݂ł̃t�@�C������ݒ肷��
                material_.texture_filename[0] =
                    fbx_texture ? fbx_texture->GetRelativeFileName() : "";
            }
            //�A���r�G���g(����)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ka.x = static_cast<float>(color[0]);
                material_.Ka.y = static_cast<float>(color[1]);
                material_.Ka.z = static_cast<float>(color[2]);
                material_.Ka.w = 1.0f;
            }
            //�X�y�L����(���ʔ���)�̏����擾
            fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
            if (fbx_property.IsValid())
            {
                const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
                material_.Ks.x = static_cast<float>(color[0]);
                material_.Ks.y = static_cast<float>(color[1]);
                material_.Ks.z = static_cast<float>(color[2]);
                material_.Ks.w = 1.0f;
            }
            //�擾�����}�e���A���̏���ݒ肷��
            materials.emplace(material_.unique_id, std::move(material_));
        }
    }
#if 1
    //�_�~�[�̃}�e���A����}��
    material material_;
    materials.emplace(material_.unique_id, std::move(material_));
#endif
}

//�o�b�t�@�̐���
void Model::CreateComObjects(ID3D11Device* device, const char* fbx_filename)
{
    //�e���b�V���ɑ΂��ăo�b�t�@�𐶐����郋�[�v
    for (mesh& mesh_ : meshes)
    {
        HRESULT hr{ S_OK };

        //���_�o�b�t�@�̐���
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

        //�C���f�b�N�X�o�b�t�@�̐���
        buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh_.indices.size());
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        subresource_data.pSysMem = mesh_.indices.data();
        hr = device->CreateBuffer(&buffer_desc, &subresource_data,
            mesh_.index_buffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
#if 0
        //���b�V���̒��_�ƃC���f�b�N�X���N���A����
        mesh_.vertices.clear();
        mesh_.indices.clear();
#endif
    }
    //�}�e���A���̒��ɂ���e�N�X�`���t�@�C�����̐������e�N�X�`���𐶐�����
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
    //���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
        { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT },
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT },
    };

    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ModelVS.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\ModelPS.cso",
            pixel_shader.ReleaseAndGetAddressOf());
    }
    
    //�萔�o�b�t�@�̐���
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}