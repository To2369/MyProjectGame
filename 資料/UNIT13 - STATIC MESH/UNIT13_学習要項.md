<!--
UNIT13_LEARNING UNIT13 学習要項
STATIC MESH - GEOMETRY

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

OBJ File format
OBJ File parser
-->

# UNIT13_LEARNING STATIC MESH - GEOMETRY 学習要項

---

## <span style="color:#334488;">OBJ File format</span>
#### <span style="color:#334488;">OBJ ファイルフォーマット</span>

* 3D ジオメトリのみを表現するための単純なデータ形式
* 頂点データと面を構成する情報だけのデータ
* テキスト形式の行(ライン)単位で情報が格納されている
* 行の頭に行が何かを表すためのキーワードが必ずある

右が**OBJ ファイルのフォーマット**で左がその説明です。

@import "images/OBJフォーマット.png"

---

## <span style="color:#334488;">OBJ File parser</span>
#### <span style="color:#334488;">OBJ ファイル 解析</span>

OBJ ファイルは行単位で読み込み。
行の頭にあるキーワード毎になんのデータなのかが定められているので、その定められたデータにそれぞれ設定を行っていきます。

例えば行に以下が記載されている場合は

    v 1.0 1.0 1.0

頂点座標の { x, y, z, } が {1.0, 1.0, 1.0} という意味になるので、頂点座標として宣言した変数に対して代入を行行ってあげればよいという事になります。

といった感じに左の**キーワード**を読み取り、右の値を適したものに設定していくという作業を OBJ ファイルの解析と呼びます。


