
## <span style="color:#334488;">バウンディングボックス</span><a name="バウンディングボックス_"></a>

---

    演習手順４－１

衝突判定用のバウンディングボックス(AABB)を static_mesh に追加していきましょう。
まずはボックスとなる変数を宣言しましょう。

|実装するファイル|実装するクラス|
--|--
static_mesh.h|static_mesh

```cpp
        ・・・省略・・・
        std::vector<material> materials;

*       DirectX::XMFLOAT3 bounding_box[2]{ 
*           { D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX }, 
*           { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX } 
*       };
    private:
        ・・・省略・・・
```

左上奥の座標と右下手前の２点の座標があればバウンディングボックスは作成することができるので、この２点を格納するための FLOAT3 を２つ宣言しました。
設定している値にはひとまず float 値の最大値が代入されています。

---

宣言した変数に対して読み込んだ頂点の座標から x, y, z それぞれ最少と最大となる値を設定していきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.cpp**|**static_mesh::static_mesh(...)**


```cpp
                ・・・省略・・・
                make_dummy_texture(device, mat.shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 16);
            }
        }

*       // 読み込んだ頂点座標の x y z の最小、最大をそれぞれバウンディングボックスに設定する
*       for (const vertex& v : vertices)
*       {
*           bounding_box[0].x = std::min<float>(bounding_box[0].x, v.position.x);
*           bounding_box[0].y = std::min<float>(bounding_box[0].y, v.position.y);
*           bounding_box[0].z = std::min<float>(bounding_box[0].z, v.position.z);
*           bounding_box[1].x = std::max<float>(bounding_box[1].x, v.position.x);
*           bounding_box[1].y = std::max<float>(bounding_box[1].y, v.position.y);
*           bounding_box[1].z = std::max<float>(bounding_box[1].z, v.position.z);
*       }
    }
    ・・・省略(下に static_mesh::render(...)の定義)・・・
```

---

ひとまずバウンディングボックス用のメッシュの生成を static_mesh の要素数１番に行います。

この時、演習手順４－４にあるようにワイヤーフレーム表示にして境界部分のみを表示するようにしましょう。


|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    
    static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Rock\\Rock.obj", true);

*   /// バウンディングボックス用のメッシュ
*   static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj", false);

    ・・・省略・・・
```

---

描画部分で岩と岩を囲むように立方体を表示させます。
この時ラスタライザステートの設定を岩は通常通りソリッドで立方体はワイヤーフレームの両面描画で表示します。

framework::render(...) の以下の部分を

```cpp
// 拡大縮小行列
DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
// 回転行列
DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
// 平行移動行列
DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
// ワールド行列
DirectX::XMFLOAT4X4 world;
DirectX::XMStoreFloat4x4(&world, S* R* T);
// 描画
static_meshes[0]->render(immediate_context.Get(), world, material_color);
```

以下のように変更しよう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        // 設定したシーン定数をバッファをピクセルシェーダー側に登録
        immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*       immediate_context->RSSetState(rasterizer_states[0].Get());
*       {
*           // 拡大縮小行列
*           DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
*           // 回転行列
*           DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*           // 平行移動行列
*           DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
*           // ワールド行列
*           DirectX::XMFLOAT4X4 world;
*           DirectX::XMStoreFloat4x4(&world, S * R * T);
*           // 描画
*           static_meshes[0]->render(immediate_context.Get(), world, material_color);
*       }

*       /// <summary>
*       /// バウンディングボックスの描画
*       /// </summary>
*       immediate_context->RSSetState(rasterizer_states[3].Get());
*       {
*           // バウンディングボックスの大きさを更新
*           const DirectX::XMFLOAT3 min[2]{ static_meshes[0]->bounding_box[0], static_meshes[1]->bounding_box[0] };
*           const DirectX::XMFLOAT3 max[2]{ static_meshes[0]->bounding_box[1], static_meshes[1]->bounding_box[1] };
*           DirectX::XMFLOAT3 dimensions[2]
*           {
*               { max[0].x - min[0].x, max[0].y - min[0].y, max[0].z - min[0].z },
*               { max[1].x - min[1].x, max[1].y - min[1].y, max[1].z - min[1].z }
*           };
*           // バウンディングボックスの位置を更新
*           DirectX::XMFLOAT3 barycenters[2]
*           {
*               { (max[0].x + min[0].x) * 0.5f, (max[0].y + min[0].y) * 0.5f, (max[0].z + min[0].z) * 0.5f },
*               { (max[1].x + min[1].x) * 0.5f, (max[1].y + min[1].y) * 0.5f, (max[1].z + min[1].z) * 0.5f }
*           };
*           DirectX::XMFLOAT3 ratio{ dimensions[0].x / dimensions[1].x, dimensions[0].y / dimensions[1].y, dimensions[0].z / dimensions[1].z };
*           DirectX::XMFLOAT3 offset{ barycenters[0].x - barycenters[1].x, barycenters[0].y - barycenters[1].y, barycenters[0].z - barycenters[1].z };
*           DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(ratio.x * scaling.x, ratio.y * scaling.y, ratio.z * scaling.z) };
*           DirectX::XMMATRIX O{ DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z) };
*           DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
*           DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z) };
*           DirectX::XMFLOAT4X4 world;
*           DirectX::XMStoreFloat4x4(&world, S * O * R * T);
*           static_meshes[1]->render(immediate_context.Get(), world, { 1, 0, 0, 1 });
*       }

    #ifdef USE_IMGUI
        ・・・省略・・・
```

