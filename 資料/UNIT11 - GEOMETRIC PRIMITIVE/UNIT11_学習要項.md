<!--
UNIT09_LEARNING UNIT11 学習要項
SPRITE - BATCHING

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Index buffers
Constant buffers
World, View and Projection transformation matrices
Scale, Rotate and Translate transformation matrices
-->

# UNIT11_LEARNING GEOMETRIC PRIMITIVE 学習要項


---

## <span style="color:#334488;">Index buffers</span>
#### <span style="color:#334488;">インデックスバッファ</span>

インデックスバッファとは頂点インデックスを指定して描画を行える機能の事です。

以下は以前紹介したモデルデータを形作るポリゴンの頂点データの図です。ポリゴンである三角形を形作る頂点座標は頂点データとして設定し頂点バッファに格納されます。

@import "images/ポリゴンと頂点バッファ.png"

頂点データとして設定される際に配列として設定しますが、この時、上から順に番号が割り当てられています。

ポリゴンを形作る三角形を指定する番号はこの配列の添え字になっています。

以前まではインデックスの指定は内部で自動で割り振られていたので

```cpp
void Draw(UINT VertexCount, UINT StartVertexLocation);
```

を利用していましたが、今回からはこのインデックスの割り振りもこちらで設定したものを利用します。

つまり、**どの頂点を使って三角形を作るのかをこちらで指定する**、ということです。

利用するメンバ関数は以下のメンバ関数です。

```cpp
void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT  BaseVertexLocation);
```

こちらで頂点を指定して三角形を作るという事は無駄を省けるという事が理解できるでしょうか？

上の図の立方体を見てみると１つの四角形を作るために６個の頂点が利用されています。

@import "images/インデックスバッファ01.png"

頂点インデックスを指定して四角形を作るのであれば、同じ座標の頂点は同じインデックスを使えばよいので、頂点は４つで作れるはずです。

@import "images/インデックスバッファ02.png"

立方体を作る場合、前面や側面は重なっている頂点がいくつもあるのでもっと数を省略できるというのは理解できるかと思います。

これら頂点のインデックスを扱うことができるのがインデックスバッファになります。

---

## <span style="color:#334488;">Constant buffers</span>
#### <span style="color:#334488;">定数バッファ</span>

シェーダー内で自由に使う事のできる値。

定数バッファを使うときは **cbuffer** キーワードを使い、構造体のように宣言します。定数バッファとして宣言された変数はシェーダー内のどこからでも使うことが出来ます。C++ でいうところのグローバル変数のようなものです。もちろん定数なので代入は出来ません。以下はシェーダー側での定数バッファの宣言の例です。

```hlsl
cbuffer PARAM : register(b0)
{
    float4 clear_color;
    uint2 screen_size;
}
```

値はデバイスコンテキストを介して CPP 側から設定を行います。なので **register()** として CPP 側から登録できるような書き方になっています。
**registre()** 内の番号はデバイスコンテキストから設定する際に指定ができます。以下は CPP 側での設定の例です。

```cpp
constants data{ clearColor, screenSize };
immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
```

**VSSetConstantBuffers()** の第一引数が **0** なので **register()** に設定している番号が **b0** になります。

---

## <span style="color:#334488;">World, View and Projection transformation matrices</span>
#### <span style="color:#334488;">ワールド変換行列、ビュー変換行列、プロジェクション変換行列</span>

---

#### <span style="color:#334488;">ワールド変換行列 ( ワールド行列 ) </span>

ローカル座標系をワールド座標系に変換する行列の事をワールド変換行列と呼びます。
ワールド変換行列は回転、スケーリング、平行移動などの情が含まれた 4x4 の行列で表現される。

---

##### ローカル座標系

それぞれのモデルデータが、それぞれに持っているモデルデータの中心を原点とした座標の事をローカル座標系と呼びます。

それぞれのモデルデータごとに原点があるので、この座標系で ３D 空間の位置を合わせるのは不可能だというのは理解できると思います。

    どのモデルの原点を基準に移動させればいいんだ？
    
と、なってしまいますね。

---

##### ワールド座標系

なのですべてのモデルにおいて共通の原点をもった座標系を定めました。それをワールド座標系と呼びます。

@import "images/ワールド変換行列.png"

---

#### <span style="color:#334488;">ビュー変換行列 ( ビュー行列 )</span>

カメラを基準にした行列です。

