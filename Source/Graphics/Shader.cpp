#include "Shader.h"
#include"../Misc.h"
#include <sstream>
#include<WICTextureLoader.h>
#include<wrl.h>
#include<filesystem>
#include<DDSTextureLoader.h>
#include<string>
#include<map>

// 頂点シェーダーオブジェクトの生成
HRESULT ShaderManager::CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
    ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{    
    // ファイルを開く
    FILE* fp{};
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // ファイルのサイズを調べて
    fseek(fp, 0, SEEK_END);
    long csoSz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    // ファイル内の文字を cso_data に文字列として取り出し、ファイルを閉じる
    std::unique_ptr<unsigned char[]>csoData{ std::make_unique<unsigned char[]>(csoSz) };
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    if (inputLayout)
    {
        hr = device->CreateInputLayout(
            inputElementDesc,                 // 入力レイアウトの設定を行うための構造体 
            numElements,                       // 構造体の配列の数
            csoData.get(),                     // コンパイルされた頂点シェーダー文字列
            csoSz,                             // コンパイルされた頂点シェーダーのサイズ
            inputLayout);                      // 成功時の入力レイアウトオブジェクト
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

// ピクセルシェーダーオブジェクトの生成
HRESULT ShaderManager::CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
{
    // ファイルを開く
    FILE* fp{};
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "SCO File not found");

    // ファイルのサイズを調べて
    fseek(fp, 0, SEEK_END);
    long csoSz{ ftell(fp) };// コンパイルされた ピクセルシェーダのサイズ
    fseek(fp, 0, SEEK_SET);

    // ファイル内の文字を cso_data に文字列として取り出し、ファイルを閉じる
    std::unique_ptr<unsigned char[]>csoData{ std::make_unique<unsigned char[]>(csoSz) };
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };

    hr = device->CreatePixelShader(
        csoData.get(),         // コンパイルされたピクセルシェーダー文字列
        csoSz,                 // コンパイルされたピクセルシェーダーのサイズ
        nullptr,                // クラス リンケージ インターフェイスへのポインター,値はNULLにすること可能
        pixelShader);          // 成功時のピクセルシェーダー
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}

// 生成されたシェーダリソースビュー
// 生成したらここに保存していきファイル名で保存
// ２回目以降はここから取得する
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

// 外部ファイルからテクスチャを読み込む
HRESULT ShaderManager::LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView,
    D3D11_TEXTURE2D_DESC* texture2dDesc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    // 既にロードされたテクスチャを検索
    auto it = resources.find(filename);
    // 既にロードされていれば、それを使用し、そうでなければ新しくロード
    if (it != resources.end()) 
    {
        *shaderResourceView = it->second.Get();
        (*shaderResourceView)->AddRef();
        (*shaderResourceView)->GetResource(resource.GetAddressOf());
    }
    // 新しいテクスチャをロードする
    else
    {
        // DDSファイルが存在するかどうかチェック
        std::filesystem::path ddsFileneme(filename);
        ddsFileneme.replace_extension("dds");
        // DDSファイルが存在したらDDSテクスチャファイル読み込み
        if (std::filesystem::exists(ddsFileneme.c_str()))
        {
            hr = DirectX::CreateDDSTextureFromFile(device, ddsFileneme.c_str(), resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        // DDS ファイルが存在しなければ通常のテクスチャ読み込み
        else
        {
            // ファイルからテクスチャを作成し、shader_resource_viewに設定
            hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
            _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        }
        resources.insert(std::make_pair(filename, *shaderResourceView));
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    // リソースからテクスチャを生成
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    // テクスチャからテクスチャ情報を取り出す
    texture2d->GetDesc(texture2dDesc);

    return hr;
}

HRESULT ShaderManager::LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    hr = DirectX::CreateDDSTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
        size, resource.GetAddressOf(), shaderResourceView);
    if (hr != S_OK)
    {
        hr = DirectX::CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
            size, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    return hr;
}

// 全てのテクスチャを解放
void ShaderManager::ReleaseAllTextures()
{
    resources.clear();
}

HRESULT ShaderManager::MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView, DWORD value,
    UINT dimension)
{
    HRESULT hr{ S_OK };

    // ダミーのテクスチャ情報
    D3D11_TEXTURE2D_DESC texture2dDesc{};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // ダミーのテクセル情報を設定
    size_t texsels = dimension * dimension;
    std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texsels) };
    for (size_t i = 0; i < texsels; i++)
    {
        sysmem[i] = value;
    }

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    // ダミーの情報でテクスチャを作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // ダミーのテクスチャ情報でシェーダーリソースビューを作成
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(
        texture2d.Get(),
        &shaderResourceViewDesc,
        shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    return hr;
}