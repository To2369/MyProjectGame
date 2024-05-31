#include "Sprite_batch.h"
#include"../misc.h"
#include <sstream>
#include"Shader.h"
#include<WICTextureLoader.h>
Sprite_batch::Sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites):max_vertices(max_sprites*6)
{
    HRESULT hr{ S_OK };

    //頂点情報のセット
    std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };
    //頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};                                                //頂点バッファオブジェクトの設定を行うための構造体
    buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;                               //頂点情報のサイズを指定
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                                        //バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               //バッファがパイプラインにバインドされる方法を特定,このバッファオブジェクトを頂点バッファとして設定
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            //CPU へアクセスする際のフラグ
    buffer_desc.MiscFlags = 0;                                                      //その他のフラグ。不要な場合は０
    buffer_desc.StructureByteStride = 0;                                            //バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。

    //サブリソースデータ
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // 初期化データへのポインタ
    subresource_data.pSysMem = vertices.get();                                        //テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresource_data.SysMemPitch = 0;                                           //テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresource_data.SysMemSlicePitch = 0;                                      //ある深度レベルの開始から次の深度レベルまでの距離(バイト単位)
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf()); //デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]       //入力レイアウトオブジェクトの設定を行うための構造体
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    ShaderManager::Instance()->CreateVsFromCso(device, "Sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    ShaderManager::Instance()->CreatePsFromCso(device, "Sprite_ps.cso", pixel_shader.GetAddressOf());

    ID3D11Resource* resource{};
    //画像ファイルからリソースとシェーダーリソースビューを生成
    hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, shader_resource_view.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    resource->Release();

    ID3D11Texture2D* texture2D{};
    //リソースからテクスチャを生成
    hr = resource->QueryInterface<ID3D11Texture2D>(&texture2D);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //テクスチャからテクスチャ情報を取り出す
    texture2D->GetDesc(&texture2d_desc);
    texture2D->Release();
}

Sprite_batch::~Sprite_batch()
{

}

//シェーダーとテクスチャの設定
void Sprite_batch::Begin(ID3D11DeviceContext* immediate_context)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);
}

void Sprite_batch::End(ID3D11DeviceContext* immediate_context)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    size_t vertex_count = vertices.size();
    _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = vertices.data();
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }
    immediate_context->Unmap(vertex_buffer.Get(), 0);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->Draw(static_cast<UINT>(vertex_count), 0);

    6から
}

void Sprite_batch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle)
{
    //幅高さを0.0からが画像最大にしてオーバーロードしたほうのRenderを呼び出す
    Render(immediate_context,
        dx, dy,
        dw, dh,
        r, g, b, a,
        angle,
        0.0f, 0.0f,
        static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite_batch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh
)
{
    //スクリーン（ビューポート）のサイズを取得
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //  (x0, y0) *----* (x1, y1)             
    //           |   /|           
    //           |  / |
    //           | /  |
    //           |/   |
    //  (x2, y2) *----* (x3, y3)

    // left-top
    float x0{ dx };
    float y0{ dy };
    // right-top
    float x1{ dx + dw };
    float y1{ dy };
    // left-bottom
    float x2{ dx };
    float y2{ dy + dh };
    //right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };

    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を短径の中心点にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);
    //スクリーン座標系から NDC 座標系に変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //テクスチャ座標の設定
    float u0{ sx / texture2d_desc.Width };
    float v0{ sy / texture2d_desc.Height };
    float u1{ (sx + sw) / texture2d_desc.Width };
    float v1{ (sy + sh) / texture2d_desc.Height };

    //頂点データの設定
    vertices.push_back({ {x0,y0,0},{r,g,b,a},{u0,v0} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x3,y3,0},{r,g,b,a},{u1,v1} });
}