掛け合わせることでカメラ視点での座標位置が計算されます。
ビュー行列は計算された座標の事をビュー座標と呼びます。

ビュー座標に変換するためのカメラの視点を決めるための行列でカメラの位置や方向を制御します。

```cpp
XMMATRIX XMMatrixLookAtLH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition, FXMVECTOR UpDirection)
```

ビュー変換行列は以下のパラメータで行列を作ります。

|型|引数名|説明|
--|--|--|--
**FXMVECTOR**|**EyePosition**|視点<br>カメラの位置<br>eye
**FXMVECTOR**|**FocusPosition**|注視点<br>カメラのターゲット位置<br>focus
**FXMVECTOR**|**upDirection**|上ベクトル<br>カメラの上向きのベクトル<br>up

また、ワールド変換行列を逆行列化することでも求められる。

---

##### ビュー座標

ビュー座標とは 3D 空間を映し出す為のカメラを原点とした座標の事です。カメラ座標とも呼ばれます。ビュー変換行列にあるカメラの位置や向きを設定することで画面に映し出される映像も変わります。

@import "images/ビュー変換行列.png"

---

#### <span style="color:#334488;">プロジェクション変換行列 ( プロジェクション行列 ) </span>

3D 空間上の座標をスクリーン(2D)に投影するための変換行列。

---

##### 透視投影(Perspective Projection)

カメラとオブジェクトの距離でサイズが変わります。
遠くのものは小さく見え、近くのものは大きく見えます。
3D ゲームで使われています。2D ゲームでもたまに利用されます。

@import "images/透視投影.png"

透視投影のプロジェクション行列を作成するには以下の関数を利用します。

```cpp
XMMATRIX XMMatrixPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
```

|型|引数名|説明|
--|--|--|--
**float**|**FovAngleY**|視野角(fovY)<br>上から下までの表示可能な角度
**float**|**AspectRatio**|画面の縦横比(アスペクト比)<br>スクリーンの幅と高さの比率
**float**|**NearZ**|一番近くの距離<br>オブジェクトが表示される一番近くの位置<br>これより手前のものは表示が行われません
**float**|**FarZ**|一番遠くの距離<br>オブジェクトが表示される一番遠くの位置<br>これより奥にあるものは表示が行われません

---

##### 平行投影(Orthogonal Projection)

カメラの距離によって見える大きさがかわりません。
同じサイズのオブジェクトがあった場合、距離に関係なく同じサイズで表示されます。

@import "images/平行投影.png"

平行投影のプロジェクション行列を作成するには以下の関数を利用します。

```cpp
XMMATRIX XMMatrixOrthographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
```

以下のパラメータで行列を作ります。
* ビューボリュームの幅 (widht)
* ビューボリュームの高さ (height)
* 一番近くの距離 (nearZ)
* 一番遠くの距離 (farZ)

|型|引数名|説明|
--|--|--|--
**float**|**ViewWidth**|ビューボリュームの幅|
**float**|**ViewHeight**|ビューボリュームの高さ|
**float**|**NearZ**|一番近くの距離<br>オブジェクトが表示される一番近くの位置<br>これより手前のものは表示が行われません
**float**|**FarZ**|一番遠くの距離<br>オブジェクトが表示される一番遠くの位置<br>これより奥にあるものは表示が行われません



---

## <span style="color:#334488;">Scale, Rotate and Translate transformation matrices</span>
#### <span style="color:#334488;">拡大変換行列、回転変換行列、移動変換行列</span>

以前 UNIT03.pdf の**アフィン変換行列**で説明した事とほぼほぼ同じ説明になりますが、3D になるので３次元という事で、さらに行列が１行増えることになります。以前までは 3x3 の行列でしたが、今回からは 4x4 の行列を扱っていきます。

加えて重要な点は前回の行列はネット上でよく見かける**右手座標系**での変換行列であるという事。
**右手座標系**とは **OpenGL** や **Vulkan** に利用されている座標系です。

ただ DirectX は**左手座標系**になるので行列同士をかけ合わせる際に以前の行列の公式をあてはめてもうまくいきません。とはいえ今は下記の関数で指定の行列を手に入れることができるのであまり行列を気にする必要はないかもしれません。