これで起動してもまだ何もバウンディングボックスの表示は行われません。
static_mesh のピクセルシェーダーにはフォンシェーディングの実装が行われている為、ワイヤーフレームだと表示されなくなっています。
ひとまず使用するピクセルシェーダーを geometric_primitive_ps.hlsl に変更し、static_mesh::render(...) 部分をピクセルシェーダーの切替ができるように拡張しましょう。

バウンディングボックス描画用のピクセルシェーダ用の変数を用意します。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    std::unique_ptr<static_mesh> static_meshes[8];

*   /// 通常メッシュとバウンディングボックスを描画時に表示切替するためピクセルシェーダも描画時に切り替える必要がある
*   Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];

    // 定数バッファフォーマット
    struct scene_constants
    ・・・省略・・・
```

---

宣言したら framework::initialize() で生成しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.h**|**framework::initialize()**

```cpp
    ・・・省略・・・
    static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj", false);
*   create_ps_from_cso(device.Get(), "geometric_primitive_ps.cso", pixel_shaders[0].GetAddressOf());
    ・・・省略・・・
```

---

次に生成したバウンディングボックス用のピクセルシェーダーを static_mesh::render(...) で受け取って切り替えれるように拡張します。

UNIT10 で sprite_batch の begin() に実装したピクセルシェーダーの切替を static_mesh の render(...) 関数に実装するだけです。

まずは static_mesh::render(...) の引数を追加しましょう。
以下の部分を

```cpp
/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">親のワールド行列</param>
/// <param name="material_color">マテリアルカラー</param>
void render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
```

以下のように変更しましょう。


|実装するファイル|実装するクラス|
--|--
**static_mesh.h**|**static_mesh**

```cpp
    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="world">親のワールド行列</param>
    /// <param name="material_color">マテリアルカラー</param>
*   /// <param name="alternative_pixel_shader">切替用ピクセルシェーダー</param>
    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color,
*       ID3D11PixelShader* alternative_pixel_shader = nullptr);
```

定義の以下の部分も

```cpp
/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">親のワールド行列</param>
/// <param name="material_color">マテリアルカラー</param>
void render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    ・・・省略・・・
}
```

以下のように変更しましょう。

|実装するファイル
--|--
**static_mesh.h**|

```cpp
    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="world">ワールド行列</param>
    /// <param name="material_color">マテリアルカラー</param>
*   /// <param name="alternative_pixel_shader">切替用ピクセルシェーダー</param>
    void static_mesh::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
        const DirectX::XMFLOAT4& material_color,
*       ID3D11PixelShader* alternative_pixel_shader)
    {
        ・・・省略・・・
    }
```

ピクセルシェーダーが受け取れるよう引数を追加したら、UNIT10 で行った切替用の実装をここで実装します。

ピクセルシェーダーを設定している以下の部分を

```cpp
immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
```

以下のように変更して下さい。

|実装するファイル|実装するメンバ関数|
--|--
**static_mesh.h**|**static_mesh::render(...)**

```cpp
*	// 切替用のピクセルシェーダーがあったらそちらを設定
*	alternative_pixel_shader ? immediate_context->PSSetShader(alternative_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
```

---

static_mesh の render() の引数を追加したので、呼び出し側でバウンディングボックス用のピクセルシェーダを設定しましょう。

framework::render() の以下の部分を

```cpp
static_meshes[1]->render(immediate_context.Get(), world, { 1, 0, 0, 1 });
```

以下のように変更しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework.h**|**framework::render(...)**

```cpp
static_meshes[1]->render(immediate_context.Get(), world, { 1, 0, 0, 1 }, pixel_shaders[0].Get());
```

実行すると以下のような画像が表示されます。

@import "images/バウンディングボックス03.png"