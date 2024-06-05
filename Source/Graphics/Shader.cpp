#include "Shader.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
#include<wrl.h>
#include<filesystem>
#include<string>
#include<map>

//頂点シェーダーオブジェクトの生成
HRESULT ShaderManager::CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{    
    //ファイルを開く
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    //ファイルのサイズを調べて
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    //ファイル内の文字を cso_data に文字列として取り出し、ファイルを閉じる
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateInputLayout(
        input_element_desc,                 //入力レイアウトの設定を行うための構造体 
        num_elements,                       //構造体の配列の数
        cso_data.get(),                     //コンパイルされた頂点シェーダー文字列
        cso_sz,                             //コンパイルされた頂点シェーダーのサイズ
        input_layout);                     //成功時の入力レイアウトオブジェクト
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

//ピクセルシェーダーオブジェクトの生成
HRESULT ShaderManager::CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
    //ファイルを開く
    FILE* fp{};
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "SCO File not found");

    //ファイルのサイズを調べて
    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };//コンパイルされた ピクセルシェーダのサイズ
    fseek(fp, 0, SEEK_SET);

    //ファイル内の文字を cso_data に文字列として取り出し、ファイルを閉じる
    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };

    hr = device->CreatePixelShader(
        cso_data.get(),         //コンパイルされたピクセルシェーダー文字列
        cso_sz,                 //コンパイルされたピクセルシェーダーのサイズ
        nullptr,                //クラス リンケージ インターフェイスへのポインター,値はNULLにすること可能
        pixel_shader);         //成功時のピクセルシェーダー
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

//生成されたシェーダリソースビュー
//生成したらここに保存していきファイル名で保存
//２回目以降はここから取得する
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;

//外部ファイルからテクスチャを読み込む
HRESULT ShaderManager::LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename, ID3D11ShaderResourceView** shader_resource_view,
    D3D11_TEXTURE2D_DESC* texture2d_desc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    //既にロードされたテクスチャを検索
    auto it = resources.find(filename);
    //既にロードされていれば、それを使用し、そうでなければ新しくロード
    if (it != resources.end()) 
    {
        *shader_resource_view = it->second.Get();
        (*shader_resource_view)->AddRef();
        (*shader_resource_view)->GetResource(resource.GetAddressOf());
    }
    //新しいテクスチャをロードする
    else
    {
        //ファイルからテクスチャを作成し、shader_resource_viewに設定
        hr = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        resources.insert(std::make_pair(filename, *shader_resource_view));
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    //リソースからテクスチャを生成
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //テクスチャからテクスチャ情報を取り出す
    texture2d->GetDesc(texture2d_desc);

    return hr;
}

//全てのテクスチャを解放
void ShaderManager::release_all_textures()
{
    resources.clear();
}