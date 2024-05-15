<!--
UNIT06_EXERCISE UNIT06 演習手順
SPRITE - TEXEL

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">

Texture space
Texel space
-->

# UNIT06 SPRITE - TEXEL 演習手順

---

#### テクスチャの任意の矩形領域を描画する

前回は読み込んだ画像ファイルをテクスチャとして生成し、そのすべて描画した。ここでは生成したテクスチャの一部分を描画する方法実装してみよう。

今回の実行結果は以下の画像になります。

@import "images/UNIT06.png"

---

    演習手順１

まずは切り取りに適した画像をテクスチャとして読み込もう。

|ファイル|メンバ関数|
--|--
|framework.cpp|framework::initialize()|

```cpp
    sprites[0] = new sprite(device, L".\\resources\\cyberpunk.jpg");

*   sprites[1] = new sprite(device, L".\\resources\\player-sprites.png");

    return truする
```

読み込んだ画像は下のような画像になる。

@import "images/player-sprites.png"

---

#### 任意の矩形領域をスプライト外部から指定できるようにする

    演習手順２

**sprite クラス** の **render メンバ関数** を<span style="border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;">オーバーロード</span>してください。

|ファイル|クラス|
--|--
|sprite.h|sprite|

```cpp
	
    void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, 
    float r, float g, float b, float a, float angle/*degree*/);

*   void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, 
*   float r, float g, float b, float a, float angle/*degree*/, 
*   float sx, float sy, float sw, float sh);
```

追加した sx, sy, sw, sh はテクセル座標系の

    左 = sx
    上 = sy
    幅 = sw
    高さ = sh

になります。

---

次は定義部分になります。

定義部分は現在の **sprite の render() メンバ関数** を内容ごとすべてコピーして引数だけを追加しましょう。

|ファイル|
--|--
|sprite.cpp|

```cpp
    void sprite::render(ID3D11DeviceContext* immediate_context,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a, float angle/*degree*/,
*   float sx, float sy, float sw, float sh);
```

テクスチャの描画する矩形を外部から設定できるようにしたら sprite の render メンバ関数内部で**テクセル座標** から **テクスチャ座標** に変換する実装を行いましょう。

$
テクセル座標系での左座標　/　テクセル座標系での画像幅　=\\　テクスチャ座標系での左座標\\
テクセル座標系での上座標　/　テクセル座標系での画像高さ　=\\　テクスチャ座標系での上座標\\
テクセル座標系での右座標　/　テクセル座標系での画像幅　=\\　テクスチャ座標系での右座標\\
テクセル座標系での下座標　/　テクセル座標系での画像高さ　=\\　テクスチャ座標系での下座標
$

ここでのテクセル座標系の左上右下は引数として受け取った

    sx          // 左
    sy          // 上
    sx + sw     // 右
    sy + sh     // 下

です。

テクセル座標系での画像幅と画像高さは **texture2d_desc** がメンバ変数として持っています。

    texture2d_desc.Width    // 幅
    texture2d_desc.Height   // 高さ

あとは **sprite** の **render メンバ関数** の下記の部分を

    vertices[0].texcoord = { 0, 0 };
    vertices[1].texcoord = { 1, 0 };
    vertices[2].texcoord = { 0, 1 };
    vertices[3].texcoord = { 1, 1 };

以下のように変更することで、

|ファイル|メンバ関数|
--|--
|sprite.cpp|sprite::render(...)|

<span style="color:#994433;font-size:80%;font-weight: bold;">render メンバ関数は２種類になったので注意して下さい。</span>

```cpp
vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
```

画像ファイルから指定の部分だけを描画する準備ができました。

---

#### 描画位置を指定して呼び出す

    演習手順３

実際に描画位置を指定して **render メンバ関数を呼び出し** 指定の矩形を描画してみましょう。

|ファイル|メンバ関数|
--|--
|framework.cpp|framework::render(...)|

```cpp
    ・・・省略・・・
    sprites[0]->render(immediate_context, 100, 100, 500, 500, 1, 1, 1, 1, 45);
*   sprites[1]->render(immediate_context, 700, 200, 200, 200, 1, 1, 1, 1, 45, 0, 0, 140, 240);

#ifdef USE_IMGUI
    ・・・省略・・・
```

---

    演習手順４

描画部分をオーバーロードで追加したはいいのですが、２つの **render メンバ関数** のコードが重複している部分が多くある事がわかるかと思います。
重複を無くし見やすくするため以前使用していたほうの render メンバ関数の中身をすべて消して下記のように変更しましょう。

|ファイル|
--|--
|sprite.cpp|

```cpp
void sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle/*degree*/)
{
*   // 幅高さを 0,0 から 画像最大にしてオーバーロードしたほうの render を呼び出す
*   render(immediate_context,
*   dx, dy, dw, dh,
*   r, g, b, a,
*   angle,
*   0.0f, 0.0f, 
*   static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}
```

以前まで使用していた render メンバ関数は画像を端から端まで描画するというものでした。つまりテクセル座標の 0 から画像サイズまでを描画するという事です。
なのでこの部分で新しく作成した render メンバ関数を呼び出し、テクセル座標で描画範囲部分に 0 から 画像サイズを指定する事と同じ処理になります。

    0.0f, 0.0f, 
    static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));

起動して以下の画像が表示できるか確認しましょう。

@import "images/UNIT06.png"