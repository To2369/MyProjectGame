<!--
UNIT09_EXERCISE UNIT11 演習手順
SPRITE - BATCHING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Index buffers
Constant buffers
World, View and Projection transformation matrices
Scale, Rotate and Translate transformation matrices
-->

# UNIT11_EXERCISE GEOMETRIC PRIMITIVE 演習手順

* 目次
    1. 幾何プリミティブを生成・描画するクラスを作成する
    2. シェーダーファイルの追加
        1. [HLSL ヘッダーファイル](#幾何プリミティブ用HLSLヘッダーファイルの作成_)
        2. [頂点シェーダーファイル](#幾何プリミティブ用頂点シェーダーファイルの作成_)
        3. [ピクセルシェーダーファイル](#幾何プリミティブ用ピクセルシェーダーファイルの作成_)
    3. [幾何プリミティブクラスを定義](#幾何プリミティブ用クラスを作成_)
    4. [幾何プリミティブクラスのコンストラクタの実装](#幾何プリミティブクラスのコンストラクタ部分を実装_)
    5. [頂点バッファオブジェクトの作成](#頂点バッファオブジェクトの作成_)
    6. [幾何プリミティブクラスの render メンバ関数の実装](#幾何プリミティブクラスのrenderメンバ関数の実装_)
    7. シーン定数バッファ
        1. [framework クラスに定義](#frameworkクラスに定義_)
        2. [シーン定数バッファオブジェクトを生成](#シーン定数バッファオブジェクトを生成_)
        3. [シーン定数バッファに値をセット](#シーン定数バッファに値をセット_)
    8. [幾何プリミティブポインタ型の配列を宣言](#幾何プリミティブポインタ型の配列を宣言_)
    9. [幾何プリミティブオブジェクトを生成](#幾何プリミティブオブジェクトを生成_)
    10. framework クラスの render メンバ関数で幾何プリミティブクラスの render メンバ関数を呼び出す
        1. [拡大縮小（S）・回転（R）・平行移動（T）行列を計算する](#拡大回転平行移動行列を計算する_)
        2. [上記３行列を合成しワールド変換行列を作成する](#上記３行列を合成しワールド変換行列を作成する_)
        3. [幾何プリミティブクラスの render メンバ関数を呼び出す](#幾何プリミティブクラスのrenderメンバ関数を呼び出す_)
    11. [実行し、正立方体が描画される事を確認する](#正立方体が描画される事を確認する_)
    12. [カメラの位置・ライトの照射方向を ImGUI を使って実行時に変更できるようにする](#カメラの位置・ライトの照射方向をImGUIを使って実行時に変更できるようにする_)
    13. [正立方体の位置・姿勢・寸法・色を ImGUI を使って実行時に変更できるようにする](#正立方体の位置・姿勢・寸法・色をImGUIを使って実行時に変更できるようにする_)

---

#### <span style="color:#334488;">HLSL ヘッダーファイル</span><a name="幾何プリミティブ用HLSLヘッダーファイルの作成_"></a>

幾何プリミティブ用の HLSL ヘッダーファイルを作成します。

    演習手２－１

シェーダー側の HLSL ヘッダーファイル **geometric_primitive.hlsil** を幾何プリミティブ用に新規で作成します。

頂点フォーマットとして用意するのは以前と同じで「位置」と「色」です。

そのほかにはオブジェクトの **定数バッファ(コンスタントバッファ)** として **ワールド行列** と **マテリアルカラー**。
シーンの **定数バッファ(コンスタントバッファ)** として **ビューとプロジェクションの行列** と **ライトの向き**。
この２つを宣言しています。

|実装するファイル|
--|--
**geometric_primitive.hlsli**|

```hlsl
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_porojection;
    float4 light_direction;
}
```

---

#### <span style="color:#334488;">頂点シェーダーファイル</span><a name="幾何プリミティブ用頂点シェーダーファイルの作成_"></a>

幾何プリミティブ用の頂点シェーダーファイルを作成します。

    演習手順２－２

シェーダー側の頂点シェーダーファイル **geometric_primitive_vs.hlsl** を幾何プリミティブ用に新規で作成します。

今回から 3D の表示を行います。なので 3D の座標を扱っています。
ですが、ディスプレイというものは 2D でしかないので、結局の所、画面に表示される頂点の位置というのは 2D です。

#### ではどのように 3D というものを表現しているのか？

まずは大きさ。奥行となる軸をふやし **投資投影(XMMatrixPerspectiveFovLH)** を使って近いものは大きく遠いものは小さく表示することで疑似的に 3D 感を出しています。

もう１つは**シェーディング**です。

@import "images/シェーディング.png"

つまりすべて 2D 空間上に疑似的な 3D を表示しているというだけで本物の 3D が表示されているわけではないという事を理解してください。 

扱う座標は 3D の座標である **ワールド座標(3D)** というものになりますが、結局は**ピクセル**として表示しなければならないので最終的には **スクリーン座標(2D)** が必要になります。つまり**ワールド座標**から**スクリーン座標**への変換が必要になるということです。以下が変換の行程になります。

@import "images/ワールド座標からスクリーン座標へ.png"

正規化座標とは NDC の事です。

まずは頂点シェーダー部分の実装をやっていきましょう。

|実装するファイル|
--|--
**geometric_primitive_vs.hlsl**|

```hlsl
#inclue "geometric_primitive.hlsli"
VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)
{
    VS_OUT vout;
    vout.position = mul(position, mul(world, view_projection));

    normal.w = 0 ;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_direction);

    vout.color.rgb = material_color.rgb * max(0, dot(L, N));
    vout.color.a = material_color.a;
    return vout; 
}
```

シェーダーの授業自体は別にありますがこの部分は重要な部分なので１つ１つ見ていきましょう。

まずはこの部分

    vout.position = mul(position, mul(world, view_projection));

ここは **ワールド座標** を **スクリーン座標** へと変換している部分です。

次の行では

    normal.w = 0 ;
    float4 N = normalize(mul(normal, world));
    float4 L = normalize(-light_direction);

シェーディングの為の光の設定を行っています。
**float4 N** が法線を正規化した値で、**float4 L** が光の方向の逆の方向を正規化した値です。

その次の行で正規化した値の内積を求め、その値に対して、rgb の色を乗算しています。

    vout.color.rgb = material_color.rgb * max(0, dot(L, N));

**光の方向の逆の方向**と**法線の方向**の内積を求める事で L と N の間の角度(ラジアン角)が求まります。
**dot(L, N)** の部分です。下の図であれば、**太陽からの光の向き**と**点からの法線**が完全に一致するのでその内積の値は 1.0f という結果になり、 

@import "images/内積計算1.png"

    material_color.rgb * max(0, 1.0f);

となって、**max()** 関数で大きい値が選択され

    marerial_color.rgb * 1.0f;

となり、rgb の値がそのまま表示されるという結果になります。
光が真正面からあたっているので rgb がそのまま表示されるという事です。

対して以下の画像の場合だと

@import "images/内積計算2.png"

計算結果は、

    material_color.rgb * max(0, 0.5f);

となり、多少薄暗くなります。

下の画像だと

@import "images/内積計算3.png"

計算結果は、

    material_color.rgb * max(0, 0.0f);

となり暗くなります。

裏側になるとマイナス値になるので

    material_color.rgb * max(0, -0.5f);

**max()** 関数でマイナス以下は 0 となります。

といった処理になっています。

最後の行で

    vout.color.a = material_color.a;

アルファ値である透明度の値を設定しています。

このように頂点シェーダー部分で疑似的な 3D 表現であるシェーディングの計算を行っています。

頂点シェーダーで 3D であった座標が **NDC** から **スクリーン座標** に計算され 2D として表示できる処理が行われ、**ラスタライズ**の処理をして次のピクセルシェーダーの処理にいたる、という順序になります。

---

#### <span style="color:#334488;">ピクセルシェーダーファイル</span><a name="幾何プリミティブ用ピクセルシェーダーファイルの作成_"></a>

幾何プリミティブ用のピクセルシェーダーファイルを新規で作成します。

    演習手順２－３

次にピクセルシェーダーファイルを作成していきます。

こちらもピクセルシェーダーファイル **geometric_primitive_ps.hlsl** を幾何プリミティブ用に新規で作成します。

|実装するファイル|
--|--
geometric_primitive_ps.hlsl|

```hlsl
#include "geometric_primitive.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    return pin.color;
}
```

ピクセルシェーダー部分は何もしていません。
色を編集せずに元の色のままを表示しています。

---

#### <span style="color:#334488;">幾何プリミティブクラスを定義</span><a name="幾何プリミティブ用クラスを作成_"></a>

    演習手順３

シェーダー部分が完成したら幾何プリミティブ用のクラスを実装していきます。
新規で **geometric_primitive.h** を作成してください。

|実装するファイル|
--|--
**geometric_primitive.h**|

```cpp
・・・省略(この上にヘッダのインクルードが必要です、 sprite.h のインクルードを調べて持って来てみて下さい)・・・

/// <summary>
/// 幾何プリミティブ
/// </summary>
class geometric_primitive
{
public:
    // 頂点フォーマット
    struct vertex
    {
        // 位置
        DirectX::XMFLOAT3 position;
        // 法線
        DirectX::XMFLOAT3 normal;
    };
    // 定数バッファ用フォーマット
    struct constants
    {
        // ワールド行列
        DirectX::XMFLOAT4X4 world;
        // マテリアルカラー
        DirectX::XMFLOAT4 material_color;
    };

private:
    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    // インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    // 頂点シェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    // ピクセルシェーダーオブジェクト
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    // 入力レイアウトオブジェクト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    // 定数バッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

public:
    geometric_primitive(ID3D11Device* device);
    virtual ~geometric_primitive() = default;

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="world">ワールド行列</param>
    /// <param name="material_color">マテリアルカラー</param>
    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);

protected:
    /// <summary>
    /// 頂点バッファオブジェクトの作成
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="vertices">頂点情報</param>
    /// <param name="vertex_count">頂点数</param>
    /// <param name="indices">インデックス情報</param>
    /// <param name="index_count">インデックス数</param>
    void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);
};
```

ヘッダファイルはあえて記載していません。
何が必要かは sprite.h で何をインクルードしているかを調べて持ってきてみて下さい。

---

#### <span style="color:#334488;">幾何プリミティブクラスのコンストラクタを実装</span><a name="幾何プリミティブクラスのコンストラクタ部分を実装_"></a>

    演習手順４

新規で **geometric_primitive.cpp** を作成してください。

|実装するファイル|
--|--
**geometric_primitive.cpp**|

```cpp
・・・省略(この上に必要なヘッダをインクルードしてみて下さい)・・・

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="device">デバイス</param>
geometric_primitive::geometric_primitive(ID3D11Device* device)
{
    /// <summary>
    /// サイズが 1.0 の正立方体データを作成する（重心を原点にする）。
    /// 正立方体のコントロールポイント数は 8 個、
    ///  1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
    /// 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。
    /// </summary>
    vertex vertices[24]{};

    /// <summary>
    ///  正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので
    /// 3 角形ポリゴンの総数は 6x2=12 個、
    /// 正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
    ///  3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
    ///  時計回りが表面になるように格納すること。
    /// </summary>
    uint32_t indices[36]{};

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

    // 頂点データとインデックスデータを使って頂点バッファオブジェクトを作成します
    create_com_buffers(device, vertices, 24, indices, 36);

    HRESULT hr{ S_OK };

    // 入力レイアウトオブジェクトの設定
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    // 頂点シェーダーオブジェクトの作成(入力レイアウトもこの中で作成しています。)
    create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
        input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    // ピクセルシェーダーオブジェクトの作成
    create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

    // 定数(コンスタント)バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

頂点バッファの作成は **create_com_buffers(...)** で行われます。

その下の処理の入力レイアウトの設定として新たに法線を設定しています。

```
D3D11_INPUT_ELEMENT_DESC input_element_desc[]
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
```

入力レイアウトオブジェクトの生成は **create_vs_from_cso(...)** で行われています。

その下の処理の定数バッファの設定の部分を見てみましょう。
ここは頂点バッファオブジェクトを生成する際と同じ構造体である **D3D11_BUFFER_DESC** を利用しています。
ですがバインドしているフラグが以下のようになっています。

    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

この設定を行って **CreateBuffer(...)** を行うことで **constant_buffer** を定数バッファとして扱う事ができるようになります。


---

#### <span style="color:#334488;">頂点バッファオブジェクトの作成</span><a name="頂点バッファオブジェクトの作成_"></a>

    演習手順５

頂点座標と、頂点インデックスの情報を元に頂点バッファオブジェクトの作成を行います。
次の UNIT12 以降の項目で様々な形の幾何プリミティブを作成していくので、この部分の処理をメンバ関数として切り離しています。

|実装するファイル|
--|--
**geometric_primitive.cpp**|

```cpp
・・・省略(この上に geometric_primitive::geormetirc_primitive(...) メンバ関数)・・・

/// <summary>
/// 頂点バッファオブジェクトの作成
/// </summary>
/// <param name="device"></param>
/// <param name="vertices">頂点情報</param>
/// <param name="vertex_count">頂点数</param>
/// <param name="indices">インデックス情報</param>
/// <param name="index_count">インデックス数</param>
void geometric_primitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // 頂点座標分のサイズを計算
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    // 頂点座標が設定されている配列を設定
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
```

2D の時と同じように頂点バッファオブジェクトの生成を行っています。
ポイントとしては 3D になり**立方体**を作成しているので頂点座標の数とインデックスの数が膨大な量になっています。

注意点としては ComPtr でダブルポインタを取得している２か所

    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());

以前までは **GetAddressOf()** を利用していましたがここでは **ReleaseAndGetAddressOf()** を利用しています。

     vertex_buffer.ReleaseAndGetAddressOf();

を GetAddressOf() で行う場合、

    vertex_buffer.Release();
    vertex_buffer.GetAddressOf();

とほぼほぼ同じ処理だと考えてください。
以前までのインスタンスを解放してから新たに生成したものを受け取るという処理です。

---

#### <span style="color:#334488;">幾何プリミティブクラスの render メンバ関数の実装</span><a name="幾何プリミティブクラスのrenderメンバ関数の実装_"></a>

幾何プリミティブクラスの描画処理部分を実装しています。

    演習手順６

|実装するファイル|
--|--
**geometric_primitive.cpp**|

```cpp

・・・省略(この上に geometric_primitive::geormetirc_primitive(...) メンバ関数)・・・

/// <summary>
/// 描画処理
/// </summary>
/// <param name="immediate_context">デバイスコンテキスト</param>
/// <param name="world">ワールド行列</param>
/// <param name="material_color">マテリアルカラー</param>
void geometric_primitive::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& material_color)
{
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	// インデックスバッファオブジェクトの設定
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// 定数バッファとして、ワールド行列とマテリアルカラーを設定
	constants data{ world, material_color };
	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	// 定数(コンスタント)バッファオブジェクトの設定
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	// インデックスを指定して描画
	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth/sizeof(uint32_t), 0, 0);
}

・・・省略(この下に void geometric_primitive::create_com_buffers(...) メンバ関数)・・・

```

インデックスを指定しての描画になるので指定するべきインデックスを GPU 側に設定する必要があります。
それが以下の部分になります。型は **DXGI_FORMAT_R32_UINT** です。

```
immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
```

**geometric_primitive.hlsli** にオブジェクト定数として **ワールド行列** と **マテリアルカラー** を設定できるようにしていました。
まず以下の部分でワールド行列とマテリアルカラーを定数バッファに格納しています。

```
constants data{ world, material_color };
immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
```

次の行で格納した定数バッファを GPU 側へ設定しています。 

```
immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
```

第一引数は以下のシェーダーの **register(b0)** の **b0** 番に対応しています。

```
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
};
```

これで CPU 側の**ワールド行列**と**マテリアルカラー**がシェーダー側の **world** と **material_color** へ代入され利用することが出来るようになります。


今までは **Draw()** でしたが、今回からはインデックスを指定しての描画になるので以下のようになります。

```
// インデックスを指定して描画
D3D11_BUFFER_DESC buffer_desc{};
index_buffer->GetDesc(&buffer_desc);
immediate_context->DrawIndexed(buffer_desc.ByteWidth/sizeof(uint32_t), 0, 0);
```

---

#### <span style="color:#334488;">framework クラスに定義</span><a name="frameworkクラスに定義_"></a>

シーン定数バッファを **framework** クラスに定義します。

    演習手順７－１

もう１つの定数(コンスタント)バッファであるシーン定数バッファを設定していきましょう。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;

*   // 定数バッファフォーマット
*   struct scene_constants
*   {
*       DirectX::XMFLOAT4X4 view_projection;	// ビュー・プロジェクション変換行列
*       DirectX::XMFLOAT4 light_direction;		// ライトの向き
*   };
*   // 定数バッファオブジェクト
*   Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

    framework(HWND hwnd);
    ・・・省略・・・
```

---

#### <span style="color:#334488;">シーン定数バッファオブジェクトを生成</span><a name="シーン定数バッファオブジェクトを生成_"></a>

framework クラスのコンストラクタでシーン定数バッファオブジェクトを生成する。

    演習手順７－２

上記で宣言したの生成を行います。

|実装するファイル|実装するメンバ関数
--|--
**geometric_primitive.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・

    sprites[0] = std::make_unique<sprite>(device.Get(), L".\\resources\\fonts\\font0.png");

*   // 定数バッファオブジェクトの生成
*   D3D11_BUFFER_DESC buffer_desc{};
*   buffer_desc.ByteWidth = sizeof(scene_constants);
*   buffer_desc.Usage = D3D11_USAGE_DEFAULT;
*   buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
*   buffer_desc.CPUAccessFlags = 0;
*   buffer_desc.MiscFlags = 0;
*   buffer_desc.StructureByteStride = 0;
*   hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
*   _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return true;
```

---

#### <span style="color:#334488;">シーン定数バッファに値をセット</span><a name="シーン定数バッファに値をセット_"></a>

framework クラスの render メンバ変数でビュー・プロジェクション変換行列を計算し、それを定数バッファにセットする。

    演習手順７－３

**framework::initialize()** で生成したシーン定数バッファに値を設定しGPU側に登録します。
まずは設定する値の計算を行います。計算する値はプロジェクション行列とビュー行列です。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::render(...)**

```cpp
        ・・・省略・・・
        sprites[0]->textout(immediate_context.Get(), ...

*       // ビューポートを取得
*       D3D11_VIEWPORT viewport;
*       UINT num_viewports{ 1 };
*       immediate_context->RSGetViewports(&num_viewports, &viewport);

*       // アスペクト比を計算
*       float aspect_ratio{ viewport.Width / viewport.Height };
*       // 左手座標系投資投影でプロジェクション行列を作成
*       DirectX::XMMATRIX P {
*           DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f)
*       };

*       // カメラの位置を設定
*       DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
*       // カメラが見るターゲットの位置を設定
*       DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
*       // カメラの上の方向を設定
*       DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
*       // 左手座標系でビュー行列を作成
*       DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };

*       scene_constants data{};
*       // ビュー行列とプロジェクション行列を掛け合わせる(乗算)
*       XMStoreFloat4x4(&data.view_projection, V * P);
*       // 光の当たる方向を設定
*       data.light_direction = { 0, 0, 1, 0 };
*       // ビュー・プロジェクション行列と光の当たる方向をシーン定数バッファとして設定
*       immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
*       // 設定したシーン定数バッファを GPU 側に登録
*       immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

    #ifdef USE_IMGUI
    ・・・省略・・・
```

ここも重要なので１つ１つ見ていきましょう。

---

まずは以下の**プロジェクション行列**を生成している部分です。

```cpp
// ビューポートを取得
D3D11_VIEWPORT viewport;
UINT num_viewports{ 1 };
immediate_context->RSGetViewports(&num_viewports, &viewport);

// アスペクト比を計算
float aspect_ratio{ viewport.Width / viewport.Height };
// 左手座標系投資投影でプロジェクション行列を作成
DirectX::XMMATRIX P{ 
    DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) 
};
```

投資投影でプロジェクション行列を生成するには以下の関数を利用しています。

```cpp
XMMATRIX XMMatrixPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
```

|型|引数名|説明|
--|--|--|--
**float**|**FovAngleY**|視野角(fovY)<br>上から下までの表示可能な角度
**float**|**AspectRatio**|画面の縦横比(アスペクト比)<br>スクリーンの幅と高さの比率
**float**|**NearZ**|一番近くの距離<br>オブジェクトが表示される一番近くの位置<br>これより手前のものは表示が行われません
**float**|**FarZ**|一番遠くの距離<br>オブジェクトが表示される一番遠くの位置<br>これより奥にあるものは表示が行われません

この関数の引数に必要な値を設定していきましょう。

視野角には 30 度が設定されています。

```
DirectX::XMConvertToRadians(30)
```

アスペクト比である画面の縦横比を計算するためにビューポートの情報を取得しています。
取得したビューポート内部にある幅と高さからアスペクト比を計算しています。

```
// ビューポートを取得
D3D11_VIEWPORT viewport;
UINT num_viewports{ 1 };
immediate_context->RSGetViewports(&num_viewports, &viewport);

// アスペクト比を計算
float aspect_ratio{ viewport.Width / viewport.Height };
```

最後にプロジェクション行列を作成。その際に表示可能な一番近い距離と一番遠い距離を設定しています。

```
// 左手座標系投資投影でプロジェクション行列を作成
DirectX::XMMATRIX P{ 
    DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) 
};
```

この距離の外側にいるものは描画の処理すら行われません。
一番近い距離が 0.1f になっているのは 0.0f だとカメラにペタッと張り付いている状態なので結局表示されないにも関わらず描画の処理は行われるという無駄なことをしてしまうので 0.1f という距離を保っています。

---

次に**ビュー行列**の作成が行われています。

```cpp
// カメラの位置を設定
DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
// カメラが見るターゲットの位置を設定
DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
// カメラの上の方向を設定
DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
// 左手座標系でビュー行列を作成
DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
```

ビュー行列を生成するには以下の関数を利用します。

```cpp
XMMATRIX XMMatrixLookAtLH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition, FXMVECTOR UpDirection)
```

カメラの位置や視点、上の方向を制御します。

|型|引数名|説明|
--|--|--|--
**FXMVECTOR**|**EyePosition**|視点<br>カメラの位置<br>eye
**FXMVECTOR**|**FocusPosition**|注視点<br>カメラが見るターゲットの位置<br>focus
**FXMVECTOR**|**upDirection**|上ベクトル<br>カメラの上の向きのベクトル<br>up

一行ずつ確認していきましょう。

```cpp
// カメラの位置を設定
DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
```

カメラを設置する位置を設定します。上記の設定だと**ワールド座標系**においてカメラの位置は z 軸に -10 とあるので、原点から少し手前に設置されることになります。

```
// カメラが見るターゲットの位置を設定
DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
```

次にターゲット(注視点)です。原点位置(0,0,0)をさしているので、手前に設置されたカメラからまっすぐに原点を見つめていることになります。つまり原点方向にあるものを表示するという事になります。

```
// カメラの上の方向を設定
DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
```

最後にカメラの上側の方向です。y 軸に対して 1.0 となっているのでカメラの上側が真上を向いているということです。普段、カメラを利用するとき、カメラの上側の方向は天井を向いていると思います。普通の状態ですね。
この値が -1.0 だったらカメラの上側が下を向いているので世界がさかさまになって表示されるというのが理解できるでしょうか？

```
// 左手座標系でビュー行列を作成
DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
```

視点、注視点、上ベクトルを設定してビュー行列を作成します。

---

生成したビューとプロジェクションの情報を掛け合わせて(乗算)し、シーン用の定数バッファとして設定し、GPU 側で利用できるようにします。

加えて光があたっている方向もここで設定します。
光はひとまずカメラの方向からまっすぐ出ているとして設定しています。
ポイントライトなどの設定はここではまだ扱っていません。まずはまっすぐの光です。

```cpp
scene_constants data{};
// ビュー行列とプロジェクション行列を掛け合わせる(乗算)
XMStoreFloat4x4(&data.view_projection, V * P);
// 光の当たる方向を設定
data.light_direction = { 0, 0, 1, 0 };
// ビュー・プロジェクション行列と光の当たる方向をシーン定数バッファとして設定
immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
// 設定したシーン定数バッファを GPU 側に登録
immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
```

この部分は途中までは幾何プリミティブ側で行った定数バッファの設定とほぼほぼ同じです。
扱うデータと登録する場所が違うだけです。
扱うデータは上記で説明した通り **ビュー・プロジェクション行列** **光の方向** です。




**geometric_primitive.hlsli** にsシーン定数として **ビュー・プロジェクション行列** と **光の方向** を設定できるようにしていました。
以下の部分でビュー・プロジェクション行列と光の方向を定数バッファに格納しています。

```
scene_constants data{};
XMStoreFloat4x4(&data.view_projection, V * P);
data.light_direction = { 0, 0, 1, 0 };
immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);

```

次の行で格納した定数バッファを GPU 側へ設定しています。 

```
immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
```

第一引数は以下のシェーダーの **register(b1)** の **b1** 番に対応しています。

```
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
};
```

これで CPU 側の**ビュー・プロジェクション行列**と**光の方向**がシェーダー側の **view_projection** と **light_direction** へ代入され利用することが出来るようになります。

---

#### <span style="color:#334488;">幾何プリミティブポインタ型の配列を宣言</span><a name="幾何プリミティブポインタ型の配列を宣言_"></a>

    演習手順８

完成した幾何プリミティブクラスを利用するので farmework クラスのメンバ変数として用意しましょう。
演習手順８にあるように要素数を８で宣言します。

**geometric_primitive** を定義するとエラーがでるので必要なヘッダをインクルードしておいてください。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;

*   // 幾何プリミティブ
*   std::unique_ptr<geometric_primitive> geometric_primitives[8];

    // 定数バッファフォーマット
    struct scene_constants
    ・・・省略・・・
```

---

#### <span style="color:#334488;">幾何プリミティブオブジェクトを生成</span><a name="幾何プリミティブオブジェクトを生成_"></a>

    演習手順９

宣言した幾何プリミティブを生成します。

|実装するファイル|実装するメンバ関数|
--|--
**framework.cpp**|**framework::initialize()**

```cpp
    ・・・省略・・・
    sprites[0] = std::make_unique<sprite>(device.Get(), L".\\resources\\fonts\\font0.png");

*   geometric_primitives[0] = std::make_unique<geometric_primitive>(device.Get());

    // 定数バッファオブジェクトの生成
    ・・・省略・・・
```

---

#### <span style="color:#334488;">拡大縮小（S）・回転（R）・平行移動（T）行列を計算する</span><a name="拡大回転平行移動行列を計算する_"></a>

    演習手順１０－１

幾何プリミティブを描画するための **render(...)** メンバ関数には **デバイスコンテキスト** の他に、**ワールド変換行列** と **マテリアルカラー** を外部から受け取れるようになっている。

まずは **ワールド変換行列** として **拡大縮小行列** **回転行列** **平行移動行列** を計算します。

その前に、3D になり奥行が必須となったので奥行を持ったピクセルの情報が必要になりました。
なので **深度ステンシルステート** を

    深度テスト:オフ、深度ライト:オフ

から

    深度テスト：オン、深度ライト：オン

に切り替えておきましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework**|**framework::render(...)**

```cpp
        ・・・省略・・・
        immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

*       // 深度テスト：オン、深度ライト：オンの深度ステンシルステートを設定しておく
*       immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);

    #ifdef USE_IMGUI
    ・・・省略・・・
```

---

深度テストの設定が終わったら、拡縮、回転、移動の行列を生成しましょう。

|実装するファイル|実装するメンバ関数|
--|--
**framework**|**framework::render(...)**

```cpp
        ・・・省略・・・
        immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);

*       // 拡大縮小行列
*       DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(1, 1, 1) };
*       // 回転行列
*       DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) };
*       // 平行移動行列
*       DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(0, 0, 0) };

    #ifdef USE_IMGUI
    ・・・省略・・・
```

---

#### <span style="color:#334488;">上記３行列を合成しワールド変換行列を作成する</span><a name="上記３行列を合成しワールド変換行列を作成する_"></a>

    演習手順１０－２

生成した３つの行列を掛け合わせてワールド変換行列を作成します。

|実装するファイル|実装するメンバ関数|
--|--
**framework**|**framework::render(...)**

```cpp
        ・・・省略・・・
        DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(0, 0, 0) };

*       // ワールド変換行列
*       DirectX::XMFLOAT4X4 world;
*       DirectX::XMStoreFloat4x4(&world, S * R * T);

    #ifdef USE_IMGUI
    ・・・省略・・・
```

---

#### <span style="color:#334488;">幾何プリミティブクラスの render メンバ関数を呼び出す</span><a name="幾何プリミティブクラスのrenderメンバ関数を呼び出す_"></a>

    演習手順１０－３

生成した **ワールド変換行列** を幾何プリミティブの描画の親行列として渡します。
ここでマテリアルカラーの設定も行っています。

|実装するファイル|実装するメンバ関数|
--|--
**framework**|**framework::render(...)**

```cpp
        ・・・省略・・・
        DirectX::XMStoreFloat4x4(&world, S * R * T);

*       // 幾何プリミティブの描画
*       geometric_primitives[0]->render(immediate_context.Get(), world, { 0.5f, 0.8f, 0.2f, 1.0f });

    #ifdef USE_IMGUI
    ・・・省略・・・
```

---

#### <span style="color:#334488;">実行し、正立方体が描画される事を確認する</span><a name="正立方体が描画される事を確認する_"></a>

    演習手順１１

実行して「**正立方体**」が表示されるかどうか確認してみましょう。

---

#### <span style="color:#334488;">カメラの位置・ライトの照射方向を ImGUI を使って実行時に変更できるようにする</span><a name="カメラの位置・ライトの照射方向をImGUIを使って実行時に変更できるようにする_"></a>


    演習手順１２

カメラの位置・ライトの位置を ImGUI を使って実行時に変更できるようにするというものです。

まずはカメラの位置から変更できるようにしていきましょう。
現状カメラ位置は直の値が設定されています。

順序としては

1. framework::update() にある ImGUI でカメラの位置の編集
2. framework::render() で編集された位置を反映

という順番になります。
メンバ関数をまたいでいるので **frmaework** クラスにカメラの位置用のメンバ変数を用意しましょう。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

*   // カメラの位置
*   DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, -10.0f, 1.0f };

    framework(HWND hwnd);
```

このカメラ位置を ImGUI で変更できるようにします。

|実装するファイル|実装するメンバ関数|
--|--
**framework.h**|**framework::update(...)**

```cpp
    ・・・省略・・・
    #ifdef USE_IMGUI
        ImGui::Begin("ImGUI");

*    	ImGui::SliderFloat("camera_position.x", &camera_position.x, -100.0f, +100.0f);
*    	ImGui::SliderFloat("camera_position.y", &camera_position.y, -100.0f, +100.0f);
*    	ImGui::SliderFloat("camera_position.z", &camera_position.z, -100.0f, +100.0f);

        ImGui::End();
    #endif
    ・・・省略・・・
```

ImGUI の仕様は以下になります。

|関数|説明
--|--
**ImGui::SliderFloat(...)**|スライダーで設定されている変数の値を変更可能

引数の説明は以下になります。

|引数|説明|
--|--
第１引数|ImGui 側に表示される名称
第２引数|変更される値へのポインタ
第３引数|最低値
第４引数|最大値

最後に変更した値をカメラの位置として設定しましょう。

現状カメラの位置は以下のように実装されています。

```cpp
DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
```

これを以下のように変更してください。

|実装するファイル|実装するメンバ関数|
--|--
**framework**|**framework::render(...)**

```cpp
    DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
```

これで実行して確認してみましょう。
ImGUI のプルダウン部分をクリックすると以下のような画面が表示されます。
矢印部分にある水色のスライダーをドラッグしてカメラの位置をリアルタイムに変更してみましょう。

@import "images/IMGUIでカメラ位置01.png"

---

次はライトの方向を変更できるように実装してみましょう。
やることはカメラの位置と同じです。

まずは編集できるようにライトの方向の値を **framework クラス** のメンバ変数として宣言しましょう。

|実装するファイル|実装するクラス
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・
    DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, -10.0f, 1.0f };

*   // ライトの方向
*   DirectX::XMFLOAT4 light_direction{ 0.0f, 0.0f, 1.0f, 0.0f };

    framework(HWND hwnd);
    ・・・省略・・・
```

宣言したら ImGUI のスライダーで編集できるようにします。

|実装するファイル|実装するメンバ関数
--|--
**framework.cpp**|**framework::update(...)**

```cpp
    ・・・省略・・・
    ImGui::SliderFloat("camera_position.z", &camera_position.z, -100.0f, +100.0f);

*   ImGui::SliderFloat("light_direction.x", &light_direction.x, -1.0f, +1.0f);
*   ImGui::SliderFloat("light_direction.y", &light_direction.y, -1.0f, +1.0f);
*   ImGui::SliderFloat("light_direction.z", &light_direction.z, -1.0f, +1.0f);

    ImGui::End();
    ・・・省略・・・
```

最低値は -1.0 で最大値は +1.0f になっているので注意してください。

起動時に表示される名前と編集する値を同じものにしていますがこれは統一する必要はありません。
つまり下記の部分などは

```cpp
ImGui::SliderFloat("light_direction.x", &light_direction.x, -1.0f, +1.0f);
```

これでも構わないという事です。デバッグする際になんの値を編集しているかが解れば問題ありません。

```cpp
ImGui::SliderFloat("hikari no houkou x", &light_direction.x, -1.0f, +1.0f);
```

編集した光の方向の値を、利用している部分に設定しましょう。
**framewok::render(...)** に実装されている以下のコードの部分を

```cpp
data.light_direction = { 0, 0, 1, 0 };
```

以下のように変更しましょう。

|ファイル|メンバ関数
--|--
**framework.cpp**|**framework::render(...)**

```cpp
    data.light_direction = light_direction;
```

---

#### <span style="color:#334488;">正立方体の位置・姿勢・寸法・色を ImGUI を使って実行時に変更できるようにする</span><a name="正立方体の位置・姿勢・寸法・色をImGUIを使って実行時に変更できるようにする_"></a>

    演習手順１３

ひとまず色以外の **拡大率(寸法)、回転(姿勢)、位置** の３つを編集できるように実装してみましょう。

現在これらの値は以下のように設定されています。

```
DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(1, 1, 1) };
DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) };
DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(0, 0, 0) };
```

演習手順１２と同じように、

1. メンバ変数を用意し
2. **framework::update(...)** で編集できるようにし
3. **framework::render(...)** で編集した値が反映されるように修正

してみて下さい。

ちなみに宣言するメンバ変数は以下のように仕様を設定しておきます。

|名称|メンバ変数名|最低値|最大値|使用する<br>ImGUI<br>関数
--|--|--|--|--
**位置**|**translation**|-10.0f|10.0f|SliderFloat
**拡大率(寸法)**|**scaling**|-10.0f|10.0f|SliderFloat
**回転(姿勢)**|**rotation**|-10.0f|10.0f|SliderFloat

上記の仕様を元に演習手順１２を参考に３つの設定を実装してみてください。

---

最後に色を ImGUI で変更できるようにしてみましょう。

これまでと同じなのですが、使用する ImGUI の関数が特殊でカラーピッカーを表示できるタイプのものを利用します。

まずはメンバ変数を用意しましょう。

|実装するファイル|実装するクラス|
--|--
**framework.h**|**framework**

```cpp
    ・・・省略・・・

*   // マテリアルカラー
*   DirectX::XMFLOAT4 material_color{ 1 ,1, 1, 1 };

    framework(HWND hwnd);
    ・・・省略・・・
```

用意したら編集できるようにします。
利用する ImGUI の関数として **ColorEdit4(...)** を利用します。

```cpp
    ・・・省略・・・
*   ImGui::ColorEdit4("material_color", reinterpret_cast<float*>(&material_color));

    ImGui::End();
    ・・・省・・・
```

これでスライダーを利用しての色の変更とカラーピッカーでの色の変更が可能になります。
すべて実装して起動すると ImGUI 部分が以下のような画像になっています。

@import "images/IMGUI完成.png"