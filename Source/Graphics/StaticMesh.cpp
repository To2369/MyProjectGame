#include "StaticMesh.h"
#include "Shader.h"
#include "..\misc.h"
#include <fstream>
#include<filesystem>
StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* obj_filename, bool flipping_v_coordinates)
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t current_index{ 0 };

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normals;
    std::vector<DirectX::XMFLOAT2> texcoords;
    std::vector<std::wstring> mtl_filenames;

    std::wifstream fin(obj_filename);
    _ASSERT_EXPR(fin, L"'OBJ file not found.");
    //obj�t�@�C���̃L�[���[�h
    wchar_t command[256];
    while (fin)
    {
        //�P�ԏ�ɂ���P�s���擾
        fin >> command;
        if (0 == wcscmp(command, L"v"))
        {
            //���_���W���擾
            float x, y, z;
            //��؂�P�ʂŃf�[�^���擾���Ă���
            fin >> x >> y >> z;
            positions.push_back({ x, y, z });

            //�P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            //�@�����擾
            float i, j, k;
            fin >> i >> j >> k;
            normals.push_back({ i, j, k });

            //�P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            //�e�N�X�`�����W�擾
            float u, v;
            fin >> u >> v;
            texcoords.push_back({ u, flipping_v_coordinates ? 1.0f - v : v });

            //�P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                vertex vertex_;
                size_t v, vt, vn;

                //�P�ԏ�ɂ���P�s�ڂ̂P�ڂ̃f�[�^���擾��
                //�f�[�^�̂����̂P�ڂ̐��l���擾
                fin >> v;
                vertex_.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    //�P�����폜
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        //�X���b�V����؂�Ŏ��̐��l���擾
                        fin >> vt;
                        vertex_.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek())
                    {
                        //�P�����폜
                        fin.ignore(1);
                        //�X���b�V����؂�Ŏ��̐��l���擾
                        fin >> vn;
                        vertex_.normal = normals.at(vn - 1);
                    }
                }
                //���_�f�[�^��ݒ�
                vertices.push_back(vertex_);
                //���_�C���f�b�N�X��ݒ�
                indices.push_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            //�}�e���A���t�@�C�������擾
            wchar_t mtllib[256];
            fin >> mtllib;
            mtl_filenames.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            //�}�e���A����
            wchar_t usemtl[MAX_PATH]{ 0 };
            fin >> usemtl;

            //�}�e���A�����ƃC���f�b�N�X�J�n�ԍ���ۑ�
            subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
        }
        else
        {
            //�P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    //���ꂼ��̃T�u�Z�b�g�̃C���f�b�N�X�̐����v�Z���Ă���
    std::vector<subset>::reverse_iterator iterator_ = subsets.rbegin();
    iterator_->index_count = static_cast<uint32_t>(indices.size()) - iterator_->index_start;
    for (iterator_ = subsets.rbegin() + 1; iterator_ != subsets.rend(); ++iterator_)
    {
        iterator_->index_count = (iterator_ - 1)->index_start - iterator_->index_start;
    }

    //MTL�t�@�C�����AOBJ�t�@�C������ݒ�
    std::filesystem::path mtl_filename(obj_filename);
    //�t�@�C���������̂� MTL �t�@�C�����ɓ���ւ���
    mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());
    //�}�e���A���t�@�C�����J��
    fin.open(mtl_filename);
    _ASSERT_EXPR(fin, L"MTL file not found.");
    while (fin)
    {
        //1�ԏ�ɂ���1�s���擾
        fin >> command;

        if (0 == wcscmp(command, L"map_Ka"))
        {
            fin.ignore();

            //�e�N�X�`������ǂݍ���
            wchar_t map_Ka[256];
            fin >> map_Ka;

            //�e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Ka).filename());
            materials.rbegin()->texture_filename = path_;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            //�e�N�X�`������ǂݍ���
            wchar_t map_Kd[256];
            fin >> map_Kd;

            //�e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Kd).filename());
            materials.rbegin()->texture_filename = path_;

            fin.ignore(1024, L'\n');
        }
        if (0 == wcscmp(command, L"map_Ks"))
        {
            fin.ignore();

            //�e�N�X�`������ǂݍ���
            wchar_t map_Ks[256];
            fin >> map_Ks;

            //�e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Ks).filename());
            materials.rbegin()->texture_filename = path_;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"newmtl"))
        {
            fin.ignore();
            wchar_t newmtl[256];
            //�V�K�Ń}�e���A�����쐬
            material mat;
            fin >> newmtl;
            mat.name = newmtl;
            //�}�e���A���̎��t��
            materials.emplace_back(mat);
        }
        else if (0 == wcscmp(command, L"Ka"))
        {
            //�F���擾�����������}�e���A���ɐݒ�
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ka = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Kd"))
        {
            //�F���擾�����������}�e���A���ɐݒ�
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Kd = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Ks"))
        {
            //�F���擾�����������}�e���A���ɐݒ�
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ks = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else
        {
            fin.ignore(1024, L'\n');
        }
    }

    //���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    //���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
         {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\StaticMesh_vs.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\StaticMesh_ps.cso",
            pixel_shader.GetAddressOf());
    }

    //�萔�o�b�t�@�̐���
    D3D11_BUFFER_DESC buffer_desc{};
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //�ǂݍ��񂾃e�N�X�`���𐶐�
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    {
        for (material& mat : materials)
        {
            ShaderManager::Instance()->LoadTextureFromFile(device, mat.texture_filename.c_str(),
                mat.shader_resource_view.GetAddressOf(), &texture2d_desc);

        }

    }
}

//�`�揈��
void StaticMesh::Render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    //�C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    for (const material& material_ : materials)
    {
        immediate_context->PSSetShaderResources(0, 1, material_.shader_resource_view.GetAddressOf());

        //�萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
        constants data{ world,material_color };
        //�}�e���A���J���[�͓ǂݍ��񂾐F�����f
        DirectX::XMStoreFloat4(&data.material_color,DirectX::XMVectorMultiply(
            DirectX::XMLoadFloat4(&material_color),DirectX::XMLoadFloat4(&material_.Kd)));
        immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
        //�萔(�R���X�^���g)�o�b�t�@�I�u�W�F�N�g�̐ݒ�
        immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
    
        //�T�u�Z�b�g��{��
        for (const subset& subset_ : subsets)
        {
            //�T�u�Z�b�g�ɂ���}�e���A�����ƈ�v������̂��`�F�b�N
            if (material_.name == subset_.usemtl)
            {
                //��v�����T�u�Z�b�g�̃C���f�b�N�X�̐��ƊJ�n�ԍ����w��
                immediate_context->DrawIndexed(subset_.index_count, subset_.index_start, 0);
            }
        }
    }
}

//���_�o�b�t�@�I�u�W�F�N�g�̍쐬
void StaticMesh::Create_com_buffers(ID3D11Device* device,
    vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    //���_���W���ݒ肳��Ă���z���ݒ�
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;

    //���_���W���ݒ肳��Ă���z���ݒ�
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}