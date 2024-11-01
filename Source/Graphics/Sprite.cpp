#include "Sprite.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };
    // 頂点情報のセット
    vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0,0}},
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1,0}},
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0,1}},
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1,1}},
    };

    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};                                                // 頂点バッファオブジェクトの設定を行うための構造体
    buffer_desc.ByteWidth = sizeof(vertices);                                       // 頂点情報のサイズを指定
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                                        // バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               // バッファがパイプラインにバインドされる方法を特定,このバッファオブジェクトを頂点バッファとして設定
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            // CPU へアクセスする際のフラグ
    buffer_desc.MiscFlags = 0;                                                      // その他のフラグ。不要な場合は０
    buffer_desc.StructureByteStride = 0;                                            // バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。

    // サブリソースデータ
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // 初期化データへのポインタ
    subresource_data.pSysMem = vertices;                                        // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresource_data.SysMemPitch = 0;                                           // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresource_data.SysMemSlicePitch = 0;                                      // ある深度レベルの開始から次の深度レベルまでの距離(バイト単位)
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf()); // デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]       // 入力レイアウトオブジェクトの設定を行うための構造体
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // 頂点シェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\SpriteVS.cso",
            vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SpritePS.cso",
            pixel_shader.GetAddressOf());
    }

    // テクスチャの読み込み
    {
        if (filename!=nullptr)
        {
            ShaderManager::Instance()->LoadTextureFromFile(device, filename,
                shader_resource_view.GetAddressOf(), &texture2d_desc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, shader_resource_view.GetAddressOf(), 0xFFFFFFFF, 16);
        }
    }

    textureWidth = texture2d_desc.Width;
    textureHeight = texture2d_desc.Height;
}
Sprite::~Sprite()
{

}

void Sprite::Render(ID3D11DeviceContext* immediate_context, 
    float dx, float dy,
    float dw, float dh,
    float r, float g,float b, float a,
    float angle)
{
    // 幅高さを0.0からが画像最大にしてオーバーロードしたほうのRenderを呼び出す
    Render(immediate_context,
        dx, dy,
        dw, dh,
        r, g, b, a,
        angle,
        0.0f, 0.0f,
        static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh
)
{
    // スクリーン（ビューポート）のサイズを取得
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
    // 回転の中心を短径の中心点にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);
    // スクリーン座標系から NDC 座標系に変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(
        0,                              // 頂点バッファの開始スロット
        1,                              // 配列内の頂点バッファの数。
        vertex_buffer.GetAddressOf(),   // 頂点バッファ
        &stride,                        // その頂点バッファーから使用される要素のサイズ
        &offset);                       // 頂点バッファの最初の要素と使用される最初の要素の間のバイト数です

    // プリミティブタイプおよびデータの序列に関するバインド
    // プリミティブトポロジーの設定
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    // 入力レイアウトオブジェクトの設定
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    // 頂点シェーダーオブジェクトの設定
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    // ピクセルシェーダーオブジェクトの設定
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    // プリミティブ描画
    // 描画処理
    immediate_context->Draw(4, 0);
}


// シェーダーリソースビューの設定
void Sprite::SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight)
{
    shader_resource_view = srv;
    textureWidth = texWidth;
    textureHeight = texHeight;
}

// 画面に文字を出す
void Sprite::Textout(ID3D11DeviceContext* immediate_context, std::string s,
    float x, float y, float w, float h,
    float r, float g, float b, float a)
{
    // 一文字分の幅と高さを計算
    float sw = static_cast<float>(texture2d_desc.Width / 16);
    float sh = static_cast<float>(texture2d_desc.Height / 16);
    // 現在の文字位置(相対位置)
    float carriage = 0;

    // 文字数分だけrender()を呼び出す
    for (const char c : s)
    {
        LONG sx = c % 0x0F;
        // 文字を表示、アスキーコードの位置にある文字位置を切り抜いて表示
        Render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
        // 文字位置を幅分ずらす
        carriage += w;
    }
}