```cpp
// 拡大行列
XMMATRIX XMMatrixScaling(float ScaleX, float ScaleY, float ScaleZ)
// 回転行列
XMMATRIX XMMatrixRotationRollPitchYaw(float Pitch, float Yaw, float Roll)
// 移動行列
XMMATRIX XMMatrixTranslation(float OffsetX, float OffsetY, float OffsetZ)
```

ただ、自作の行列関数や、 DirectX がバージョンアップした際や、まったく別のフレームワークを利用しなくてはいけなくなった場合などは上の関数すらも自作しなくてはならないので**左手座標系**における行列の公式を確認しておきましょう。

それでは右手座標系と左手座標系で公式がどのように変わるのか確認していきましょう。

---

#### <span style="color:#334488;">拡大変換行列 ( 拡大行列 ) </span>

アフィン変換でも紹介したように**右手座標系**での拡大行列は

$
\begin{pmatrix} x' \\ y' \\ z' \end{pmatrix} =
\begin{pmatrix} X & 0 & 0 \\ 0 & Y & 0 \\ 0 & 0 & Z\end{pmatrix}
\begin{pmatrix} x \\ y \\ z \end{pmatrix} =
\begin{pmatrix} X*x + 0*y + 0*z \\ 0*x + Y*y + 0*z \\ 0*x + 0*y + Z*z \end{pmatrix} =
\begin{pmatrix} X*x \\ Y*y \\ Z*z \end{pmatrix}
$

です。

これが**左手座標系**になると結果は同じですが、途中の公式と計算が変化します。

$
\begin{pmatrix} x' \\ y' \\ z' \end{pmatrix} =
\begin{pmatrix} x & y & z \end{pmatrix}
\begin{pmatrix} X & 0 & 0 \\ 0 & Y & 0 \\ 0 & 0 & Z\end{pmatrix} =
\begin{pmatrix} X*x + 0*y + 0*z \\ 0*x + Y*y + 0*z \\ 0*x + 0*y + Z*z \end{pmatrix} =
\begin{pmatrix} X*x \\ Y*y \\ Z*z \end{pmatrix}
$

です。
拡大変換だといまいちわかりづらいですが、縦に計算が行われています。
初めの計算だけを抜き取ると以下のようになっています。

$
\begin{pmatrix} x' \\ y' \\ z' \end{pmatrix} =
\begin{pmatrix} x & y & z \end{pmatrix}
\begin{pmatrix} X \\ 0  \\ 0 \end{pmatrix} =
\begin{pmatrix} X*x + 0*y + 0*z \end{pmatrix} =
\begin{pmatrix} X*x \end{pmatrix}
$

なので計算結果は変わりません。

これで 4x4 になるので１行が移動用のダミーの行列になるということで


$
\begin{pmatrix} x' \\ y' \\ z' \\ 0 \end{pmatrix} =
\begin{pmatrix} x & y & z & 0\end{pmatrix}
\begin{pmatrix} X & 0 & 0 & 0 \\ 0 & Y & 0 & 0 \\ 0 & 0 & Z & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix} =
\begin{pmatrix} X*x + 0*y + 0*z + 0*0 \\ 0*x + Y*y + 0*z + 0*0 \\ 0*x + 0*y + Z*z + 0*0\\ 0*x + 0*y + 0*z + 0*1 \end{pmatrix} =
\begin{pmatrix} X*x \\ Y*y \\ Z*z \\ 0 \end{pmatrix}
$

となる。

拡縮行列を作成する場合は

    XMMATRIX XMMatrixScaling(float ScaleX, float ScaleY, float ScaleZ)

を利用します。

---

#### <span style="color:#334488;">回転変換行列 ( 回転行列 ) </span>

3D の回転には X 軸による回転、Y 軸による回転、Z 軸による回転の３種類があります。
対して 2D の回転は Z 軸による回転のみでした。下記は以前説明したアフィン変換による回転です。

$
\begin{pmatrix} x' \\ y' \end{pmatrix} =
\begin{pmatrix} \cos\theta & -\sin\theta \\ 
\sin\theta & \cos\theta \end{pmatrix}
\begin{pmatrix} x \\ y \end{pmatrix} =
\begin{pmatrix} \cos\theta*x+-\sin\theta*y \\ \sin\theta*x+\cos\theta*y \end{pmatrix}
$

これを 4x4 にしたうえで**左手座標系**に変換すると以下のようになります。

