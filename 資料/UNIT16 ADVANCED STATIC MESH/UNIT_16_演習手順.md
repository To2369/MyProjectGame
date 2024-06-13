<!--
UNIT16_EXERCISE UNIT16 演習手順
ADVANCED STATIC MESH

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Phong shading
Specular relection
Bump map
Normal mapping
Dummy texture
Bounding box
-->

# UNIT16_EXCERCISE ADVANCED STATIC MESH 演習手順

* 目次
    1. [鏡面反射(スペキュラー)](#鏡面反射_)
        1. [読み込むファイル名を変更](#読み込むファイル名を変更_)
        2. [定数バッファにカメラ位置を追加](#定数バッファにカメラ位置を追加_)
        3. [カメラ位置をピクセルシェーダーにバインド](#カメラ位置をピクセルシェーダーにバインド_)
        5. [シェーダーヘッダに位置と法線とカメラ位置を追加](#シェーダーヘッダに位置と法線とカメラ位置を追加_)
        6. [頂点シェーダーを修正](#頂点シェーダーを修正_)
        7. [ピクセルシェーダーに鏡面反射を実装](#ピクセルシェーダーに鏡面反射を実装_)
        8. [鏡面反射の確認](#鏡面反射の確認_)
    2. [バンプマップ](#バンプマップ_)
    3. [ダミーテクスチャ](#ダミーテクスチャ_)
    4. [バウンディングボックス](#バウンディングボックス_)

---

#### <span style="color:#334488;">鏡面反射(スペキュラー)</span><a name="鏡面反射_"></a>

---

#### <span style="color:#334488;">読み込むファイル名を変更</span><a name="読み込むファイル名を変更_"></a>

    演習手順１－１

ピクセルシェーダーで**フォンシェーディング**の**スペキュラー**を表現してみましょう。
つまり**フォン反射モデル**の１つである**鏡面反射**をシェーダーで実装してみましょう。

まずはスペキュラーを実装した際に違いがわかりやすいモデルに変更します。

framework::initialize() にある以下の static_mesh の生成部分を

```cpp
static_meshes[0] = std::make_unique<static_mesh>(device.Get(),L".\\resources\\Mr.Incredible\\Mr.Incredible.obj", true);
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Rock\\Rock.obj", true);
```

---

#### <span style="color:#334488;">定数バッファにカメラ位置を追加</span><a name="定数バッファにカメラ位置を追加_"></a>

    演習手順１－２

鏡面反射の計算にはカメラの計算が必要になります。計算を行っているシェーダ側にカメラの情報を送る必要があります。
ということで、シーン定数バッファにカメラの情報を追加しましょう。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework::scene_constants**

```cpp
    DirectX::XMFLOAT4X4 view_projection;	// ビュー・プロジェクション変換行列
    DirectX::XMFLOAT4 light_direction;		// ライトの向き
*   DirectX::XMFLOAT4 camera_position;		// カメラ位置
```

---

#### <span style="color:#334488;">カメラ位置をピクセルシェーダーにバインド</span><a name="カメラ位置をピクセルシェーダーにバインド_"></a>

    演習手順１－３

宣言した変数に対してカメラ位置を代入し、ピクセルシェーダー側に設定しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    ・・・省略・・・

    scene_constants data{};
    // ビュー行列とプロジェクション行列を掛け合わせる(乗算)
    XMStoreFloat4x4(&data.view_projection, V * P);
    // 光の当たる方向を設定
    data.light_direction = light_direction;
*   // カメラ位置を設定
*   data.camera_position = camera_position;
    // ビュー・プロジェクション行列と光の当たる方向をシーン定数バッファとして設定
    immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
    // 設定したシーン定数バッファを GPU 側に登録
    immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
*   // 設定したシーン定数をバッファをピクセルシェーダー側に登録
*   immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

    ・・・省略・・・
```

---

#### <span style="color:#334488;">シェーダーヘッダに位置と法線とカメラ位置を追加</span><a name="シェーダーヘッダに位置と法線とカメラ位置を追加_"></a>

    演習手順１－５

鏡面反射の計算をピクセルシェーダ側で行うにあたって頂点シェーダ側から**位置**と**法線**が必要になるので出力に位置と法線を追加します。


|実装するファイル|実装する構造体|
--|--
**static_mesh.hlsli**|**VS_OUT**

```hlsl
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
*   float4 world_position : POSITION;
*   float4 world_normal : NORMAL;
```

加えて、先ほどシーン定数バッファにカメラ位置を追加したので、シェーダーヘッダ側のシーン定数バッファにもカメラ位置を追加しましょう。

|実装するファイル|実装する構造体|
--|--
**static_mesh.hlsli**|**SCENE_CONSTANT_BUFFER**

```hlsl
    row_major float4x4 view_projection;
    float4 light_direction;
*   float4 camera_position;
```

---

#### <span style="color:#334488;">頂点シェーダーを修正</span><a name="頂点シェーダーを修正_"></a>

    演習手順１－６

頂点シェーダーのコード部分を鏡面反射を計算できるように変更していきましょう。
鏡面反射の計算はピクセルシェーダー側で行います。
必よな位置と法線を出力にいれピクセルシェーダ側に渡してあげましょう。

法線計算が行われている以下の部分を

```cpp
normal.w = 0;
float4 N = normalize(mul(normal, world));
float4 L = normalize(-light_direction);

vout.color.rgb = material_color.rgb * max(0, dot(L, N));
vout.color.a = material_color.a;
```

以下のように変更してください。


|実装するファイル|実装する関数|
--|--
**static_mesh_vs.hlsl**|**main(...)**

```cpp
vout.world_position = mul(position, world);
normal.w = 0;
vout.world_normal = normalize(mul(normal, world));

vout.color = material_color;
```

---

#### <span style="color:#334488;">ピクセルシェーダーに鏡面反射を実装</span><a name="ピクセルシェーダーに鏡面反射を実装_"></a>

    演習手順１－７

ピクセルシェーダー側の処理になります。
準備は出来ているので、鏡面反射の計算をしましょう。

ピクセルシェーダーの以下の部分を

```hlsl
return color_map.Sample(anisotropic_sampler_state, pin.texcoord) * pin.color;
```

以下のように変更しましょう。

|実装するファイル|実装する関数|
--|--
**static_mesh_ps.mlsl**|**main(...)**

```hlsl
float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
float alpha = color.a;
float3 N = normalize(pin.world_normal.xyz);

float3 L = normalize(-light_direction.xyz);
float3 diffuse = color.rgb * max(0, dot(N, L));

float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);

return float4(diffuse + specular, alpha) * pin.color;
```

---

#### <span style="color:#334488;">鏡面反射の確認</span><a name="鏡面反射の確認_"></a>

    演習手順１－８

ビルドして確認。

@import "images/スペキュラ01.png"

確認し終わったら以下のコードの **128** を別の値に変更して実行し、変化を確認してみましょう。

```cpp
float3 specular = pow(max(0, dot(N, normalize(V + L))), 128);
```

---

#### <span style="color:#334488;">バンプマップ</span><a name="バンプマップ_"></a>

---

#### <span style="color:#334488;">鏡面反射(スペキュラー)</span><a name="鏡面反射_"></a>

    演習手順２－１

次はバンプマップを使えるように実装を行っていきましょう。

**static_mesh::material** クラスの以下の部分を

```
// テクスチャファイル名
std::wstring texture_filename;
// テクスチャの情報
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
```

バンプマップ用のテクスチャも読み込めるよう以下のように配列に変更します。

|実装するファイル|実装するクラス|
--|--
**static_mesh.h**|**static_mesh::material**

```cpp
// テクスチャファイル名
std::wstring texture_filenames[2];
// テクスチャの情報
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
```

変数名も少し変えているので注意して下さい。

---

    演習手順２－２

要素の０番に今まで通り通常のテクスチャの情報を、要素の１番にバンプマップ用のテクスチャの情報を代入します。

変数名を変更し配列にしたことで cpp 側でエラーが出ているかと思いますが、その部分も含めて変更して行きましょう。

static_mesh のコンストラクタの以下の部分を

```cpp
materials.rbegin()->texture_filename = path;
```

以下のように変更してください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
materials.rbegin()->texture_filenames[0] = path;
```

次にバンプマップの情報の読み込み部分を実装していきます。
バンプマップの情報が記載されている頭のキーワードは **map_bump** もしくは **bump** のどちらかです。
なのでこの２つでチェックをかけます。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
        ・・・省略・・・
        materials.rbegin()->texture_filenames[0] = path;
        fin.ignore(1024, L'\n');
    }
*   else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
*   {
*       fin.ignore();

*       // バンプマップ用のテクスチャファイル名の取得
*       wchar_t map_bump[256];
*       fin >> map_bump;
*       std::filesystem::path path(obj_filename);
*       path.replace_filename(std::filesystem::path(map_bump).filename());
*       materials.rbegin()->texture_filenames[1] = path;
*       fin.ignore(1024, L'\n');
*   }
    else if (0 == wcscmp(command, L"newmtl"))
    {
        fin.ignore();
        ・・・省略・・・
```

---

    演習手順２－３

通常のテクスチャとバンプマップ用のテクスチャのファイル名を読み込めたので次はこれらのテクスチャの生成を行います。

以下の部分でエラーが出ていると思います。

```cpp
load_texture_from_file(device, mat.texture_filename.c_str(),
    mat.shader_resource_view.GetAddressOf(), &texture2d_desc);
```

要素の０番にテクスチャの生成を、要素の１番にバンプマップ用のテクスチャを生成しましょう。

上の部分を以下のように変更してください。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
if (mat.texture_filenames[0].size() > 0)
    load_texture_from_file(device, mat.texture_filenames[0].c_str(),
        mat.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
```

**マテリアルにテクスチャのファイル名が設定されていたらテクスチャを生成する**、という事をしています。

現状、要素の０番である通常のテクスチャの生成のみが記載されています。

要素の１番であるバンプマップ用のテクスチャを生成するにはどのようにすれば良いでしょうか？
ほぼほぼ同じで通常のテクスチャの生成の下にそのまま実装するだけなので実装してみてください。

    課題１
        バンプマップ用のテクスチャの生成を実装して下さい。

<!--
答え>
if (mat.texture_filenames[1].size() > 0)
    load_texture_from_file(device, mat.texture_filenames[1].c_str(),
        mat.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
-->

---

    演習手順２－４

シェーダーリソースビューも変数名を変更し配列にしたので以下の部分でエラーが出ています。

```cpp
immediate_context->PSSetShaderResources(0, 1, mat.shader_resource_view.GetAddressOf());
```

この部分を通常のテクスチャを **t0** に登録し、バンプマップ用のテクスチャを **t1** に登録してシェーダー側で利用できるようにしましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::render(...)**

```cpp
    // テクスチャの情報をシェーダー側に設定
*   immediate_context->PSSetShaderResources(0, 1, mat.shader_resource_views[0].GetAddressOf());
*   // バンプマップ用のテクスチャをシェーダー側に設定
*   immediate_context->PSSetShaderResources(1, 1, mat.shader_resource_views[1].GetAddressOf());
```

**PSSetShaderResources(...)** の第１引数に設定する値に注意して下さい。この値で **t0** **t1** どちらに登録されるかが決定されます。

---

    演習手順２－５

演習手順２－４で登録した **t1** のバンプマップ用テクスチャをピクセルシェーダ側に定義し、バンプマップ用の法線マッピングの処理を実装しましょう。

|実装するファイル|
--|--
**static_mesh_ps.hlsl**|

```hlsl
    #include "static_mesh.hlsli"
    Texture2D color_map : register(t0);
*   Texture2D normal_map : register(t1);
    SamplerState point_sampler_state : register(s0);
    ・・・省略・・・

    float4 main(VS_OUT pin) : SV_TARGET
    {
        ・・・省略・・・
        float3 N = normalize(pin.world_normal.xyz);

*       float3 T = float3(1.0001, 0, 0);
*       float3 B = normalize(cross(N, T));
*       T = normalize(cross(B, N));

*       float4 normal = normal_map.Sample(linear_sampler_state, pin.texcoord);
*       normal = (normal * 2.0) - 1.0;
*       normal.w = 0;
*       N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

        float3 L = normalize(-light_direction.xyz);
        ・・・省略・・・
```

---

    演習手順２－６

実行して確認してみましょう。

@import "images/バンプマップ02.png"

---

    演習手順３－１

現状マテリアルファイルを持っていない OBJ ファイルを読み込もうとするとプログラムが停止にしてしまいます。
これらのファイルを読み込めるようにダミーのテクスチャを作りダミーのマテリアルを追加して起動できるようにしましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
*   //_ASSERT_EXPR(fin, L"'OBJ file not found.");
```

---

    演習手順３－２

マテリアルファイルがないのでマテリアルの情報が０になる
マテリアルのファイルがないというだけでマテリアルのファイル名とマテリアル名は OBJ ファイル側に設定されているので、ひとまずサブセットに登録したマテリアル名でダミーのマテリアルを設定しておく。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    ・・・省略・・・
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

*   if (materials.size() == 0)
*   {
*       for (const subset& s : subsets)
*       {
*           materials.push_back({ s.usemtl });
*       }
*   }

    // 読み込んだテクスチャを生成する
    D3D11_TEXTURE2D_DESC texture2d_desc{};

    ・・・省略・・・
```

---

    演習手順３－３

次はこのマテリアルにテクスチャを登録します。
描画の際にピクセルシェーダ側でシェーダーリソースビューを利用しているのでテクスチャの情報が無いとエラーになります。
ということでダミーのテクスチャを作成してダミーのマテリアルに登録します。
まずはダミーのテクスチャを texture.h と texture.cpp に実装しましょう。

|実装するファイル|
--|--
**texture.h**|

```cpp

    ・・・省略・・・
    void release_all_textures();

*   /// <summary>
*   /// ダミーのテクスチャを作成
*   /// </summary>
*   /// <param name="device">デバイス</param>
*   /// <param name="shader_resource_view">生成されたテクスチャ</param>
*   /// <param name="value">色</param>
*   /// <param name="dimension">サイズ</param>
*   /// <returns></returns>
*   HRESULT make_dummy_texture(
*       ID3D11Device* device, 
*       ID3D11ShaderResourceView** shader_resource_view, 
*       DWORD value, 
*       UINT dimension);
```

定義を実装しましょう。

|実装するファイル|
--|--
**texture.cpp**|

```cpp
・・・省略(上に release_all_textures() の定義)・・・

/// <summary>
/// ダミーのテクスチャを作成
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="shader_resource_view">生成されたテクスチャ</param>
/// <param name="value">色</param>
/// <param name="dimension">サイズ</param>
/// <returns></returns>
HRESULT make_dummy_texture(
    ID3D11Device* device,
    ID3D11ShaderResourceView** shader_resource_view, 
    DWORD value, 
    UINT dimension)
{
    HRESULT hr{ S_OK };

    // ダミーのテクスチャ情報を設定
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    texture2d_desc.Width = dimension;
    texture2d_desc.Height = dimension;
    texture2d_desc.MipLevels = 1;
    texture2d_desc.ArraySize = 1;
    texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2d_desc.SampleDesc.Count = 1;
    texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // ダミーのテクセル情報を設定
    size_t texels = dimension * dimension;
    unique_ptr<DWORD[]> sysmem{ make_unique<DWORD[]>(texels) };
    for (size_t i = 0; i < texels; ++i)
        sysmem[i] = value;

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = sysmem.get();
    subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

    // ダミーの情報でテクスチャを作成
    ComPtr<ID3D11Texture2D> texture2d;
    hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // ダミーのテクスチャ情報でシェーダーリソースビューを作成
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = texture2d_desc.Format;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_resource_view_desc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(
        texture2d.Get(),
        &shader_resource_view_desc,
        shader_resource_view);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}
```

---

    演習手順３－４

作成ダミーテクスチャ作成関数を使ってダミーのカラーテクスチャを実際に作ってみましょう。
読み込んだマテリアルからテクスチャとバンプマップに利用される法線マップ用のテクスチャを読み込んでいる以下の部分を

```cpp
for (material& mat : materials)
{
    if (mat.texture_filenames[0].size() > 0)
        load_texture_from_file(device, mat.texture_filenames[0].c_str(),
            mat.shader_resource_views[0].GetAddressOf(), &texture2d_desc);

    if (mat.texture_filenames[1].size() > 0)
        load_texture_from_file(device, mat.texture_filenames[1].c_str(),
            mat.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
}
```

テクスチャファイル名が無かった場合の処理としてダミーのテクスチャを生成していくように実装しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**

```cpp
    for (material& mat : mateしておこう。
    {
*       if (mat.texture_filenames[0].size() > 0)
*       {
            load_texture_from_file(device, mat.texture_filenames[0].c_str(),
                mat.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
*       }
*       else
*       {
*           // ダミー用のカラーマップテクスチャを生成し設定する
*           make_dummy_texture(device, mat.shader_resource_views[0].GetAddressOf(),
*           0xFFFFFFFF, 16);
*       }

*       if (mat.texture_filenames[1].size() > 0)
*       {
            load_texture_from_file(device, mat.texture_filenames[1].c_str(),
                mat.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
*       }
*       else
*       {
*           // ダミー用の法線マップテクスチャを生成し設定する
*           make_dummy_texture(device, mat.shader_resource_views[1].GetAddressOf(),
*           0xFFFF7F7F, 16);
*       }
	
```

カラーマップテクスチャと法線マップテクスチャに設定している色が違うので注意しておこう。

何も表示されない場合は以下の部分を確認してみよう。
framework.cpp のサンプラーステートの部分が D3D11_TEXTURE_ADDRESS_WRAP になっているかどうか確認しておこう。

```cpp
sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
```

D3D11_TEXTURE_ADDRESS_BORDER になっているとテクスチャが繰り返されないため、ダミーで設定した色が繰り返されることなく、結果表示が行われていないように見えてしまします。

---

#### <span style="color:#334488;">バウンディングボックス</span><a name="バウンディングボックス_"></a>

    演習手順４

演習手順の順序に従いバウンディングボックスを完成させてみてください。    