#include"GeometricPrimitive.h"
#include"..\Misc.h"
#include<vector>
#define VertexPoints 24
#define VertexNumber 36
GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
    // サイズが 1.0 の正立方体データを作成する（重心を原点にする）。
    // 正立方体のコントロールポイント数は 8 個、
    // 1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
    // 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    vertex vertices[VertexPoints]{};

    // 正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので
    // 3 角形ポリゴンの総数は 6x2=12 個、
    // 正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
    // 3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
    // 時計回りが表面になるように格納すること。
    uint32_t indices[VertexNumber]{};

    // 上面
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    uint32_t face = 0;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // 下面
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 前面
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // 後面
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 右面
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // 左面
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 頂点データとインデックスデータを使って頂点バッファオブジェクトを作成
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // 頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitiveVs.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // ピクセルシェーダーオブジェクトの作成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePs.cso",
            pixelShader.GetAddressOf());
    }
    // 定数(コンスタント)バッファの作成
    {
        bufferDesc.ByteWidth = sizeof(constants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
}

// 描画
void GeometricPrimitive::Render(ID3D11DeviceContext* dc,
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& materialColor)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    // インデックスバッファオブジェクトの設定
    dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());

    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);

    // 定数バッファとして、ワールド行列とマテリアルカラーを設定
    constants data{ world,materialColor };
    dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    // 定数(コンスタント)バッファオブジェクトの設定
    dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // インデックスを指定して描画
    D3D11_BUFFER_DESC buffer_desc{};
    indexBuffer->GetDesc(&buffer_desc);
    dc->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

// 頂点バッファオブジェクトの作成
void GeometricPrimitive::CreateComBuffers(ID3D11Device* device, 
    vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount)
{
    HRESULT hr{ S_OK };

    // 頂点座標が設定されている配列を設定
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertexCount);
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

