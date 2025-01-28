#include "SpriteBatch.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t maxSprites) :maxVertices(maxSprites * 6)
{
    HRESULT hr{ S_OK };

    // 頂点情報のセット
    std::unique_ptr<Vertex[]> vertices{ std::make_unique<Vertex[]>(maxVertices) };
    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC bufferDesc{};                                                // 頂点バッファオブジェクトの設定を行うための構造体
    bufferDesc.ByteWidth = sizeof(Vertex) * maxVertices;                          // 頂点情報のサイズを指定
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                                        // バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               // バッファがパイプラインにバインドされる方法を特定,このバッファオブジェクトを頂点バッファとして設定
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            // CPU へアクセスする際のフラグ
    bufferDesc.MiscFlags = 0;                                                      // その他のフラグ。不要な場合は０
    bufferDesc.StructureByteStride = 0;                                            // バッファが構造化バッファを表す場合のバッファ構造内の各要素のサイズ。

    // サブリソースデータ
    D3D11_SUBRESOURCE_DATA subresourceData{};
    // 初期化データへのポインタ
    subresourceData.pSysMem = vertices.get();                                                  // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresourceData.SysMemPitch = 0;                                                           // テクスチャの１行の先頭から次の行までの距離(バイト単位)。
    subresourceData.SysMemSlicePitch = 0;                                                      // ある深度レベルの開始から次の深度レベルまでの距離(バイト単位)
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());   // デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
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
            vertexShader.GetAddressOf(), inputLayout.GetAddressOf(),
            inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // ピクセルシェーダーオブジェクトの生成
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SpritePS.cso",
            pixelShader.GetAddressOf());
    }
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\EffectPS.cso",
            replacedPixelShader.GetAddressOf());
    }
    // テクスチャの読み込み
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, filename,
            shaderResourceView.GetAddressOf(),
            &texture2dDesc);
    }
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, filename,
            replacedShaderResourceView.GetAddressOf(),
            &replacedTexture2dDesc);
    }
}

SpriteBatch::~SpriteBatch()
{

}

// シェーダーとテクスチャの設定
void SpriteBatch::Begin(ID3D11DeviceContext* dc,
    ID3D11PixelShader* replacedPixelShader,
    ID3D11ShaderResourceView* replacedShaderResourceView)
{
    vertices.clear();
    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    replacedPixelShader ? dc->PSSetShader(replacedPixelShader, nullptr, 0) : 
        dc->PSSetShader(pixelShader.Get(), nullptr, 0);
    if (replacedShaderResourceView)
    {
        // texture2d_descにさし変わったテクスチャ情報を設定
        HRESULT hr{ S_OK };
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        replacedShaderResourceView->GetResource(resource.GetAddressOf());
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
        hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        texture2d->GetDesc(&texture2dDesc);

        // 差し変わったテクスチャを設定
        dc->PSSetShaderResources(0, 1, &replacedShaderResourceView);
    }
    else
    {
        // 今までのテクスチャを設定
        dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
    }
}

void SpriteBatch::End(ID3D11DeviceContext* dc)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
    hr = dc->Map(vertexBuffer.Get(),
        0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    size_t vertexCount = vertices.size();
    _ASSERT_EXPR(maxVertices >= vertexCount, "Buffer overflow");
    Vertex* data{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
    if (data != nullptr)
    {
        const Vertex* p = vertices.data();
        memcpy_s(data, maxVertices * sizeof(Vertex), p, vertexCount * sizeof(Vertex));
    }
    dc->Unmap(vertexBuffer.Get(), 0);

    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());

    dc->Draw(static_cast<UINT>(vertexCount), 0);
}


void SpriteBatch::Render(ID3D11DeviceContext* dc,
    float dx, float dy,
    float dw, float dh
)
{
    // 幅高さを0.0からが画像最大にしてオーバーロードしたほうのRenderを呼び出す
    Render(dc,
        dx, dy,
        dw, dh,
        1, 1, 1, 1,
        0.0f,
        0.0f, 0.0f,
        static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height));
}

void SpriteBatch::Render(ID3D11DeviceContext* dc,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
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

void SpriteBatch::Render(ID3D11DeviceContext* dc,
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
    dc->RSGetViewports(&num_viewports, &viewport);

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

    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

    // 回転の中心を短径の中心点にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, sin, cos);
    rotate(x1, y1, cx, cy, sin, cos);
    rotate(x2, y2, cx, cy, sin, cos);
    rotate(x3, y3, cx, cy, sin, cos);

    // スクリーン座標系から NDC 座標系に変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // テクスチャ座標の設定
    float u0{ sx / texture2dDesc.Width };
    float v0{ sy / texture2dDesc.Height };
    float u1{ (sx + sw) / texture2dDesc.Width };
    float v1{ (sy + sh) / texture2dDesc.Height };

    // 頂点データの設定
    vertices.push_back({ {x0,y0,0},{r,g,b,a},{u0,v0} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x3,y3,0},{r,g,b,a},{u1,v1} });
}