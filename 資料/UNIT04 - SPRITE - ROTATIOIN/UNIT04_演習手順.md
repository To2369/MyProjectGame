<!--
UNIT04_EXERCISE UNIT04 演習手順
SPRITE - ROTATION
-->


# UNIT04 SPRITE - ROTATION 演習手順

UNIT04.pdf

---

#### <span style="color:#334488;">回転角度を外部から設定できるようにする</span>

    演習手順１

||
--|--
**sprite.h**|**sprite**

```cpp
    void render(ID3D11DeviceContext* immediate_context,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,
*   float angle/*degree*/);
```

||
--|--
**sprite.cpp**|

```cpp
    void sprite::render(ID3D11DeviceContext* immediate_context,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a,
*   float angle/*degree*/);
```

---

スプライトの描画部分でアフィン変換行列を使って回転をさせてみよう。

|||
--|--
**sprite.h**|**sprite::render(...)**

```cpp
    ・・・省略・・・
    // right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };

*   auto rotate = [](float& x, float& y, float cx, float cy, float angle) {
*       x -= cx;
*       y -= cy;

*       float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
*       float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
*       float tx{ x }, ty{ y };

*       x = cos * tx + -sin * ty;
*       y = sin * tx + cos * ty;

*       x += cx;
*       y += cy;
*   };

*   float cx = dx + dw * 0.5f;
*   float cy = dy + dh * 0.5f;

*   rotate(x0, y0, cx, cy, angle);
*   rotate(x1, y1, cx, cy, angle);
*   rotate(x2, y2, cx, cy, angle);
*   rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC 座標系に変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    ・・・省略・・・
```

回転のアフィン変換行列は下の式の囲った部分と、

$
\begin{pmatrix} x' \\ y' \\ 1 \end{pmatrix} =
\begin{pmatrix} \cos\theta & -\sin\theta & 0 \\ \sin\theta & \cos\theta & 0 \\
0 & 0 & 1 \end{pmatrix}
\begin{pmatrix} x \\ y \\ 1 \end{pmatrix} =
\boxed{
\begin{pmatrix} \cos\theta*x+-\sin\theta*y + 0*1 \\ \sin\theta*x+\cos*y + 0*1 \\ 0*x +0*y+1*1 \end{pmatrix}
}
$

ソースコードの以下の部分に注目

    x = cos * tx + -sin * ty;
    y = sin * tx + cos * ty;

変換行列が使われている部分を理解しておこう。

この変換をポリゴンの頂点である４点すべてに行う必要がある。
今回の演習ではそれを**ラムダ式**という形で行っている

    auto rotate = [](float& x, float& y, float cx, float cy, float angle) 

ラムダ式はローカルな関数と考えてもらえばよい。
ローカルな **rotate 関数** をポリゴンの４点に対して呼び出すことで

    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

回転のアフィン変換が行われている。

---

#### <span style="color:#334488;">スプライトの回転</span>


ラムダ式とラムダ式内のソースコード上で気になる部分がありますね。

ラムダ式の引数であるこの部分。画像の**中心位置 cx, 中心位置 cy**。

    float cx, float cy

ラムダ式内の２か所。

    x -= cx;
    y -= cy;

    ・・・省略・・と

    x += cx;
    y += cy;

#### この部分は何をしているのか？

スプライトの回転は以下の順番で処理が行われている。

@import "images/スプライトの回転01.png"
##### <span style="color:#222299;">◇スプライトの中心を原点に移動</span>
@import "images/スプライトの回転02.png"
##### <span style="color:#222299;">◇アフィン変換行列式で頂点を回転</span>
@import "images/スプライトの回転03.png"
##### <span style="color:#222299;">◇原点から元の位置へ移動</span>
@import "images/スプライトの回転04.png"

つまり

    x -= cx;
    y -= cy;

    ・・・省略・・・

    x += cx;
    y += cy;

この部分は回転する過程で原点に移動した際画像の中心位置に移動を行う処理を記述している。

この部分を記載していないと、画像の左下を回転の中心としてしまうため、おかしな表示になってしまうので覚えておこう。
