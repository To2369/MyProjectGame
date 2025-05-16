#include "StaticMesh.h"
#include "Shader.h"
#include "../Misc.h"
#include <fstream>
#include<filesystem>
StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* objFilename, bool flippingVcoordinates)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t currentIndex{ 0 };

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normals;
    std::vector<DirectX::XMFLOAT2> texcoords;
    std::vector<std::wstring> mtlFilenames;

    std::wifstream fin(objFilename);
    _ASSERT_EXPR(fin, L"'OBJ file not found.");
    // obj�t�@�C���̃L�[���[�h
    wchar_t command[256];
    while (fin)
    {
        // �P�ԏ�ɂ���P�s���擾
        fin >> command;
        if (0 == wcscmp(command, L"v"))
        {
            // ���_���W���擾
            float x, y, z;
            // ��؂�P�ʂŃf�[�^���擾���Ă���
            fin >> x >> y >> z;
            positions.push_back({ x, y, z });

            // �P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            // �@�����擾
            float i, j, k;
            fin >> i >> j >> k;
            normals.push_back({ i, j, k });

            // �P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            // �e�N�X�`�����W�擾
            float u, v;
            fin >> u >> v;
            texcoords.push_back({ u, flippingVcoordinates ? 1.0f - v : v });

            // �P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                Vertex vertex;
                size_t v, vt, vn;

                // �P�ԏ�ɂ���P�s�ڂ̂P�ڂ̃f�[�^���擾��
                // �f�[�^�̂����̂P�ڂ̐��l���擾
                fin >> v;
                vertex.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    //�P�����폜
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        // �X���b�V����؂�Ŏ��̐��l���擾
                        fin >> vt;
                        vertex.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek())
                    {
                        // �P�����폜
                        fin.ignore(1);
                        // �X���b�V����؂�Ŏ��̐��l���擾
                        fin >> vn;
                        vertex.normal = normals.at(vn - 1);
                    }
                }
                // ���_�f�[�^��ݒ�
                vertices.push_back(vertex);
                // ���_�C���f�b�N�X��ݒ�
                indices.push_back(currentIndex++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            // �}�e���A���t�@�C�������擾
            wchar_t mtllib[256];
            fin >> mtllib;
            mtlFilenames.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            // �}�e���A����
            wchar_t usemtl[MAX_PATH]{ 0 };
            fin >> usemtl;

            // �}�e���A�����ƃC���f�b�N�X�J�n�ԍ���ۑ�
            subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
        }
        else
        {
            // �P�ԏ�ɂ���P�s���폜
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    // ���ꂼ��̃T�u�Z�b�g�̃C���f�b�N�X�̐����v�Z���Ă���
    std::vector<Subset>::reverse_iterator iterator = subsets.rbegin();
    iterator->indexCount = static_cast<uint32_t>(indices.size()) - iterator->indexStart;
    for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
    {
        iterator->indexCount = (iterator - 1)->indexStart - iterator->indexStart;
    }

    // MTL�t�@�C�����AOBJ�t�@�C������ݒ�
    std::filesystem::path mtlFilename(objFilename);
    // �t�@�C���������̂� MTL �t�@�C�����ɓ���ւ���
    mtlFilename.replace_filename(std::filesystem::path(mtlFilenames[0]).filename());
    // �}�e���A���t�@�C�����J��
    fin.open(mtlFilename);
    // _ASSERT_EXPR(fin, L"MTL file not found.");
    while (fin)
    {
        // 1�ԏ�ɂ���1�s���擾
        fin >> command;

        if (0 == wcscmp(command, L"map_Ka"))
        {
            fin.ignore();

            // �e�N�X�`������ǂݍ���
            wchar_t mapKa[256];
            fin >> mapKa;

            // �e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKa).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            // �e�N�X�`������ǂݍ���
            wchar_t mapKd[256];
            fin >> mapKd;

            // �e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKd).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        if (0 == wcscmp(command, L"map_Ks"))
        {
            fin.ignore();

            // �e�N�X�`������ǂݍ���
            wchar_t mapKs[256];
            fin >> mapKs;

            // �e�N�X�`���t�@�C�����Ƀp�X�����t����
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKs).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
        {
            fin.ignore();
            // �p���v�}�b�v�p�̃e�N�X�`���t�@�C�����̎擾
            // �p���v�}�b�s���O��K�p����Ɖ��ʂ̕\��������
            wchar_t mapBump[256];
            fin >> mapBump;

            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapBump).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::BUMP)] = path;
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"newmtl"))
        {
            fin.ignore();
            wchar_t newmtl[256];
            // �V�K�Ń}�e���A�����쐬
            Material mat;
            fin >> newmtl;
            mat.name = newmtl;
            // �}�e���A���̎��t��
            materials.push_back(mat);
        }
        else if (0 == wcscmp(command, L"Ka"))
        {
            // �F���擾�����������}�e���A���ɐݒ�
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ka = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Kd"))
        {
            // �F���擾�����������}�e���A���ɐݒ�
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Kd = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Ks"))
        {
            // �F���擾�����������}�e���A���ɐݒ�
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
    fin.close();

    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
         {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\StaticMeshVS.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\StaticMeshPS.cso",
            pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::DEFAULT)].GetAddressOf());
    }
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePS.cso",
            pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::GEOMETRICPRIMITEVE)].GetAddressOf());
    }

    // �萔�o�b�t�@�̐���
    D3D11_BUFFER_DESC bufferDesc{};
    {
        bufferDesc.ByteWidth = sizeof(Constants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    if (materials.size() == 0)
    {
        for (const Subset& subset : subsets)
        {
            materials.push_back({ subset.usemtl });
        }
    }

    // �ǂݍ��񂾃e�N�X�`���𐶐�
    D3D11_TEXTURE2D_DESC texture2dDesc{};
    {
        for (Material& mat : materials)
        {
            if (mat.textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)].size() > 0)
            {
                ShaderManager::Instance()->LoadTextureFromFile(device, mat.textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)].c_str(),
                    mat.shaderResourceView[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf(), &texture2dDesc);
            }
            else
            {
                // �_�~�[�p�̃J���[�}�b�v�e�N�X�`���𐶐����ݒ肷��
                ShaderManager::Instance()->MakeDummyTexture(device, mat.shaderResourceView[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf(),
                    0xFFFFFFFF, 16);
            }

            if (mat.textureFilenames[static_cast<int>(STATICMESH_STATE::BUMP)].size() > 0)
            {
                ShaderManager::Instance()->LoadTextureFromFile(device, mat.textureFilenames[static_cast<int>(STATICMESH_STATE::BUMP)].c_str(),
                    mat.shaderResourceView[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf(), &texture2dDesc);
            }
            else
            {
                // �_�~�[�p�̖@���}�b�v�e�N�X�`���𐶐����ݒ肷��
                ShaderManager::Instance()->MakeDummyTexture(device, mat.shaderResourceView[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf(),
                    0xFFFFFFFF, 16);
            }
        }
        // �ǂݍ��񂾒��_���W�� x y z �̍ŏ��A�ő�����ꂼ��o�E���f�B���O�{�b�N�X�ɐݒ肷��
        for (const Vertex& v : vertices)
        {
            boundingBox[0].x = std::min<float>(boundingBox[0].x, v.position.x);
            boundingBox[0].y = std::min<float>(boundingBox[0].y, v.position.y);
            boundingBox[0].z = std::min<float>(boundingBox[0].z, v.position.z);
            boundingBox[1].x = std::max<float>(boundingBox[1].x, v.position.x);
            boundingBox[1].y = std::max<float>(boundingBox[1].y, v.position.y);
            boundingBox[1].z = std::max<float>(boundingBox[1].z, v.position.z);
        }
    }
}

// �`�揈��
void StaticMesh::Render(ID3D11DeviceContext* dc,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
    PIXEL_SHADER_STATE state)
{
    uint32_t stride{ sizeof(Vertex) };
    uint32_t offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    // �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());

    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    // �ؑ֗p�̃s�N�Z���V�F�[�_�[���������炻�����ݒ�
    dc->PSSetShader(pixelShaders[static_cast<int>(state)].Get(), nullptr, 0);
    for (const Material& material : materials)
    {
        dc->PSSetShaderResources(0, 1, 
            material.shaderResourceView[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf());
        dc->PSSetShaderResources(1, 1,
            material.shaderResourceView[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf());
#if 1
        // �萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
        Constants data{ world,material.Ka,material.Kd,material.Ks };
        // �}�e���A���J���[�͓ǂݍ��񂾐F�����f
        DirectX::XMStoreFloat4(&data.Kd, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&materialColor), DirectX::XMLoadFloat4(&material.Kd)));
# else
        // �萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
        constants data{ world,material_color };
        // �}�e���A���J���[�͓ǂݍ��񂾐F�����f
        DirectX::XMStoreFloat4(&data.material_color, DirectX::XMVectorMultiply(
            DirectX::XMLoadFloat4(&material_color), DirectX::XMLoadFloat4(&material_.Kd)));
# endif
        dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
        // �萔(�R���X�^���g)�o�b�t�@�I�u�W�F�N�g�̐ݒ�
        dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        // �T�u�Z�b�g��{��
        for (const Subset& subset : subsets)
        {
            // �T�u�Z�b�g�ɂ���}�e���A�����ƈ�v������̂��`�F�b�N
            if (material.name == subset.usemtl)
            {
                // ��v�����T�u�Z�b�g�̃C���f�b�N�X�̐��ƊJ�n�ԍ����w��
                dc->DrawIndexed(subset.indexCount, subset.indexStart, 0);
            }
        }
    }
}

// ���_�o�b�t�@�I�u�W�F�N�g�̍쐬
void StaticMesh::CreateComBuffers(ID3D11Device* device,
    Vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount)
{
    HRESULT hr{ S_OK };

    // ���_���W���ݒ肳��Ă���z���ݒ�
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // ���_���W���ݒ肳��Ă���z���ݒ�
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData,
        vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresourceData.pSysMem = indices;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}