//正方形
GeometricCube::GeometricCube(ID3D11Device* device) :GeometricPrimitive(device)
{
    // サイズが 1.0 の正立方体データを作成する（重心を原点にする）。
    // 正立方体のコントロールポイント数は 8 個、
    // 1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
    // 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    vertex vertices[VertexPoints]{};

    // 正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので
    // 3 角形ポリゴンの総数は 6x2=12 個、
    // 正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
    // 3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
    // 時計回りが表面になるように格納すること。
    uint32_t indices[VertexNumber]{};

   // 上面
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    uint32_t face = 0;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

   // 下面
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 前面
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // 後面
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 右面
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // 左面
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // 頂点データとインデックスデータを使って頂点バッファオブジェクトを作成
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // 頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitiveVS.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // ピクセルシェーダーオブジェクトの作成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePS.cso",
            pixelShader.GetAddressOf());
    }

    // 定数(コンスタント)バッファの作成
    {
        bufferDesc.ByteWidth = sizeof(constants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
}

// 円柱
GeometricCylinder::GeometricCylinder(ID3D11Device* device, float radius, float height, uint32_t slices)
    :GeometricPrimitive(device)
{
    // 分割数(円を作るための三角形の数、６個の三角形)
    // 半径
    float radius_ = radius;
    // 頂点データ
    vertex* vertices = new vertex[(slices + 1) * 2 + slices * 2];
    // 頂点インデックス
    uint32_t* indices = new uint32_t[(slices * 3) * 2 + (3 * 2 * slices)];

    // 三角形の角度 (デグリー角)
    float d{ 2.0f * DirectX::XM_PI / slices };
    // 半径
    //float r{ 0.5f };

    // 真ん中の頂点座標と法線
    vertices[0].position = { 0.0f, 1.0f, 0.0f };
    vertices[0].normal = { 0.0f, 1.0f, 0.0f };

    // 円の周りの頂点座標と法線を設定
    for (int i = 0; i < slices; ++i) {

        // 角度から頂点座標を計算
        vertices[1 + i].position.x = radius_ * cosf(i * d);
        vertices[1 + i].position.y = 1.0f;
        vertices[1 + i].position.z = radius_ * sinf(i * d);
        // 法線を設定
        vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
    }

    for (int i = 0; i < slices - 1; ++i)
    {
        indices[(i * 3) + 0] = 0;
        indices[(i * 3) + 1] = i + 2;
        indices[(i * 3) + 2] = i + 1;
    }

    indices[slices * 3 - 3] = 0;
    indices[slices * 3 - 2] = 1;
    indices[slices * 3 - 1] = slices;

    // 下の円
    int baseSlices = slices + 1;

    // 下の円の真ん中の頂点座標と法線
    vertices[baseSlices].position = { 0.0f, 0.0f, 0.0f };
    vertices[baseSlices].normal = { 0.0f, -1.0f, 0.0f };

    // 円の周りの頂点座標と法線を設定
    for (int i = 0; i < slices; ++i)
    {
        // 角度から頂点座標を計算
        vertices[slices + 1 + 1 + i].position.x = radius_ * cosf(i * d);
        vertices[slices + 1 + 1 + i].position.y = 0.0f;
        vertices[slices + 1 + 1 + i].position.z = radius_ * sinf(i * d);
        // 法線を設定
        vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
    }

    // 下の円のインデックスの設定
    int baseIndex = slices + 1;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices[(slices * 3) + (i * 3) + 0] = baseIndex + 0;
        indices[(slices * 3) + (i * 3) + 1] = baseIndex + i + 1;
        indices[(slices * 3) + (i * 3) + 2] = baseIndex + i + 2;
    }

    indices[(slices * 3 * 2) - 1] = baseIndex + 1;
    indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
    indices[(slices * 3 * 2) - 3] = baseIndex;


    // 側面
    for (int i = 0; i < slices; ++i)
    {
        float x = radius_ * cosf(i * d);
        float z = radius_ * sinf(i * d);

        // 上の点の座標と法線
        vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { x, 1.0f , z };
        vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { x, 0.0f , z };

        // 下の点の座標と法線
        vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { x, 0.0f , z };
        vertices[slices + 1 + slices + 1 + i * 2 + 1].normal = { x, 0.0f , z };
    }

    for (int i = 0; i < slices - 1; ++i)
    {
        // i 番目の四角形の１つ目の三角形
        indices[slices * 3 * 2 + i * 6 + 0] = slices + 1 + slices + 1 + i * 2 + 0;
        indices[slices * 3 * 2 + i * 6 + 1] = slices + 1 + slices + 1 + i * 2 + 2;
        indices[slices * 3 * 2 + i * 6 + 2] = slices + 1 + slices + 1 + i * 2 + 1;
        // i 番目の四角形の２つ目の三角形
        indices[slices * 3 * 2 + i * 6 + 3] = slices + 1 + slices + 1 + i * 2 + 1;
        indices[slices * 3 * 2 + i * 6 + 4] = slices + 1 + slices + 1 + i * 2 + 2;
        indices[slices * 3 * 2 + i * 6 + 5] = slices + 1 + slices + 1 + i * 2 + 3;
    }

    // ６つ目の四角形の１つ目の三角形
    indices[slices * 3 * 2 + (slices - 1) * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 1] = slices + 1 + slices + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 2] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
    // ６つ目の四角形の２つ目の三角形
    indices[slices * 3 * 2 + (slices - 1) * 6 + 3] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 4] = slices + 1 + slices + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 5] = slices + 1 + slices + 1 + 1;

    //頂点バッファのオブジェクトの作成
    CreateComBuffers(device, vertices, (slices + 1) * 2 + slices * 2, indices, (slices * 3) * 2 + (3 * 2 * slices));

    delete[] vertices;
    delete[] indices;
}

