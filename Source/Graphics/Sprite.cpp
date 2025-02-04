#include "Sprite.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };
    // 頂点情報のセット
    Vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0,0}},
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1,0}},
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0,1}},
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1,1}},
    };

    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC bufferDesc{};                                                // 頂点バッファオブジェクトの設定を行うための構造体
    bufferDesc.ByteWidth = sizeof(vertices);                                       // 頂点情報のサイズを指定
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                                        // バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               // バッファがパイプラインにバインドされる方法を特定,このバッファオブジェクトを頂点バッファとして設定
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            // CPU へアクセスする際のフラグ
    bufferDesc.MiscFlags = 0;                                                      // その他のフラグ。不要な場合は０
    bufferDesc.StructureByteStride = 0;                                            // バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。

    // サブリソースデータ
    D3D11_SUBRESOURCE_DATA subresourceData{};
    // 初期化データへのポインタ
    subresourceData.pSysMem = vertices;                                        // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresourceData.SysMemPitch = 0;                                           // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresourceData.SysMemSlicePitch = 0;                                      // ある深度レベルの開始から次の深度レベルまでの距離(バイト単位)
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf()); // デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]       // 入力レイアウトオブジェクトの設定を行うための構造体
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
            vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }
   

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SpritePS.cso",
            pixelShader.GetAddressOf());
    }

    // テクスチャの読み込み
    {
        if (filename!=nullptr)
        {
            ShaderManager::Instance()->LoadTextureFromFile(device, filename,
                shaderResourceView.GetAddressOf(), &texture2dDesc);
        }
        else
        {
            ShaderManager::Instance()->MakeDummyTexture(device, shaderResourceView.GetAddressOf(), 0xFFFFFFFF, 16);
        }
    }

    textureWidth = texture2dDesc.Width;
    textureHeight = texture2dDesc.Height;
}
Sprite::~Sprite()
{

}

void Sprite::Render(ID3D11DeviceContext* dc, 
    float dx, float dy,
    float dw, float dh,
    float r, float g,float b, float a,
    float angle)
{
    // 幅高さを0.0からが画像最大にしてオーバーロードしたほうのRenderを呼び出す
    Render(dc,
        dx, dy,
        dw, dh,
        r, g, b, a,
        angle,
        0.0f, 0.0f,
        static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void Sprite::Render(ID3D11DeviceContext* dc,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh
)
{
    // スクリーン（ビューポート）のサイズを取得
    D3D11_VIEWPORT viewport{};
    UINT numViewports{ 1 };
    dc->RSGetViewports(&numViewports, &viewport);

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
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
    hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2dDesc.Width, sy / texture2dDesc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2dDesc.Width, sy / texture2dDesc.Height };
        vertices[2].texcoord = { sx / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2dDesc.Width, (sy + sh) / texture2dDesc.Height };
    }

    dc->Unmap(vertexBuffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(
        0,                              // 頂点バッファの開始スロット
        1,                              // 配列内の頂点バッファの数。
        vertexBuffer.GetAddressOf(),   // 頂点バッファ
        &stride,                        // その頂点バッファーから使用される要素のサイズ
        &offset);                       // 頂点バッファの最初の要素と使用される最初の要素の間のバイト数です

    // プリミティブタイプおよびデータの序列に関するバインド
    // プリミティブトポロジーの設定
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    // 入力レイアウトオブジェクトの設定
    dc->IASetInputLayout(inputLayout.Get());

    // シェーダーのバインド
    // 頂点シェーダーオブジェクトの設定
    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    // ピクセルシェーダーオブジェクトの設定
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);
    dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
    // プリミティブ描画
    // 描画処理
    dc->Draw(4, 0);
}

// シェーダーリソースビューの設定
void Sprite::SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight)
{
    shaderResourceView = srv;
    textureWidth = texWidth;
    textureHeight = texHeight;
}

// 画面に文字を出す
void Sprite::Textout(ID3D11DeviceContext* dc, std::string s,
    float x, float y, float w, float h,
    float r, float g, float b, float a)
{
    // 一文字分の幅と高さを計算
    float sw = static_cast<float>(texture2dDesc.Width / 16);
    float sh = static_cast<float>(texture2dDesc.Height / 16);
    // 現在の文字位置(相対位置)
    float carriage = 0;

    // 文字数分だけrender()を呼び出す
    for (const char c : s)
    {
        LONG sx = c % 0x0F;
        // 文字を表示、アスキーコードの位置にある文字位置を切り抜いて表示
        Render(dc, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
        // 文字位置を幅分ずらす
        carriage += w;
    }
}