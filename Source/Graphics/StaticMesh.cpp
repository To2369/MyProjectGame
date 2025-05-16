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
            texcoords.push_back({ u, flippingVcoordinates ? 1.0f - v : v });

            // １番上にある１行を削除
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"f"))
        {
            for (size_t i = 0; i < 3; i++)
            {
                Vertex vertex;
                size_t v, vt, vn;

                // １番上にある１行目の１つ目のデータを取得し
                // データのうちの１つ目の数値を取得
                fin >> v;
                vertex.position = positions.at(v - 1);
                if (L'/' == fin.peek())
                {
                    //１文字削除
                    fin.ignore(1);
                    if (L'/' != fin.peek())
                    {
                        // スラッシュ区切りで次の数値を取得
                        fin >> vt;
                        vertex.texcoord = texcoords.at(vt - 1);
                    }
                    if (L'/' == fin.peek())
                    {
                        // １文字削除
                        fin.ignore(1);
                        // スラッシュ区切りで次の数値を取得
                        fin >> vn;
                        vertex.normal = normals.at(vn - 1);
                    }
                }
                // 頂点データを設定
                vertices.push_back(vertex);
                // 頂点インデックスを設定
                indices.push_back(currentIndex++);
            }
            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"mtllib"))
        {
            // マテリアルファイル名を取得
            wchar_t mtllib[256];
            fin >> mtllib;
            mtlFilenames.push_back(mtllib);
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
    std::vector<Subset>::reverse_iterator iterator = subsets.rbegin();
    iterator->indexCount = static_cast<uint32_t>(indices.size()) - iterator->indexStart;
    for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
    {
        iterator->indexCount = (iterator - 1)->indexStart - iterator->indexStart;
    }

    // MTLファイル名、OBJファイル名を設定
    std::filesystem::path mtlFilename(objFilename);
    // ファイル名部分のみ MTL ファイル名に入れ替える
    mtlFilename.replace_filename(std::filesystem::path(mtlFilenames[0]).filename());
    // マテリアルファイルを開く
    fin.open(mtlFilename);
    // _ASSERT_EXPR(fin, L"MTL file not found.");
    while (fin)
    {
        // 1番上にある1行を取得
        fin >> command;

        if (0 == wcscmp(command, L"map_Ka"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t mapKa[256];
            fin >> mapKa;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKa).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_Kd"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t mapKd[256];
            fin >> mapKd;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKd).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        if (0 == wcscmp(command, L"map_Ks"))
        {
            fin.ignore();

            // テクスチャ名を読み込む
            wchar_t mapKs[256];
            fin >> mapKs;

            // テクスチャファイル名にパスを取り付ける
            std::filesystem::path path(objFilename);
            path.replace_filename(std::filesystem::path(mapKs).filename());
            materials.rbegin()->textureFilenames[static_cast<int>(STATICMESH_STATE::NONE)] = path;

            fin.ignore(1024, L'\n');
        }
        else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
        {
            fin.ignore();
            // パンプマップ用のテクスチャファイル名の取得
            // パンプマッピングを適用すると凹凸の表現をする
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
            // 新規でマテリアルを作成
            Material mat;
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
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
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
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\StaticMeshVS.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\StaticMeshPS.cso",
            pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::DEFAULT)].GetAddressOf());
    }
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePS.cso",
            pixelShaders[static_cast<int>(PIXEL_SHADER_STATE::GEOMETRICPRIMITEVE)].GetAddressOf());
    }

    // 定数バッファの生成
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

    // 読み込んだテクスチャを生成
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
                // ダミー用のカラーマップテクスチャを生成し設定する
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
                // ダミー用の法線マップテクスチャを生成し設定する
                ShaderManager::Instance()->MakeDummyTexture(device, mat.shaderResourceView[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf(),
                    0xFFFFFFFF, 16);
            }
        }
        // 読み込んだ頂点座標の x y z の最小、最大をそれぞれバウンディングボックスに設定する
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

// 描画処理
void StaticMesh::Render(ID3D11DeviceContext* dc,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor,
    PIXEL_SHADER_STATE state)
{
    uint32_t stride{ sizeof(Vertex) };
    uint32_t offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    // インデックスバッファオブジェクトの設定
    dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());

    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    // 切替用のピクセルシェーダーがあったらそちらを設定
    dc->PSSetShader(pixelShaders[static_cast<int>(state)].Get(), nullptr, 0);
    for (const Material& material : materials)
    {
        dc->PSSetShaderResources(0, 1, 
            material.shaderResourceView[static_cast<int>(STATICMESH_STATE::NONE)].GetAddressOf());
        dc->PSSetShaderResources(1, 1,
            material.shaderResourceView[static_cast<int>(STATICMESH_STATE::BUMP)].GetAddressOf());
#if 1
        // 定数バッファとして、ワールド行列とマテリアルカラーを設定
        Constants data{ world,material.Ka,material.Kd,material.Ks };
        // マテリアルカラーは読み込んだ色も反映
        DirectX::XMStoreFloat4(&data.Kd, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&materialColor), DirectX::XMLoadFloat4(&material.Kd)));
# else
        // 定数バッファとして、ワールド行列とマテリアルカラーを設定
        constants data{ world,material_color };
        // マテリアルカラーは読み込んだ色も反映
        DirectX::XMStoreFloat4(&data.material_color, DirectX::XMVectorMultiply(
            DirectX::XMLoadFloat4(&material_color), DirectX::XMLoadFloat4(&material_.Kd)));
# endif
        dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
        // 定数(コンスタント)バッファオブジェクトの設定
        dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
        // サブセットを捜査
        for (const Subset& subset : subsets)
        {
            // サブセットにあるマテリアル名と一致するものをチェック
            if (material.name == subset.usemtl)
            {
                // 一致したサブセットのインデックスの数と開始番号を指定
                dc->DrawIndexed(subset.indexCount, subset.indexStart, 0);
            }
        }
    }
}

// 頂点バッファオブジェクトの作成
void StaticMesh::CreateComBuffers(ID3D11Device* device,
    Vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount)
{
    HRESULT hr{ S_OK };

    // 頂点座標が設定されている配列を設定
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // 頂点座標が設定されている配列を設定
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