<!--
UNIT30_LEARNING UNIT30 学習要項
SERIALIZATION

<span style="color:#994433;border: 1px red solid; padding: 2px;font-size:100%;font-weight: bold;"></span>

Serialization
Cereal
-->

# <span style="color:#444444;">UNIT30 学習要項</span>

## <span style="color:#334488;">SERIALIZATION</span>

---

### <span style="color:#334488;">Serialization</span>
#### <span style="color:#334488;">シリアライズ</span>

シリアライズとは、複数の要素を一列に並べる操作や処理のこと。
単にシリアライズといった場合にはデータ構造などを独自形式ファイルに変換する事を指します。

今回のケースでは skinned_mesh クラスにあるデータを独自形式ファイルに変換します。

@import "images/シリアライズ01.png"

モデルが複雑になるにつれて起動時間が遅くなってきていることに気付いているかと思う。

@import "images/シリアライズ02.png"

なので skinned_mesh に読み込んだ必要なデータだけを独自形式ファイルに変換して外部に保存し、次回以降の読み込みを、その独自形式ファイルから取り出す事で読み込みにかかる時間を限りなく減らすというものである。

@import "images/シリアライズ03.png"

これらの仕組みの事を**シリアライズ**と呼びます。

また、シリアライズの仕組みを利用している中で「元データを独自形式ファイルに変換する処理の事」を**シリアライズ**と呼び、「独自形式ファイルを元データに復元する処理の事」を**デシリアライズ**と呼びます。

---

### <span style="color:#334488;">Cereal</span>
#### <span style="color:#334488;">Cereal</span>

上記のシリアライズを行う事の出来る C++11 用のシリアライズライブラリ。

バイナリ、JSON, XML の複数のフォーマットにシリアライズでき、比較的簡単に導入ができる。