//球
GeometricSphere::GeometricSphere(
    ID3D11Device* device,
    float radius,
    uint32_t slices,
    uint32_t stacks):GeometricPrimitive(device)
{
    // 球の頂点とインデックスのリストを初期化
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    // 球の半径
    float r{ radius };

    // 上部と下部の頂点を作成
    vertex topVertex{};
    topVertex.position = { 0.0f, +r, 0.0f };
    topVertex.normal = { 0.0f, +1.0f, 0.0f };

    vertex bottomVertex{};
    bottomVertex.position = { 0.0f, -r, 0.0f };
    bottomVertex.normal = { 0.0f, -1.0f, 0.0f };

    // 頂点リストに上部の頂点を追加
    vertices.emplace_back(topVertex);

    // 経度方向の分割数に基づいて縦方向のループを実行
    float phiStep{ DirectX::XM_PI / stacks };
    float thetaStep{ 2.0f * DirectX::XM_PI / slices };

    for (uint32_t i = 1; i <= stacks - 1; ++i)
    {
        float phi{ i * phiStep };

        // 経度方向のループを実行
        for (uint32_t j = 0; j <= slices; ++j)
        {
            float theta{ j * thetaStep };

            vertex v{};

            // 球の表面上の点の座標を計算
            v.position.x = r * sinf(phi) * cosf(theta);
            v.position.y = r * cosf(phi);
            v.position.z = r * sinf(phi) * sinf(theta);

            // 法線ベクトルを計算して正規化
            DirectX::XMVECTOR p{ XMLoadFloat3(&v.position) };
            DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

            // 頂点リストに頂点を追加
            vertices.emplace_back(v);
        }
    }

    // 頂点リストに下部の頂点を追加
    vertices.emplace_back(bottomVertex);

    // インデックスの生成
    // 三角形ストリップの各頂点インデックスを生成
    for (uint32_t i = 1; i <= slices; ++i)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    // 経度方向のループで三角形ストリップを生成
    uint32_t baseIndex{ 1 };
    uint32_t ringVertexCount{ slices + 1 };
    for (uint32_t i = 0; i < stacks - 2; ++i)
    {
        for (uint32_t j = 0; j < slices; ++j)
        {
            indices.emplace_back(baseIndex + i * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    // 南極点のインデックスを取得
    uint32_t southPoleIndex{ static_cast<uint32_t>(vertices.size() - 1) };

    // 南極点の直上のリングのベースインデックスを取得
    baseIndex = southPoleIndex - ringVertexCount;

    // 南極点を中心とした三角形ストリップを生成
    for (uint32_t i = 0; i < slices; ++i)
    {
        indices.emplace_back(southPoleIndex);
        indices.emplace_back(baseIndex + i);
        indices.emplace_back(baseIndex + i + 1);
    }

    // 頂点バッファのオブジェクトの作成
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

// カプセル
GeometricCapsule::GeometricCapsule(ID3D11Device* device,
    float mantleHeight,
    float radius,
    uint32_t slices,
    uint32_t ellipsoidStacks,
    uint32_t stacks) :GeometricPrimitive(device)
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    const int baseOffset = 0;

    slices = std::max<uint32_t>(3u, slices);
    stacks = std::max<uint32_t>(1u, stacks);
    ellipsoidStacks = std::max<uint32_t>(2u, ellipsoidStacks);

    const float invSlices = 1.0f / static_cast<float>(slices);
    const float invMantleStacks = 1.0f / static_cast<float>(stacks);
    const float invEllipsoidStacks = 1.0f / static_cast<float>(ellipsoidStacks);

    const float pi2{ 3.14159265358979f * 2.0f };
    const float pi05{ 3.14159265358979f * 0.5f };
    const float angleSteps = invSlices * pi2;
    const float halfHeight = mantleHeight * 0.5f;

    /* Generate mantle vertices */
    struct spherical {
        float radius, theta, phi;
    } point{ 1, 0, 0 };
    DirectX::XMFLOAT3 position, normal;
    DirectX::XMFLOAT2 texcoord;

    float angle = 0.0f;

    // マントル部分の頂点生成ループ
    for (uint32_t u = 0; u <= slices; ++u)
    {
        /* X座標とZ座標の計算 */
        texcoord.x = sinf(angle);
        texcoord.y = cosf(angle);

        position.x = texcoord.x * radius;
        position.z = texcoord.y * radius;

        /* 法線ベクトルの計算 */
        normal.x = texcoord.x;
        normal.y = 0;
        normal.z = texcoord.y;

        float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x = normal.x / magnitude;
        normal.y = normal.y / magnitude;
        normal.z = normal.z / magnitude;

        /* 上部と下部の頂点を追加 */
        texcoord.x = static_cast<float>(slices - u) * invSlices;

        for (uint32_t v = 0; v <= stacks; ++v)
        {
            texcoord.y = static_cast<float>(v) * invMantleStacks;
#if _HAS_CXX20
            position.y = lerp(half_height, -half_height, texcoord.y);
#else
            position.y = halfHeight * (1 - texcoord.y) + -halfHeight * texcoord.y;
#endif

            vertices.push_back({ position, normal });
        }

        /* 次のスライス用に角度を増加 */
        angle += angleSteps;
    }

    const float topCoverSide = { 1 };
    uint32_t topBaseOffsetEllipsoid = { 0 };

    topBaseOffsetEllipsoid = static_cast<uint32_t>(vertices.size());

    for (uint32_t v = 0; v <= ellipsoidStacks; ++v)
    {
        /* 球面座標系のθの計算 */
        texcoord.y = static_cast<float>(v) * invEllipsoidStacks;
        point.theta = texcoord.y * pi05;

        for (uint32_t u = 0; u <= slices; ++u)
        {
            /* 球面座標系のφの計算 */
            texcoord.x = static_cast<float>(u) * invSlices;
            point.phi = texcoord.x * pi2 * topCoverSide + pi05;

            /* 球面座標系を直交座標系に変換して、法線をセット */
            const float sin_theta = sinf(point.theta);
            position.x = point.radius * cosf(point.phi) * sin_theta;
            position.y = point.radius * sinf(point.phi) * sin_theta;
            position.z = point.radius * cosf(point.theta);

            std::swap(position.y, position.z);
            position.y *= topCoverSide;

            /* 法線を求めて半球体を移動 */
            float magnitude = sqrtf(position.x * position.x + position.y * position.y + position.z * position.z);
            normal.x = position.x / magnitude;
            normal.y = position.y / magnitude;
            normal.z = position.z / magnitude;

            /* 半径と高さを使って座標を変換 */
            position.x *= radius;
            position.y *= radius;
            position.z *= radius;
            position.y += halfHeight * topCoverSide;

            // 頂点の座標に回転を適用
            vertices.push_back({ position, normal });
        }
    }

    const float bottomCoverSide = { -1 };
    uint32_t bottomBaseOffsetEllipsoid = { 0 };

    bottomBaseOffsetEllipsoid = static_cast<uint32_t>(vertices.size());

    for (uint32_t v = 0; v <= ellipsoidStacks; ++v)
    {
        /* 球面座標系のθの計算 */
        texcoord.y = static_cast<float>(v) * invEllipsoidStacks;
        point.theta = texcoord.y * pi05;

        for (uint32_t u = 0; u <= slices; ++u)
        {
            /* 球面座標系のφの計算 */
            texcoord.x = static_cast<float>(u) * invSlices;
            point.phi = texcoord.x * pi2 * bottomCoverSide + pi05;

            /* 球面座標系を直交座標系に変換して、法線をセット */
            const float sin_theta = sinf(point.theta);
            position.x = point.radius * cosf(point.phi) * sin_theta;
            position.y = point.radius * sinf(point.phi) * sin_theta;
            position.z = point.radius * cosf(point.theta);

            std::swap(position.y, position.z);
            position.y *= bottomCoverSide;

            /* 法線を求めて半球体を移動 */
            float magnitude = sqrtf(position.x * position.x + position.y * position.y + position.z * position.z);
            normal.x = position.x / magnitude;
            normal.y = position.y / magnitude;
            normal.z = position.z / magnitude;

            /* 半径と高さを使って座標を変換 */
            position.x *= radius;
            position.y *= radius;
            position.z *= radius;
            position.y += halfHeight * bottomCoverSide;

            // 頂点の座標に回転を適用
            vertices.push_back({ position, normal });
        }
    }
    /* Generate indices for the mantle */
    int offset = baseOffset;
    for (uint32_t u = 0; u < slices; ++u)
    {
        for (uint32_t v = 0; v < stacks; ++v)
        {
            auto i0 = v + 1 + stacks;
            auto i1 = v;
            auto i2 = v + 1;
            auto i3 = v + 2 + stacks;

            indices.emplace_back(i0 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i3 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i2 + offset);
            indices.emplace_back(i3 + offset);
        }
        offset += (1 + stacks);
    }

    /* Generate indices for the top and bottom */
    for (uint32_t v = 0; v < ellipsoidStacks; ++v)
    {
        for (uint32_t u = 0; u < slices; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v * (slices + 1) + u;
            auto i1 = v * (slices + 1) + (u + 1);

            auto i2 = (v + 1) * (slices + 1) + (u + 1);
            auto i3 = (v + 1) * (slices + 1) + u;

            /* Add new indices */
            indices.emplace_back(i0 + topBaseOffsetEllipsoid);
            indices.emplace_back(i1 + topBaseOffsetEllipsoid);
            indices.emplace_back(i3 + topBaseOffsetEllipsoid);
            indices.emplace_back(i1 + topBaseOffsetEllipsoid);
            indices.emplace_back(i2 + topBaseOffsetEllipsoid);
            indices.emplace_back(i3 + topBaseOffsetEllipsoid);
        }
    }
    for (uint32_t v = 0; v < ellipsoidStacks; ++v)
    {
        for (uint32_t u = 0; u < slices; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v * (slices + 1) + u;
            auto i1 = v * (slices + 1) + (u + 1);

            auto i2 = (v + 1) * (slices + 1) + (u + 1);
            auto i3 = (v + 1) * (slices + 1) + u;

            /* Add new indices */
            indices.emplace_back(i0 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i1 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i3 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i1 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i2 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i3 + bottomBaseOffsetEllipsoid);
        }
    }

    // 頂点バッファのオブジェクトの作成
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}