#include"GeometricPrimitive.h"
#include"..\misc.h"
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
    Create_com_buffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    //入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    shaderMgr = ShaderManager::Instance();
    //頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています)
    shaderMgr->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_vs.cso", vertex_shader.GetAddressOf(),
        input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    //ピクセルシェーダーオブジェクトの作成
    shaderMgr->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitive_ps.cso", pixel_shader.GetAddressOf());
    
    //定数(コンスタント)バッファの作成
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;     
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
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
void GeometricPrimitive::Create_com_buffers(ID3D11Device* device, 
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