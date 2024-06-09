


* 目次
    1. [円柱](#円柱_)
    2. [円から考える](#円から考える_)
    3. [分割対応](#分割対応_)
    4. [下の円を作る](#下の円を作る_)
    5. [側面を作る](#側面を作る_)


---

#### <span style="color:#334488;">円柱</span><a name="円柱_"></a>

以下は円柱を作るための考え方になります。

@import "images/円柱01.png"

まずきれいな円柱だとわかりづらいかもしれませんが、検索をかけるとワイヤーフレームで雑な円の円柱がでてきます。

@import "images/円柱02.png"

上下にある円の部分は三角形をつなぎあわせて円に見せているだけにすぎません。
三角形の数が多ければ多いほどきれいな円に見えるという仕組みです。
上と下の円にある三角形の頂点をつなぐように側面の四角形を作っています。
シンプルに三角形が増えることできれいな円柱が出来上がるという仕組みです。

初めからたくさん頂点がある状態の円柱で考えると考えがまとまらないかと思うので、まずは画像にあるように６個に分かれた円柱で考えてみましょう。

---

#### <span style="color:#334488;">円から考える</span><a name="円から考える_"></a>

６つの三角形を利用する円柱を作るとして、上と下にあるような円を描画するにはどうすればよいのでしょうか？

円を描画するには四角形から辺を増やしてやるという考え方をします。
つまり真ん中に点をおいて三角形を増やしてやるという事ですね。

@import "images/円.png"

以下が円を作る為のコードになります。
geometric_cylinger のコンストラクタに記載して起動すれば円が表示されます。
ただ最終は円柱なので確認の為にのみ記載しておきます。

```cpp
// 半径
float radius = 0.5f;

// 頂点データ
vertex vertices[6 + 1]{};
// 頂点インデックス
uint32_t indices[6 * 3]{};

// 三角形の角度 (デグリー角)
float d{ 2.0f * DirectX::XM_PI / 6 };
// 半径
float r{ 0.5f };

// 円の真ん中の頂点座標と法線
vertices[0].position = { 0.0f, 0.0f, 0.0f };
vertices[0].normal = { 0.0f, 1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < 6; ++i) {

    // 角度から頂点座標を計算
    vertices[1 + i].position.x = radius * cosf(i * d);
    vertices[1 + i].position.y = 0.0f;
    vertices[1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
}

// 頂点インデックスを設定
// １つ目の三角形
indices[0] = 0;
indices[1] = 2;
indices[2] = 1;

// ２つ目の三角形
indices[3] = 0;
indices[4] = 3;
indices[5] = 2;

// ３つ目の三角形
indices[6] = 0;
indices[7] = 4;
indices[8] = 3;

// ４つ目の三角形
indices[9] = 0;
indices[10] = 5;
indices[11] = 4;

// ５つ目の三角形
indices[12] = 0;
indices[13] = 6;
indices[14] = 5;

// ６つ目の三角形
indices[15] = 0;
indices[16] = 1;
indices[17] = 6;

create_com_buffers(device, vertices, 6 + 1, indices, 6 * 3);
```

---

#### <span style="color:#334488;">分割対応</span><a name="分割対応_"></a>

まずは頂点と頂点インデックスの配列の要素数の部分を見てみましょう。

```cpp
// 頂点
vertex vertices[6 + 1]{};
// 頂点インデックス
uint32_t indices[6 * 3]{};
```

この部分は三角形が増えれば数が変化します。
頂点は

    三角形の数 + 中心の点

頂点インデックスは

    三角形の数 * 三角形に必要な頂点の数

になります。
なので配列を動的に変更できるようにします。

```cpp
・・・省略・・・
// 頂点データ
vertex* vertices = new vertex[slices + 1];
// 頂点インデックス
uint32_t* indices = new uint32_t[slices * 3];

// 三角形の角度 (デグリー角)
float d{ 2.0f * DirectX::XM_PI / slices };

・・・省略・・・

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i)
{
    ・・・省略・・・
}

・・・省略・・・

create_com_buffers(device, vertices, slices + 1, indices, slices * 3);
```

これで slices の値を利用できるので三角形の数を変更できるようになり、
以下の部分の頂点データなど、数が増えても変更する必要はなくなりました。

```cpp
// 角度から頂点座標を計算
vertices[1 + i].position.x = radius * cosf(i * d);
vertices[1 + i].position.y = 0.0f;
vertices[1 + i].position.z = radius * sinf(i * d);
// 法線を設定
vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
```

ですが、以下の頂点インデックスは面が増えるごとにコードを追加していかなければなりません。

```cpp
// １つ目の三角形
indices[0] = 0;
indices[1] = 2;
indices[2] = 1;

// ２つ目の三角形
indices[3] = 0;
indices[4] = 3;
indices[5] = 2;
```

共通点が無いように見える頂点インデックスもある程度法則があります。
１つ目の三角形と２つ目の三角形を見てみましょう

どちらも１つ目のインデックスには 0 が代入されています。これは円の中心にある点のインデックスです。
三角形がいくつになってもこの部分は必ず０です。
あとはそれぞれ１つずつ増えていっているだけなので繰り返しで書くなら以下のようになります。

```cpp
for(int i = 0; i < slices; ++i)
{
    indices[(i * 3) + 0] = 0;
    indices[(i * 3) + 1] = i + 2;
    indices[(i * 3) + 2] = i + 1;
}
```

これで「円に必要な数－１」の三角形を用意することが出来ましたが、最後の三角形を確認してみましょう。

```cpp
// ６つ目の三角形
indices[15] = 0;
indices[16] = 1;
indices[17] = 6;
```

最後は 7 ではなく頂点が回って戻ってくるので最初のインデックスである 1 に戻り、
その１つ手前の値には面の数が代入されています。

最後だけ特殊なので繰り替えす数を１減らして、最後の三角形は繰り返しの外側で設定します。
なので最終的に以下のようになります。

```cpp
for(int i = 0 ; i < slices - 1; i++)
{
    indices[(i * 3) + 0] = 0;
    indices[(i * 3) + 1] = i + 2;
    indices[(i * 3) + 2] = i + 1;
}

indices[slices * 3 - 3] = 0;
indices[slices * 3 - 2] = 1;
indices[slices * 3 - 1] = slices;
```

ここまでの編集したコードが以下になります。

---

```cpp
// 分割数(円を作るための三角形の数、６個の三角形)
slices = 6;
// 半径
float radius = 0.5f;

// 頂点データ
vertex* vertices = new vertex[slices + 1];
// 頂点インデックス
uint32_t* indices = new uint32_t[slices * 3];

// 三角形の角度 (デグリー角)
float d{ 2.0f * DirectX::XM_PI / slices };
// 半径
float r{ 0.5f };

// 真ん中の頂点座標と法線
vertices[0].position = { 0.0f, 0.0f, 0.0f };
vertices[0].normal = { 0.0f, 1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i) {

    // 角度から頂点座標を計算
    vertices[1 + i].position.x = radius * cosf(i * d);
    vertices[1 + i].position.y = 0.0f;
    vertices[1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
}

for (int i = 0; i < slices - 1; ++i)
{
    indices[(i * 3) + 0] = 0;
    indices[(i * 3) + 1] = i + 2;
    indices[(i * 3) + 2] = i + 1;
}

indices[slices * 3 - 3] = 0;
indices[slices * 3 - 2] = 1;
indices[slices * 3 - 1] = slices;

create_com_buffers(device, vertices, (slices + 1) * 2, indices, (slices * 3) * 2);

delete[] vertices;
delete[] indices;
```

---

#### <span style="color:#334488;">下の円を作る</span><a name="下の円を作る_"></a>

先ほど作った円を円柱の**上の円**として**下の円**も作ってみましょう。

まずは頂点データと頂点インデックスを下の分追加、つまり倍にします。

```cpp
// 頂点データ
vertex* vertices = new vertex[(slices + 1) * 2];
// 頂点インデックス
uint32_t* indices = new uint32_t[(slices * 3) * 2];
```

上の円の座標位置を少し上にあげるので y 位置に 0.5 を設定しておきます。

まずは真ん中の頂点座標を上にあげます

```cpp
// 真ん中の頂点座標と法線
vertices[0].position = { 0.0f, 0.5f, 0.0f };
```

円の周りの頂点座標も同じ値だけ上にあげておきます。

```cpp
vertices[1 + i].position.y = 0.5f;
```

次に下の円を実装していきます。
頂点の番号は上の円の続きからとなり、上の円の最大が 6 だったのでここで利用する最初の番号は 7 からになります。
つまり面の数 + 1 になります。

下の円の真ん中の点の頂点データと法線を以下のように設定しました。
上の円の反対側になるので法線は逆転しています。

```cpp
// 下の円の真ん中の頂点座標と法線
vertices[slices + 1].position = { 0.0f, -0.5f, 0.0f };
vertices[slices + 1].normal = { 0.0f, -1.0f, 0.0f };
```

次に下の円の周りの頂点データを設定していきます。
ここも y の座標位置が少し下がっていて法線が逆転しています。
下の円の頂点データ番号の始まりは上の円のつづきの番号である slices + 1 となります。

```cpp
// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i)
{
    // 角度から頂点座標を計算
    vertices[slices + 1 + 1 + i].position.x = radius * cosf(i * d);
    vertices[slices + 1 + 1 + i].position.y = -0.5f;
    vertices[slices + 1 + 1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
}
```

vertices の添え字がだんだん怪しくなってきました。

    slices + 1 + 1 + i

slices + 1 は上の円の続きで最後の 1 は下の円の真ん中の頂点の数の分です。
周りの頂点番号はこの slices + 1 + 1 の続きからとなるので次の i が足されることで番号を決定しています。

---

最後に頂点インデックスです。
頂点インデックスも上の円の続きからになるので

```
indeices[18] = 7;
indeices[19] = 9;
indeices[20] = 8;
```

となりますが、上の円がこちらを向いているときは下の円は隠れて見えていません。
反転させたとき下の円がこちらを向いていたら逆に上の円は隠れるでしょう。
ですがこの時、上の円の三角形も下の円の三角形も同じ右周りで三角形が作られていた場合、どうなるでしょうか？

反転した時下の円は裏向きになってしまうので描画されなくなってしまいます。なので下の円は三角形を逆回りで設定します。

```
indeices[18] = 7;
indeices[19] = 8;
indeices[20] = 9;
```

上の設定を基準として実装しましょう。

```cpp
int base_index = slices + 1;
for (uint32_t i = 0; i < slices - 1; ++i)
{
    indices[(slices * 3) + (i * 3) + 0] = base_index + 0;
    indices[(slices * 3) + (i * 3) + 1] = base_index + i + 1;
    indices[(slices * 3) + (i * 3) + 2] = base_index + i + 2;
}

indices[(slices * 3 * 2) - 1] = base_index + 1;
indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
indices[(slices * 3 * 2) - 3] = base_index;
```

slices * 3 は上の円で利用された 0 ~ 17 の続きである 18 番からという意味になっています。
現状のコードは以下になります。

---

```cpp
// 分割数(円を作るための三角形の数、６個の三角形)
slices = 6;
// 半径
float radius = 0.5f;

// 頂点データ
vertex* vertices = new vertex[(slices + 1) * 2];
// 頂点インデックス
uint32_t* indices = new uint32_t[(slices * 3) * 2];

// 三角形の角度 (デグリー角)
float d{ 2.0f * DirectX::XM_PI / slices };
// 半径
float r{ 0.5f };

// 真ん中の頂点座標と法線
vertices[0].position = { 0.0f, 0.5f, 0.0f };
vertices[0].normal = { 0.0f, 1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i) {

    // 角度から頂点座標を計算
    vertices[1 + i].position.x = radius * cosf(i * d);
    vertices[1 + i].position.y = 0.5f;
    vertices[1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
}

for (int i = 0; i < slices - 1; ++i)
{
    indices[(i * 3) + 0] = 0;
    indices[(i * 3) + 1] = i + 2;
    indices[(i * 3) + 2] = i + 1;
}

indices[slices * 3 - 3] = 0;
indices[slices * 3 - 2] = 1;
indices[slices * 3 - 1] = slices;

// 下の円の真ん中の頂点座標と法線
vertices[slices + 1].position = { 0.0f, -0.5f, 0.0f };
vertices[slices + 1].normal = { 0.0f, -1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i)
{
    // 角度から頂点座標を計算
    vertices[slices + 1 + 1 + i].position.x = radius * cosf(i * d);
    vertices[slices + 1 + 1 + i].position.y = -0.5f;
    vertices[slices + 1 + 1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
}

// 下の円のインデックスの設定
int base_index = slices + 1;
for (uint32_t i = 0; i < slices - 1; ++i)
{
    indices[(slices * 3) + (i * 3) + 0] = base_index + 0;
    indices[(slices * 3) + (i * 3) + 1] = base_index + i + 1;
    indices[(slices * 3) + (i * 3) + 2] = base_index + i + 2;
}

indices[(slices * 3 * 2) - 1] = base_index + 1;
indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
indices[(slices * 3 * 2) - 3] = base_index;

create_com_buffers(device, vertices, (slices + 1) * 2, indices, (slices * 3) * 2);

delete[] vertices;
delete[] indices;
```

---

#### <span style="color:#334488;">側面の矩形を作る</span><a name="側面の矩形を作る_"></a>

最後に側面の矩形を作っていきます・・・番号の管理が危うくなっていますがもう少しなので頑張りましょう。

面の数を 6 として考えてみると・・・

頂点データのほうは、

場所|利用している番号|総数|
--|--|--|--
上の円| 0 ~ 6 | 7 個 
下の円| 7 ~ 13 | 7 個

    0 + i
    slices + 1 + 1 + i

これから作る側面は以下のようになります
||||
--|--|--|--
側面| 14 ~ 25 | 12 個

頂点インデックスになると

場所|利用している番号|総数|
--|--|--|--
上の円| 0 ~ 17 | 18 個
下の円| 18 ~ 35 | 18 個

    i * 3
    slices * 3 + i * 3

側面で利用する番号は
三角形12枚分なので12*3で36点必要
||||
--|--|--|--
側面 | 36 ~ 71 | 36 個

となります。

側面で利用される頂点データの番号は　14 ~ 25 とあります。
最後はこの番号を利用して四角形を６面ぐるっと張り巡らせるようにすれば良いのです。

添え字の値をそのまま書いて行くと下のようになる

```cpp
// １つ目の上の点の座標
vertices[14].position = { radius * cosf(0 * d), 0.5f ,radius * sinf(0 * d) }
// １つ目の上の点の法線
vertices[14].normal = { radius * cosf(0 * d), 0.0f ,radius * sinf(0 * d) }

// １つ目の下の点の座標
vertices[15].position = { radius * cosf(0 * d), -0.5f ,radius * sinf(0 * d) }
// １つ目の下の点の法線
vertices[15].normal = { radius * cosf(0 * d), 0.0f ,radius * sinf(0 * d) }

// ２つ目の上の点の座標
vertices[14].position = { radius * cosf(1 * d), 0.5f ,radius * sinf(1 * d) }
// ２つ目の上の点の法線
vertices[14].normal = { radius * cosf(1 * d), 0.0f ,radius * sinf(1 * d) }

// ２つ目の下の点の座標
vertices[15].position = { radius * cosf(1 * d), -0.5f ,radius * sinf(1 * d) }
// ２つ目の下の点の法線
vertices[15].normal = { radius * cosf(1 * d), 0.0f ,radius * sinf(1 * d) }

・・・省略・・・


// ６つ目の上の点の座標
vertices[24].position = { radius * cosf(5 * d), 0.5f ,radius * sinf(5 * d) }
// ６つ目の上の点の法線
vertices[24].normal = { radius * cosf(5 * d), 0.0f ,radius * sinf(5 * d) }

// ６つ目の下の点の座標
vertices[25].position = { radius * cosf(5 * d), -0.5f ,radius * sinf(5 * d) }
// ６つ目の下の点の法線
vertices[25].normal = { radius * cosf(5 * d), 0.0f ,radius * sinf(5 * d) }
```

ハードコーディングするなら上のようになるが繰り替えし出来る部分や法則を切り取って見てみよう。

まずは添え字に使われている番号ですが、 slices + 1 + slices + 1　から始まっています。
ですが、上の点と下の点の座標の計算に使われる番号は上の点と下の点で同じ番号である 0 が利用されています。

なのでインデックスの設定と同じように i * 2 + 0 として番号をずらします。
カウンターで表現できるようになったので繰り返し文にまとめましょう。

```cpp
for (int i = 0 ; i < slices; ++i)
{
    // 上の点の座標
    vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { radius * cosf(i * d), 0.5f ,radius * sinf(i * d) }
    // 上の点の法線
    vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { radius * cosf(i * d), 0.0f ,radius * sinf(i * d) }

    // 下の点の座標
    vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { radius * cosf(i * d), -0.5f ,radius * sinf(i * d) }
    // 下の点の法線
    vertices[slices + 1 + slices + 1 + i * 2 + 1].normal = { radius * cosf(i * d), 0.0f ,radius * sinf(i * d) }
}
```

見づらいので座標の計算をまとめましょう。

```cpp
for (int i = 0 ; i < slices; ++i)
{
    float x = radius * cosf(i * d);
    float z = radius * sinf(i * d);

    // 上の点の座標と法線
    vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { x, 0.5f , z };
    vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { x, 0.0f , z };

    // 下の点の座標と法線
    vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { x, -0.5f , z };
    vertices[slices + 1 + slices + 1 + i * 2 + 1].normal = { x, 0.0f , z };
}
```

まとまりました。
次に頂点インデックスです。これもまずハードコーディングしてみましょう。
始まる添え字番号は 36 からで 利用する頂点インデックスは 14 からです。 

```cpp
// １つ目の四角形の１つ目の三角形
indices[36] = 14;
indices[37] = 16;
indices[38] = 15;
// １つ目の四角形の２つ目の三角形
indices[39] = 15;
indices[40] = 16;
indices[41] = 17;
// ２つ目の四角形の１つ目の三角形
indices[42] = 16;
indices[43] = 18;
indices[44] = 17;
// ２つ目の四角形の２つ目の三角形
indices[45] = 17;
indices[46] = 18;
indices[47] = 19;

・・・省略・・・

// ５つ目の四角形の１つ目の三角形
indices[60] = 22;
indices[61] = 24;
indices[62] = 23;
// ５つ目の四角形の２つ目の三角形
indices[63] = 23;
indices[64] = 24;
indices[65] = 25;

// ６つ目の四角形の１つ目の三角形
indices[66] = 24;
indices[67] = 14;
indices[68] = 25;
// ６つ目の四角形の２つ目の三角形
indices[69] = 25;
indices[70] = 14;
indices[71] = 15;

```

36 は上の円の数と下の円の数の総数である slices * 3 * 2 から計算できます。
そこから１つ目の三角形のインデックスと２つ目の三角形のインデックスを同時に設定していけばよいので計算は i * 6 + n です。
つまり以下のようになります。

```cpp
// i 番目の四角形の１つ目の三角形
indices[slices * 3 * 2 + i * 6 + 0] = 14;
indices[slices * 3 * 2 + i * 6 + 1] = 16;
indices[slices * 3 * 2 + i * 6 + 2] = 15;
// i 番目の四角形の２つ目の三角形
indices[slices * 3 * 2 + i * 6 + 3] = 15;
indices[slices * 3 * 2 + i * 6 + 4] = 16;
indices[slices * 3 * 2 + i * 6 + 5] = 17;
```

次に設定する頂点インデックスです。
１つ目の三角形は

    14 16 15

で、２つ目の三角形は

    15 16 17

と設定されているので以下で表現できます。

```cpp
slices + 1 + slices + 1 + 0
slices + 1 + slices + 1 + 2
slices + 1 + slices + 1 + 1

slices + 1 + slices + 1 + 1
slices + 1 + slices + 1 + 2
slices + 1 + slices + 1 + 3
```

ここで２つ目の矩形を確認すると

```cpp
// ２つ目の四角形の１つ目の三角形
indices[42] = 16;
indices[43] = 18;
indices[44] = 17;
// ２つ目の四角形の２つ目の三角形
indices[45] = 17;
indices[46] = 18;
indices[47] = 19;
```

となっていて、１つ目の矩形から全体的に値が 2 足されていることがわかるので、繰り返し分だけ 2 を足していくので以下のようになります。

```cpp
slices + 1 + slices + 1 + i * 2 + 0
slices + 1 + slices + 1 + i * 2 + 2
slices + 1 + slices + 1 + i * 2 + 1

slices + 1 + slices + 1 + i * 2 + 1
slices + 1 + slices + 1 + i * 2 + 2
slices + 1 + slices + 1 + i * 2 + 3
```

最後の矩形だけは以下のようになっています。
１つ目の四角形と５つ目の四角形と合わせて法則の特徴を考えてみよう

```cpp
// ５つ目の四角形の１つ目の三角形
indices[60] = 22;
indices[61] = 24;
indices[62] = 23;
// ５つ目の四角形の２つ目の三角形
indices[63] = 23;
indices[64] = 24;
indices[65] = 25;

// ６つ目の四角形の１つ目の三角形
indices[66] = 24;
indices[67] = 14;
indices[68] = 25;
// ６つ目の四角形の２つ目の三角形
indices[69] = 25;
indices[70] = 14;
indices[71] = 15;

・・・

// １つ目の四角形の１つ目の三角形
indices[36] = 14;
indices[37] = 16;
indices[38] = 15;
// １つ目の四角形の２つ目の三角形
indices[39] = 15;
indices[40] = 16;
indices[41] = 17;
```

特徴としては、
    「１つ目の三角形」の「２つ目の値」が「２つ目の三角形」の「２つ目の値」
    「１つ目の三角形」の「３つ目の値」が「２つ目の三角形」の「１つ目の値」
    「２つ目の三角形」の「３つ目の値」が「次の矩形の１つ目の三角形」の「３つ目の値」

となっているのがわかる。
なのでこれに習って最後の矩形のみ計算して１つめの矩形の頂点インデックスを使うよう計算すると以下のようになります。

```cpp
// ６つ目の四角形の１つ目の三角形
indices[66] = slices + 1 + slices + 1 + slices - 1 + slices - 1;
indices[67] = slices + 1 + slices + 1;
indices[68] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
// ６つ目の四角形の２つ目の三角形
indices[69] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
indices[70] = slices + 1 + slices + 1;
indices[71] = slices + 1 + slices + 1 + 1;
```

まとめてみましょう。

```cpp
for(int i = 0 ; i < slices - 1; ++i)
{
    // i 番目の四角形の１つ目の三角形
    indices[slices * 3 * 2 + i * 6 + 0] = slices + 1 + slices + 1 + i * 2 + 0;
    indices[slices * 3 * 2 + i * 6 + 1] = slices + 1 + slices + 1 + i * 2 + 2;
    indices[slices * 3 * 2 + i * 6 + 2] = slices + 1 + slices + 1 + i * 2 + 1;
    // i 番目の四角形の２つ目の三角形
    indices[slices * 3 * 2 + i * 6 + 3] = slices + 1 + slices + 1 + i * 2 + 1;
    indices[slices * 3 * 2 + i * 6 + 4] = slices + 1 + slices + 1 + i * 2 + 2;
    indices[slices * 3 * 2 + i * 6 + 5] = slices + 1 + slices + 1 + i * 2 + 3;
}

// ６つ目の四角形の１つ目の三角形
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1;
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1;
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
// ６つ目の四角形の２つ目の三角形
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1;
indices[slices * 3 * 2 + 5 * 6 + 0] = slices + 1 + slices + 1 + 1;
```


となる。６つ目の四角形の１つ目の三角形部分の添え字部分を見てみよう

    slices * 3 * 2 + 6 * 5 + 0

slices * 3 * 2 は上の円と下の円の頂点インデックスの数です。
5 * 6 は繰り返し部分で設定している、矩形の部分の三角形２つ分の６枚目 * ６頂点分という事です。

しかし、このままだと 5 * 6 としているので６枚分割の円柱にしか対応できなくなってしまいます。
6 は頂点が６個あるので不変ですが、円柱は６枚なのか３２枚なのかは引数によって変化する必要があります。

この部分には添え字の最大数が必要になるので分割数から１を引いた数を設定する必要があります。
つまり

    slices - 1

となります。

これで上下の円と周りの矩形の部分を結ぶことができました。
最終的なコードは以下のようになります。

```cpp
// 分割数(円を作るための三角形の数、６個の三角形)
slices = 6;
// 半径
float radius = 0.5f;

// 頂点データ
vertex* vertices = new vertex[(slices + 1) * 2 + slices * 2];
// 頂点インデックス
uint32_t* indices = new uint32_t[(slices * 3) * 2 + (3 * 2 * slices)];

// 三角形の角度 (デグリー角)
float d{ 2.0f * DirectX::XM_PI / slices };
// 半径
float r{ 0.5f };

// 真ん中の頂点座標と法線
vertices[0].position = { 0.0f, 0.5f, 0.0f };
vertices[0].normal = { 0.0f, 1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i) {

    // 角度から頂点座標を計算
    vertices[1 + i].position.x = radius * cosf(i * d);
    vertices[1 + i].position.y = 0.5f;
    vertices[1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
}

for (int i = 0; i < slices - 1; ++i)
{
    indices[(i * 3) + 0] = 0;
    indices[(i * 3) + 1] = i + 2;
    indices[(i * 3) + 2] = i + 1;
}

indices[slices * 3 - 3] = 0;
indices[slices * 3 - 2] = 1;
indices[slices * 3 - 1] = slices;

// 下の円
int base_slices = slices + 1;

// 下の円の真ん中の頂点座標と法線
vertices[base_slices].position = { 0.0f, -0.5f, 0.0f };
vertices[base_slices].normal = { 0.0f, -1.0f, 0.0f };

// 円の周りの頂点座標と法線を設定
for (int i = 0; i < slices; ++i)
{
    // 角度から頂点座標を計算
    vertices[slices + 1 + 1 + i].position.x = radius * cosf(i * d);
    vertices[slices + 1 + 1 + i].position.y = -0.5f;
    vertices[slices + 1 + 1 + i].position.z = radius * sinf(i * d);
    // 法線を設定
    vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
}

// 下の円のインデックスの設定
int base_index = slices + 1;
for (uint32_t i = 0; i < slices - 1; ++i)
{
    indices[(slices * 3) + (i * 3) + 0] = base_index + 0;
    indices[(slices * 3) + (i * 3) + 1] = base_index + i + 1;
    indices[(slices * 3) + (i * 3) + 2] = base_index + i + 2;
}

indices[(slices * 3 * 2) - 1] = base_index + 1;
indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
indices[(slices * 3 * 2) - 3] = base_index;


// 側面
for (int i = 0; i < slices; ++i)
{
    float x = radius * cosf(i * d);
    float z = radius * sinf(i * d);

    // 上の点の座標と法線
    vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { x, 0.5f , z };
    vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { x, 0.0f , z };

    // 下の点の座標と法線
    vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { x, -0.5f , z };
    vertices[slices + 1 + slices + 1 + i * 2 + 1].normal = { x, 0.0f , z };
}

for (int i = 0; i < slices - 1; ++i)
{
    // i 番目の四角形の１つ目の三角形
    indices[slices * 3 * 2 + i * 6 + 0] = slices + 1 + slices + 1 + i * 2 + 0;
    indices[slices * 3 * 2 + i * 6 + 1] = slices + 1 + slices + 1 + i * 2 + 2;
    indices[slices * 3 * 2 + i * 6 + 2] = slices + 1 + slices + 1 + i * 2 + 1;
    // i 番目の四角形の２つ目の三角形
    indices[slices * 3 * 2 + i * 6 + 3] = slices + 1 + slices + 1 + i * 2 + 1;
    indices[slices * 3 * 2 + i * 6 + 4] = slices + 1 + slices + 1 + i * 2 + 2;
    indices[slices * 3 * 2 + i * 6 + 5] = slices + 1 + slices + 1 + i * 2 + 3;
}

// ６つ目の四角形の１つ目の三角形
indices[slices * 3 * 2 + (slices - 1) * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1;
indices[slices * 3 * 2 + (slices - 1) * 6 + 1] = slices + 1 + slices + 1;
indices[slices * 3 * 2 + (slices - 1) * 6 + 2] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
// ６つ目の四角形の２つ目の三角形
indices[slices * 3 * 2 + (slices - 1) * 6 + 3] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
indices[slices * 3 * 2 + (slices - 1) * 6 + 4] = slices + 1 + slices + 1;
indices[slices * 3 * 2 + (slices - 1) * 6 + 5] = slices + 1 + slices + 1 + 1;


create_com_buffers(device, vertices, (slices + 1) * 2 + slices * 2, indices, (slices * 3) * 2 + (3 * 2 * slices));

delete[] vertices;
delete[] indices;
```


一番上の

```cpp
slices = 6;
```

をコメントアウトして起動してみましょう。
framework::initialize() で geometric_primitive の生成を 32 としているので３２分割されています。

ここまでが円柱の考え方になります。