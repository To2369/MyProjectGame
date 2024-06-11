#include "StaticMesh.h"
#include "Shader.h"
#include "..\misc.h"
#include <vector>
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
    //objファイルのキーワード
    wchar_t command[256];
    while (fin)
    {
        //１番上にある１行を取得
        fin >> command;
        if (0 == wcscmp(command, L"v"))
        {
            //頂点座標を取得
            float x, y, z;
            //区切り単位でデータを取得していく
            fin >> x >> y >> z;
            positions.emplace_back( x,y,z );

            //１番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vn"))
        {
            //法線を取得
            float i, j, k;
            fin >> i >> j >> k;
            normals.emplace_back( i,j,k );

            //１番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"vt"))
        {
            //テクスチャ座標取得
            float u, v;
            fin >> u >> v;
            texcoords.emplace_back(u, flipping_v_coordinates ? 1.0f - v : v);

            //１番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                vertex vertex_;
                size_t v, vt, vn;

                //１番上にある１行目の１つ目のデータを取得し
                //データのうちの１つ目の数値を取得
                fin >> v;
                vertex_.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    //１文字削除
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        //スラッシュ区切りで次の数値を取得
                        fin >> vt;
                        vertex_.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek())
                    {
                        //１文字削除
                        fin.ignore(1);
                        //スラッシュ区切りで次の数値を取得
                        fin >> vn;
                        vertex_.normal = normals.at(vn - 1);
                    }
                }
                //頂点データを設定
                vertices.emplace_back(vertex_);
                //頂点インデックスを設定
                indices.emplace_back(current_index++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            //マテリアルファイル名を取得
            wchar_t mtllib[256];
            fin >> mtllib;
            mtl_filenames.emplace_back(mtllib);
        }
        else
        {
            //１番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
    }
    fin.close();

    //MTLファイル名、OBJファイル名を設定
    std::filesystem::path mtl_filename(obj_filename);
    //ファイル名部分のみ MTL ファイル名に入れ替える
    mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());
    //マテリアルファイルを開く
    fin.open(mtl_filename);
    _ASSERT_EXPR(fin, L"MTL file not found.");
    while (fin)
    {
        //1番上にある1行を取得
        fin >> command;

        if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            //テクスチャ名を読み込む
            wchar_t map_Kd[256];
            fin >> map_Kd;

            //テクスチャファイル名にパスを取り付ける
            std::filesystem::path path_(obj_filename);
            path_.replace_filename(std::filesystem::path(map_Kd).filename());
            texture_filename = path_;

            fin.ignore(1024, L'\n');
        }
        else
        {
            fin.ignore(1024, L'\n');
        }
    }

    //頂点バッファのオブジェクトの作成
    Create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    //入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
         {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\StaticMesh_vs.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\StaticMesh_ps.cso",
            pixel_shader.GetAddressOf());
    }

    //定数バッファの生成
    D3D11_BUFFER_DESC buffer_desc{};
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //読み込んだテクスチャを生成
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, texture_filename.c_str(),
            shader_resource_view.GetAddressOf(), &texture2d_desc);
    }
}

//描画処理
void StaticMesh::Render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    //インデックスバッファオブジェクトの設定
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    //定数バッファとして、ワールド行列とマテリアルカラーを設定
    constants data{ world,material_color };
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    //定数(コンスタント)バッファオブジェクトの設定
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    //インデックスを指定して描画
    D3D11_BUFFER_DESC buffer_desc{};
    index_buffer->GetDesc(&buffer_desc);
    immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

//頂点バッファオブジェクトの作成
void StaticMesh::Create_com_buffers(ID3D11Device* device,
    vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    //頂点座標が設定されている配列を設定
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;

    //頂点座標が設定されている配列を設定
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