$
\begin{pmatrix} x' \\ y' \\ z'\\ 0 \end{pmatrix} =
\begin{pmatrix} x & y & z & 0 \end{pmatrix}
\begin{pmatrix} \cos\theta & \sin\theta & 0 & 0 \\ -\sin\theta & \cos\theta & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1  \end{pmatrix} = 
\begin{pmatrix} x*\cos\theta + y*-\sin\theta + z*0 + 0*0 \\
x*\sin\theta + y*\cos\theta + z*0 + 0*0 \\
x*0 + y*0 + z*1 + 0*0 \\
x*0 + y*0 + z*0 + 0*1 \end{pmatrix} = \\
\begin{pmatrix} x*\cos\theta+y*-\sin\theta \\ x*\sin\theta+y*\cos\theta \\ z \\ 0 \end{pmatrix}
$

途中の計算は変化しますが計算結果は同じで、そこに 3D なので z の要素が含まれました。
Z 軸による回転行列を作成する場合は

    XMMATRIX XMMatrixRotationZ(float Angle)

を利用します。


3D になると X 軸による回転と Y 軸による回転も可能となるので回転行列にはあと２つ追加されることになります。

以下が X 軸による回転です。

$
\begin{pmatrix} x' \\ y' \\ z'\\ 0 \end{pmatrix} =
\begin{pmatrix} x & y & z & 0 \end{pmatrix}
\begin{pmatrix}
    1 & 0 & 0 & 0 \\
    0 & \cos\theta & \sin\theta & 0 \\
    0 & -\sin\theta & \cos\theta & 0 \\
    0 & 0 & 0 & 1
\end{pmatrix} = 
\begin{pmatrix} 
x*1 + y*0 + z*0 + 0*0 \\
x*0 + y*\cos\theta + z*-\sin\theta + 0*0 \\
x*0 + y*\sin\theta + z*\cos\theta + 0*0 \\
x*0 + y*0 + z*0 + 0*1 
\end{pmatrix} = \\
\begin{pmatrix} x \\ y*\cos\theta+z*-\sin\theta \\ y*\sin\theta+z*\cos\theta \\ 0 \end{pmatrix}
$

X 軸による回転行列を作成する場合は

    XMMATRIX XMMatrixRotationX(float Angle)

を利用します。


最後に Y 軸による回転です。

$
\begin{pmatrix} x' \\ y' \\ z'\\ 0 \end{pmatrix} =
\begin{pmatrix} x & y & z & 0 \end{pmatrix}
\begin{pmatrix}
    \cos\theta & 0 & -\sin\theta & 0 \\
    0 & 1 & 0 & 0 \\
    \sin\theta & 0 & \cos\theta & 0 \\
    0 & 0 & 0 & 1
\end{pmatrix} = 
\begin{pmatrix}
x*\cos\theta + y*0 + z*\sin\theta + 0*0\\
x*0 + y*1 + z*0 + 0*0 \\
x*\sin\theta + y*0 + z*\cos\theta + 0*0\\
x*0 + y*0 + z*0 + 1*0
\end{pmatrix} = \\
\begin{pmatrix} x*\cos\theta + z*\sin\theta \\ y \\ x*\sin\theta + z*\cos\theta \\ 0 \end{pmatrix}
$

Y 軸による回転行列を作成する場合は

    XMMATRIX XMMatrixRotationY(float Angle)

を利用します。

---

#### <span style="color:#334488;">移動変換行列 ( 移動行列 ) </span>

最後に移動変換行列です。アフィン変換で見せた移動変換行列は**右手座標系**でした。



**左手座標系**にすると

$
\begin{pmatrix} x' \\ y' \\ z' \\ 1 \end{pmatrix} =
\begin{pmatrix} x & y & z & 1\end{pmatrix}
\begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ X & Y & Z & 1 \end{pmatrix} =
\begin{pmatrix} 1*x + 0*y + 0*z + X*1 \\ 0*x + 1*y + 0*z + 1*Y \\ 0*x + 0*y + z*1 + 1*Z\\ 0*x + 0*y + 0*z + 1*1 \end{pmatrix} =
\begin{pmatrix} x+X \\ y+Y \\ z+Z \\ 1 \end{pmatrix}
$

となります。途中の計算の行程は変化してますが、結果は変化してません。

移動を回転行列を作成する場合は

    XMMATRIX XMMatrixTranslation(float OffsetX, float OffsetY, float OffsetZ)

を利用します。



---