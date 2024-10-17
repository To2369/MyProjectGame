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
    // objファイルのキーワード
    wchar_t command[256];
    while (fin)
    {
        // １番上にある１行を取得
        fin >> command;
        if (0 == wcscmp(command, L"v"))
        {
            // 頂点座標を取得
            float x, y, z;
            // 区切り単位でデータを取得していく
            fin >> x >> y >> z;
            positions.push_back({ x, y, z });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            // 法線を取得
            float i, j, k;
            fin >> i >> j >> k;
            normals.push_back({ i, j, k });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            // テクスチャ座標取得
            float u, v;
            fin >> u >> v;
            texcoords.push_back({ u, flipping_v_coordinates ? 1.0f - v : v });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                vertex vertex_;
                size_t v, vt, vn;

                // １番上にある１行目の１つ目のデータを取得し
                // データのうちの１つ目の数値を取得
                fin >> v;
                vertex_.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    //１文字削除
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        // スラッシュ区切りで次の数値を取得
                        fin >> vt;
                        vertex_.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek())
                    {
                        // １文字削除
                        fin.ignore(1);
                        // スラッシュ区切りで次の数値を取得
                        fin >> vn;
                        vertex_.normal = normals.at(vn - 1);
                    }
                }
                // 頂点データを設定
                vertices.push_back(vertex_);
                // 頂点インデックスを設定
                indices.push_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            // マテリアルファイル名を取得
            wchar_t mtllib[256];
            fin >> mtllib;
            mtl_filenames.push_back(mtllib);
        }
        else if (0 == wcscmp(command, L"usemtl"))
        {
            // マテリアル名
            wchar_t usemtl[MAX_PATH]{ 0 };
            fin >> usemtl;

            // マテリアル名とインデックス開始番号を保存
            subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
        }
        else
        {
            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    // それぞれのサブセットのインデックスの数を計算していく
    std::vector<subset>::reverse_iterator iterator_ = subsets.rbegin();
    iterator_->index_count = static_cast<uint32_t>(indices.size()) - iterator_->index_start;
    for (iterator_ = subsets.rbegin() + 1; iterator_ != subsets.rend(); ++iterator_)
    {
        iterator_->index_count = (iterator_ - 1)->index_start - iterator_->index_start;
    }

    // MTLファイル名、OBJファイル名を設定
    std::filesystem::path mtl_filename(obj_filename);
    // ファイル名部分のみ MTL ファイル名に入れ替える
    mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());
    // マテリアルファイルを開く
    fin.open(mtl_filename);
    // _ASSERT_EXPR(fin, L"MTL file not found.");
    while (fin)
    {
        // 1番上にある1行を取得
        fin >> command;

        if (0 == wcscmp(command, L"map_Ka"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t map_Ka[256];
            fin >> map_Ka;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Ka).filename());
            materials.rbegin()->texture_filenames[static_cast<int>(STATICMESH_STATE::NONE)] = path_;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t map_Kd[256];
            fin >> map_Kd;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Kd).filename());
            materials.rbegin()->texture_filenames[static_cast<int>(STATICMESH_STATE::NONE)] = path_;

            fin.ignore(1024, L'\n');
        }
        if (0 == wcscmp(command, L"map_Ks"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t map_Ks[256];
            fin >> map_Ks;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Ks).filename());
            materials.rbegin()->texture_filenames[static_cast<int>(STATICMESH_STATE::NONE)] = path_;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
        {
            fin.ignore();
            // パンプマップ用のテクスチャファイル名の取得
            // パンプマッピングを適用すると凹凸の表現をする
            wchar_t map_bump[256];
            fin >> map_bump;

            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_bump).filename());
            materials.rbegin()->texture_filenames[static_cast<int>(STATICMESH_STATE::BUMP)] = path_;
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"newmtl"))
        {
            fin.ignore();
            wchar_t newmtl[256];
            // 新規でマテリアルを作成
            material mat;
            fin >> newmtl;
            mat.name = newmtl;
            // マテリアルの取り付け
            materials.push_back(mat);
        }
        else if (0 == wcscmp(command, L"Ka"))
        {
            // 色を取得し生成したマテリアルに設定
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Ka = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Kd"))
        {
            // 色を取得し生成したマテリアルに設定
            float r, g, b;
            fin >> r >> g >> b;
            materials.rbegin()->Kd = { r,g,b,1 };
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"Ks"))
        {
            // 色を取得し生成したマテリアルに設定
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

    // 頂点バッファのオブジェクトの作成
    Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
         {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // 頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\StaticMeshVS.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\StaticMeshPS.cso",
            pixel_shaders[static_cast<int>(PIXEL_SHADER_STATE::DEFAULT)].GetAddressOf());
    }
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePS.cso",
            pixel_shaders[static_cast<int>(PIXEL_SHADER_STATE::GEOMETRICPRIMITEVE)].GetAddressOf());
    }

    // 定数バッファの生成
    D3D11_BUFFER_DESC buffer_desc{};
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    if (materials.size() == 0)
    {
        for (const subset& subset_ : subsets)
        {
            materials.push_back({ subset_.usemtl });
        }
    }

    // 読み込んだテクスチャを生成
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    {
        for (material& mat : materials)
        {
            if (mat.texture_filenames[static_cast<int>(STATICMESH_STATE::NONE)].size() > 0)
            {
                ShaderManager::Instance()->LoadTextureFromFile(device, mat.texture_filenames[static_cast<int>(STATICMESH_STATE::NONE)].c_str(),
                    mat.shader_resource_view[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                // ダミー用のカラーマップテクスチャを生成し設定する
                ShaderManager::Instance()->MakeDummyTexture(device, mat.shader_resource_view[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf(),
                    0xFFFFFFFF, 16);
            }

            if (mat.texture_filenames[static_cast<int>(STATICMESH_STATE::BUMP)].size() > 0)
            {
                ShaderManager::Instance()->LoadTextureFromFile(device, mat.texture_filenames[static_cast<int>(STATICMESH_STATE::BUMP)].c_str(),
                    mat.shader_resource_view[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf(), &texture2d_desc);
            }
            else
            {
                // ダミー用の法線マップテクスチャを生成し設定する
                ShaderManager::Instance()->MakeDummyTexture(device, mat.shader_resource_view[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf(),
                    0xFFFFFFFF, 16);
            }
        }
        // 読み込んだ頂点座標の x y z の最小、最大をそれぞれバウンディングボックスに設定する
        for (const vertex& v : vertices)
        {
            bounding_box[0].x = std::min<float>(bounding_box[0].x, v.position.x);
            bounding_box[0].y = std::min<float>(bounding_box[0].y, v.position.y);
            bounding_box[0].z = std::min<float>(bounding_box[0].z, v.position.z);
            bounding_box[1].x = std::max<float>(bounding_box[1].x, v.position.x);
            bounding_box[1].y = std::max<float>(bounding_box[1].y, v.position.y);
            bounding_box[1].z = std::max<float>(bounding_box[1].z, v.position.z);
        }
    }
}

// 描画処理
void StaticMesh::Render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
    PIXEL_SHADER_STATE state)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    // インデックスバッファオブジェクトの設定
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    // 切替用のピクセルシェーダーがあったらそちらを設定
    immediate_context->PSSetShader(pixel_shaders[static_cast<int>(state)].Get(), nullptr, 0);
    for (const material& material_ : materials)
    {
        immediate_context->PSSetShaderResources(0, 1, 
            material_.shader_resource_view[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf());
        immediate_context->PSSetShaderResources(1, 1,
            material_.shader_resource_view[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf());
        // 定数バッファとして、ワールド行列とマテリアルカラーを設定
        constants data{ world,material_.Ka,material_.Kd,material_.Ks };
        // マテリアルカラーは読み込んだ色も反映
        DirectX::XMStoreFloat4(&data.Kd,DirectX::XMVectorMultiply(
            DirectX::XMLoadFloat4(&material_color),DirectX::XMLoadFloat4(&material_.Kd)));
        immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
        // 定数(コンスタント)バッファオブジェクトの設定
        immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
        immediate_context->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
        // サブセットを捜査
        for (const subset& subset_ : subsets)
        {
            // サブセットにあるマテリアル名と一致するものをチェック
            if (material_.name == subset_.usemtl)
            {
                // 一致したサブセットのインデックスの数と開始番号を指定
                immediate_context->DrawIndexed(subset_.index_count, subset_.index_start, 0);
            }
        }
    }
}

// 頂点バッファオブジェクトの作成
void StaticMesh::Create_com_buffers(ID3D11Device* device,
    vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    // 頂点座標が設定されている配列を設定
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;

    // 頂点座標が設定されている配列を設定
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}