<!--
UNIT14_LEARNING UNIT14 学習要項
STATIC MESH - TEXTURE

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

MTL File format
MTL File parser
-->

# UNIT14_LEARNING STATIC MESH - TEXTURE 学習要項

---

## <span style="color:#334488;">MTL File format</span>
#### <span style="color:#334488;">MTL ファイルフォーマット</span>

* OBJ ファイルに対応するマテリアル(材質)のデータファイル
* OBJ ファイル内に記載されていて、OBJ ファイルを読み込んだ際にキーワード指定でマテリアルファイルを読み込む
* テキスト形式の行(ライン)単位で情報が格納されている
* 行の頭に行が何かを表すためのキーワードが必ずある

OBJ ファイルには頂点座標や法線やインデックスの情報などが記載されていました。
一方 MTL ファイルにはテクスチャファイル名や環境光色(アンビエント)、拡散反射光色(ディフューズ)、鏡面反射光色(スペキュラ)などが記載されている。

---

## <span style="color:#334488;">MTL File parser</span>
#### <span style="color:#334488;">MTL ファイル解析</span>

こちらも解析の方法は OBJ ファイルと同じです。

１行１行の頭にあるキーワードを確認してデリミタ単位に区切ってデータを格納していきます。