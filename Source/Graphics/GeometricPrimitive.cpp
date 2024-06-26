#include"GeometricPrimitive.h"
#include"..\misc.h"
#include<vector>
#define VertexPoints 24
#define VertexNumber 36
GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
    //サイズが 1.0 の正立方体データを作成する（重心を原点にする）。
    //正立方体のコントロールポイント数は 8 個、
    //1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
    //頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    vertex vertices[VertexPoints]{};

    //正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので
    //3 角形ポリゴンの総数は 6x2=12 個、
    //正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
    //3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
    //時計回りが表面になるように格納すること。
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

    //頂点データとインデックスデータを使って頂点バッファオブジェクトを作成
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    //入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_vs.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //ピクセルシェーダーオブジェクトの作成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_ps.cso",
            pixel_shader.GetAddressOf());
    }
    //定数(コンスタント)バッファの作成
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

//描画
void GeometricPrimitive::Render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
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
void GeometricPrimitive::CreateComBuffers(ID3D11Device* device, 
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

//正方形
GeometricCube::GeometricCube(ID3D11Device* device) :GeometricPrimitive(device)
{
    //サイズが 1.0 の正立方体データを作成する（重心を原点にする）。
     //正立方体のコントロールポイント数は 8 個、
     //1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
     //頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    vertex vertices[VertexPoints]{};

    //正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので
    //3 角形ポリゴンの総数は 6x2=12 個、
    //正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
    //3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
    //時計回りが表面になるように格納すること。
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

    //頂点データとインデックスデータを使って頂点バッファオブジェクトを作成
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    //入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_vs.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //ピクセルシェーダーオブジェクトの作成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_ps.cso",
            pixel_shader.GetAddressOf());
    }

    //定数(コンスタント)バッファの作成
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

