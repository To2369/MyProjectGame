#include "Sprite.h"
#include"../misc.h"
#include <sstream>
#include"Shader.h"
Sprite::Sprite(ID3D11Device* device)
{
    HRESULT hr{ S_OK };

    //頂点情報のセット
    vertex vertices[]
    {
        { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
        { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
        { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
        { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
    };

    //頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};                                                //頂点バッファオブジェクトの設定を行うための構造体
    buffer_desc.ByteWidth = sizeof(vertices);                                       //頂点情報のサイズを指定
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                                        //バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               //バッファがパイプラインにバインドされる方法を特定,このバッファオブジェクトを頂点バッファとして設定
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            //CPU へアクセスする際のフラグ
    buffer_desc.MiscFlags = 0;                                                      //その他のフラグ。不要な場合は０
    buffer_desc.StructureByteStride = 0;                                            //バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。

    //サブリソースデータ
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // 初期化データへのポインタ
    subresource_data.pSysMem = vertices;                                        //テクスチャの１行の先頭から次の行までの距離(バイト単位)。
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
    };
    
    ShaderManager::Instance()->CreateVsFromCso(device,"Sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    ShaderManager::Instance()->CreatePsFromCso(device, "Sprite_ps.cso", pixel_shader.GetAddressOf());


}

Sprite::~Sprite()
{

}

void Sprite::Render(ID3D11DeviceContext* immediate_context, 
    float dx, float dy,
    float dw, float dh,
    float r, float g,
    float b, float a)
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
    float y2{ dx + dh };
    //right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };

    // スクリーン座標系からNDCへの座標変換を行う
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1,1,1,1 };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    //頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(
        0,                              //頂点バッファの開始スロット
        1,                              //配列内の頂点バッファの数。
        vertex_buffer.GetAddressOf(),   //頂点バッファ
        &stride,                        //その頂点バッファーから使用される要素のサイズ
        &offset);                       //頂点バッファの最初の要素と使用される最初の要素の間のバイト数です

    //プリミティブタイプおよびデータの序列に関するバインド
    //プリミティブトポロジーの設定
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //入力レイアウトオブジェクトのバインド
    //入力レイアウトオブジェクトの設定
    immediate_context->IASetInputLayout(input_layout.Get());

    //シェーダーのバインド
    //頂点シェーダーオブジェクトの設定
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    //ピクセルシェーダーオブジェクトの設定
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    //immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    //プリミティブ描画
    //描画処理
    immediate_context->Draw(4, 0);
}