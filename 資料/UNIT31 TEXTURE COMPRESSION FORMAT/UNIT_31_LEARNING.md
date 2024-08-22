<!--
UNIT31_LEARNING UNIT31 学習要項
TEXTURE COMPRESSION FORMAT

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Texture compression format
Mipmap
DDS file
-->

# <span style="color:#444444;">UNIT31 学習要項</span>

## <span style="color:#334488;">TEXTURE COMPRESSION FORMAT</span>

---

### <span style="color:#334488;">Texture compression format</span>
#### <span style="color:#334488;">圧縮テクスチャフォーマット</span>

GPU に最適化された画像形式。

通常画像には jpeg や png が多く使われるが GPU 自体はこれらの画像フォーマットをデコードする機能ありません。
利用している描画フレームワークに含まれている画像を GPU に転送する処理を行う際に、内部的に CPU でデコードしてから転送を行っています。

1024px * 1024px の jpeg ファイルをテクスチャーとして使用する場合、一律で 1024 * 1024 * 3（ピクセルあたりのバイト数）= 3MB の情報にデコードされ、GPUに転送されます。圧縮率の高い jpeg でも GPU に展開すると画像サイズに比例した容量になります。

これは、GPU へ転送する際に CPU でのデコード処理と転送オスとが高くなる事と、VRAM(GPU 上のメモリ)を多く使用する事を意味します。

そこで対策として出てくるのが**圧縮テクスチャー**です。圧縮テクスチャーは GPU がデコードできるフォーマットのことです。
圧縮されたデータはそのまま GPU へ転送され VRAM に配置されます。シェーダーからの読み込み時に始めてデコードされます。
なので、キャッシュを効率よく利用でき、 VRAM へのメモリアクセスが削減できるので非圧縮の形式より高速になります。

---

### <span style="color:#334488;">Mipmap</span>
#### <span style="color:#334488;">ミップマップ</span>

ミップマップは以前解説した通りですが、今回は DDS テクスチャにコンバートする際にミップマップとして利用できるかどうかを指定することが出来ます。
コンバートする際に

    -m 0

上記のように -m の後に 0 を指定することですべてのミップマップを生成した状態で DDS テクスチャを作成することができます。
1 を指定するとミップマップを削除した状態で DDS テクスチャを作成します。

---

### <span style="color:#334488;">DDS file</span>
#### <span style="color:#334488;">DDS ファイル</span>

DDS（Direct Draw Surface）とは圧縮テクスチャフォーマットの１つで、DirectX で簡単に利用することができる。