//円柱
GeometricCylinder::GeometricCylinder(ID3D11Device* device,
    uint32_t slices):GeometricPrimitive(device)
{
    // 分割数(円を作るための三角形の数、６個の三角形)
    // 半径
    float radius = 0.5f;

    // 頂点データ
    vertex* vertices = new vertex[(slices + 1) * 2 + slices * 2];
    // 頂点インデックス
    uint32_t* indices = new uint32_t[(slices * 3) * 2 + (3 * 2 * slices)];

    // 三角形の角度 (デグリー角)
    float d{ 2.0f * DirectX::XM_PI / slices };
    // 半径
    float r{ 0.5f };

    // 真ん中の頂点座標と法線
    vertices[0].position = { 0.0f, 0.5f, 0.0f };
    vertices[0].normal = { 0.0f, 1.0f, 0.0f };

    // 円の周りの頂点座標と法線を設定
    for (int i = 0; i < slices; ++i) {

        // 角度から頂点座標を計算
        vertices[1 + i].position.x = radius * cosf(i * d);
        vertices[1 + i].position.y = 0.5f;
        vertices[1 + i].position.z = radius * sinf(i * d);
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
    int base_slices = slices + 1;

    // 下の円の真ん中の頂点座標と法線
    vertices[base_slices].position = { 0.0f, -0.5f, 0.0f };
    vertices[base_slices].normal = { 0.0f, -1.0f, 0.0f };

    // 円の周りの頂点座標と法線を設定
    for (int i = 0; i < slices; ++i)
    {
        // 角度から頂点座標を計算
        vertices[slices + 1 + 1 + i].position.x = radius * cosf(i * d);
        vertices[slices + 1 + 1 + i].position.y = -0.5f;
        vertices[slices + 1 + 1 + i].position.z = radius * sinf(i * d);
        // 法線を設定
        vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
    }

    // 下の円のインデックスの設定
    int base_index = slices + 1;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices[(slices * 3) + (i * 3) + 0] = base_index + 0;
        indices[(slices * 3) + (i * 3) + 1] = base_index + i + 1;
        indices[(slices * 3) + (i * 3) + 2] = base_index + i + 2;
    }

    indices[(slices * 3 * 2) - 1] = base_index + 1;
    indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
    indices[(slices * 3 * 2) - 3] = base_index;


    // 側面
    for (int i = 0; i < slices; ++i)
    {
        float x = radius * cosf(i * d);
        float z = radius * sinf(i * d);

        // 上の点の座標と法線
        vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { x, 0.5f , z };
        vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { x, 0.0f , z };

        // 下の点の座標と法線
        vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { x, -0.5f , z };
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
    uint32_t slices,
    uint32_t stacks) :GeometricPrimitive(device)
{
    //球の頂点とインデックスのリストを初期化
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    //球の半径
    float r{ 0.5f };

    //上部と下部の頂点を作成
    vertex top_vertex{};
    top_vertex.position = { 0.0f, +r, 0.0f };
    top_vertex.normal = { 0.0f, +1.0f, 0.0f };

    vertex bottom_vertex{};
    bottom_vertex.position = { 0.0f, -r, 0.0f };
    bottom_vertex.normal = { 0.0f, -1.0f, 0.0f };

    //頂点リストに上部の頂点を追加
    vertices.emplace_back(top_vertex);

    //経度方向の分割数に基づいて縦方向のループを実行
    float phi_step{ DirectX::XM_PI / stacks };
    float theta_step{ 2.0f * DirectX::XM_PI / slices };

    for (uint32_t i = 1; i <= stacks - 1; ++i)
    {
        float phi{ i * phi_step };

        //経度方向のループを実行
        for (uint32_t j = 0; j <= slices; ++j)
        {
            float theta{ j * theta_step };

            vertex v{};

            //球の表面上の点の座標を計算
            v.position.x = r * sinf(phi) * cosf(theta);
            v.position.y = r * cosf(phi);
            v.position.z = r * sinf(phi) * sinf(theta);

            //法線ベクトルを計算して正規化
            DirectX::XMVECTOR p{ XMLoadFloat3(&v.position) };
            DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

            //頂点リストに頂点を追加
            vertices.emplace_back(v);
        }
    }

    //頂点リストに下部の頂点を追加
    vertices.emplace_back(bottom_vertex);

    //インデックスの生成
    //三角形ストリップの各頂点インデックスを生成
    for (uint32_t i = 1; i <= slices; ++i)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    //経度方向のループで三角形ストリップを生成
    uint32_t base_index{ 1 };
    uint32_t ring_vertex_count{ slices + 1 };
    for (uint32_t i = 0; i < stacks - 2; ++i)
    {
        for (uint32_t j = 0; j < slices; ++j)
        {
            indices.emplace_back(base_index + i * ring_vertex_count + j);
            indices.emplace_back(base_index + i * ring_vertex_count + j + 1);
            indices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);

            indices.emplace_back(base_index + (i + 1) * ring_vertex_count + j);
            indices.emplace_back(base_index + i * ring_vertex_count + j + 1);
            indices.emplace_back(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
    }

    //南極点のインデックスを取得
    uint32_t south_pole_index{ static_cast<uint32_t>(vertices.size() - 1) };

    //南極点の直上のリングのベースインデックスを取得
    base_index = south_pole_index - ring_vertex_count;

    //南極点を中心とした三角形ストリップを生成
    for (uint32_t i = 0; i < slices; ++i)
    {
        indices.emplace_back(south_pole_index);
        indices.emplace_back(base_index + i);
        indices.emplace_back(base_index + i + 1);
    }

    //頂点バッファのオブジェクトの作成
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

//カプセル
GeometricCapsule::GeometricCapsule(ID3D11Device* device,
    float mantle_height,
    const DirectX::XMFLOAT3& radius,
    uint32_t slices,
    uint32_t ellipsoid_stacks,
    uint32_t mantle_stacks) :GeometricPrimitive(device)
{
    std::vector<vertex> vertices;   //頂点のリスト
    std::vector<uint32_t> indices;  //インデックスのリスト
    const int base_offset = 0;      //基準オフセット

    //スライス、マントルスタック、楕円体スタックの最小値を設定
    slices = std::max<uint32_t>(3u, slices);
    mantle_stacks = std::max<uint32_t>(1u, mantle_stacks);
    ellipsoid_stacks = std::max<uint32_t>(2u, ellipsoid_stacks);

    //各種ステップの逆数を計算
    const float inv_slices = 1.0f / static_cast<float>(slices);
    const float inv_mantle_stacks = 1.0f / static_cast<float>(mantle_stacks);
    const float inv_ellipsoid_stacks = 1.0f / static_cast<float>(ellipsoid_stacks);

    const float pi_2{ 3.14159265358979f * 2.0f };   //2π
    const float pi_0_5{ 3.14159265358979f * 0.5f }; //π/2
    const float angle_steps = inv_slices * pi_2;    //角度ステップ
    const float half_height = mantle_height * 0.5f; //マントルの高さの半分

    //マントルの頂点を生成 
    struct spherical {
        float radius, theta, phi;
    } point{ 1, 0, 0 };     //球の座標系情報
    DirectX::XMFLOAT3 position, normal;     //位置と法線ベクトル
    DirectX::XMFLOAT2 texcoord;             //テクスチャ座標

    float angle = 0.0f; //角度初期化
    for (uint32_t u = 0; u <= slices; ++u)
    {
        //X- および Z- 座標を計算
        texcoord.x = sinf(angle);
        texcoord.y = cosf(angle);

        position.x = texcoord.x * radius.x;
        position.z = texcoord.y * radius.z;

        //法線ベクトルを計算
        normal.x = texcoord.x;
        normal.y = 0;
        normal.z = texcoord.y;

        float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x = normal.x / magnitude;
        normal.y = normal.y / magnitude;
        normal.z = normal.z / magnitude;

        //上部と下部の頂点を追加
        texcoord.x = static_cast<float>(slices - u) * inv_slices;

        for (uint32_t v = 0; v <= mantle_stacks; ++v)
        {
            texcoord.y = static_cast<float>(v) * inv_mantle_stacks; //頂点の Y 座標を計算
#if _HAS_CXX20
            position.y = lerp(half_height, -half_height, texcoord.y);
#else
            position.y = half_height * (1 - texcoord.y) + -half_height * texcoord.y;
#endif
            vertices.push_back({ position, normal });   //頂点を追加
        }

        //次のイテレーションのために角度を増加
        angle += angle_steps;
    }

    //上部および下部のカバー頂点を生成 
    const float cover_side[2] = { 1, -1 };
    uint32_t base_offset_ellipsoid[2] = { 0 };
    for (size_t i = 0; i < 2; ++i)
    {
        base_offset_ellipsoid[i] = static_cast<uint32_t>(vertices.size());

        for (uint32_t v = 0; v <= ellipsoid_stacks; ++v)
        {
            //球面座標のθを計算
            texcoord.y = static_cast<float>(v) * inv_ellipsoid_stacks;
            point.theta = texcoord.y * pi_0_5;

            for (uint32_t u = 0; u <= slices; ++u)
            {
                //球面座標のφを計算
                texcoord.x = static_cast<float>(u) * inv_slices;
                point.phi = texcoord.x * pi_2 * cover_side[i] + pi_0_5;

                //球面座標を直交座標に変換し、法線ベクトルを設定
                const float sin_theta = sinf(point.theta);
                position.x = point.radius * cosf(point.phi) * sin_theta;
                position.y = point.radius * sinf(point.phi) * sin_theta;
                position.z = point.radius * cosf(point.theta);

                std::swap(position.y, position.z);
                position.y *= cover_side[i];

                //ノーマルを取得し、半球を移動
                float magnitude = sqrtf(position.x * position.x + position.y * position.y + position.z * position.z);
                normal.x = position.x / magnitude;
                normal.y = position.y / magnitude;
                normal.z = position.z / magnitude;

                //半径と高さで座標を変換
                position.x *= radius.x;
                position.y *= radius.y;
                position.z *= radius.z;
                position.y += half_height * cover_side[i];

                //新しい頂点を追加
                vertices.push_back({ position, normal });
            }
        }
    }

    //マントルのインデックスを生成
    int offset = base_offset;
    for (uint32_t u = 0; u < slices; ++u)
    {
        for (uint32_t v = 0; v < mantle_stacks; ++v)
        {
            auto i0 = v + 1 + mantle_stacks;
            auto i1 = v;
            auto i2 = v + 1;
            auto i3 = v + 2 + mantle_stacks;

            indices.emplace_back(i0 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i3 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i2 + offset);
            indices.emplace_back(i3 + offset);
        }
        offset += (1 + mantle_stacks);
    }

    //上部および下部のインデックスを生成
    for (size_t i = 0; i < 2; ++i)
    {
        for (uint32_t v = 0; v < ellipsoid_stacks; ++v)
        {
            for (uint32_t u = 0; u < slices; ++u)
            {
                //現在の面のインデックスを計算
                auto i0 = v * (slices + 1) + u;
                auto i1 = v * (slices + 1) + (u + 1);

                auto i2 = (v + 1) * (slices + 1) + (u + 1);
                auto i3 = (v + 1) * (slices + 1) + u;

                //新しいインデックスを追加
                indices.emplace_back(i0 + base_offset_ellipsoid[i]);
                indices.emplace_back(i1 + base_offset_ellipsoid[i]);
                indices.emplace_back(i3 + base_offset_ellipsoid[i]);
                indices.emplace_back(i1 + base_offset_ellipsoid[i]);
                indices.emplace_back(i2 + base_offset_ellipsoid[i]);
                indices.emplace_back(i3 + base_offset_ellipsoid[i]);
            }
        }
    }
    //頂点バッファのオブジェクトの作成
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}