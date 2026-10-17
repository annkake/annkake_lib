# テンプレート（C++20）

- AtCoder / ICPC / AHC はいずれも `LOCAL` 定義時のみ debug を有効にします。無効時は引数も評価しません。
- `debug(x)` は `[DEBUG] x = 値` を標準エラーへ出します。pair、tuple、配列、コンテナ、optional、variant、`debug_fields()` を持つ構造体に対応し、modint は `val()` を表示します。
- `rep(i,n)`: 0〜n-1、`rrep(i,n)`: 1〜n、`drep(i,n)`: n-1〜0。n <= 0 はいずれも0回です。
- `nfor(i,s,n)`: s〜n-1、`dfor(i,s,n)`: s-1〜n。上下限が逆転している場合は0回です。境界式は一度だけ評価します。境界値は `int` で表現できる整数を指定してください。
- `popcount(x)`: 64bit整数の立っているビット数。
- `floor_div(a,b)` / `ceil_div(a,b)`: 符号付き整数の除算の切り下げ／切り上げ。負の除数にも対応します。ゼロ除算と結果の範囲外は例外にします。標準の `std::floor` / `std::ceil` と衝突しない名前です。
- `fix(n)`: `cout << fixed << setprecision(n)`。
- `pq_max<T>` / `pq_min<T>`: `top()` が最大／最小になる優先度付きキュー。旧 `pq` / `pq_g` は置き換えてください。
- `ll` は廃止し、`vl` などの要素型も `int` にしました。ALGO は既存の `#define int long long` により64bit、AHCは通常の32bitです。AHCで大きいスコアが必要なら `int64_t` を指定してください。`INF64` は64bit定数を維持しています。

## ローカル実行と出力分離

```sh
g++ -std=c++20 -O2 -DLOCAL templates/atcoder.cpp -o a.out
./a.out < input.txt
```

`LOCAL` 指定時はC++内で出力を保持し、通常終了時に `[cerr]`、`[cout]` の順で元の標準エラーへまとめて表示します。Pythonの実行は不要です。各ストリーム内の順序は保持します。大量の出力はメモリを使用し、異常終了時は表示されない場合があります。対話型問題では `LocalOutput` の生成を外してください。

`LOCAL` 指定時のみ `#define endl "\n"` を定義します。未指定時は標準の `std::endl` を使い、出力の分離も行いません。提出時は `-DLOCAL` を外してください。

## 異なる整数型の比較

`max(a,b)` / `min(a,b)` は異なる整数型をテンプレートの `int` に変換して比較します。同型の値や明示的な `std::max` / `std::min` は標準関数が処理します。`chmax` / `chmin` は整数を同様に変換して比較し、更新の有無を返します。変換先と代入先に値が収まることが前提です。同じ非整数型同士の `chmax` / `chmin` も利用できます。

ACL の配列の戻り値は `auto z = z_algorithm(s);` のように受け取ります。ALGO側のdebug関数は名前空間を使わず、`debug_print_value` / `debug_print_tuple` / `print_debug` として定義しています。

## 共通定義の管理

ALGO / ICPC / AHC の共通定義の原本は `templates/common.hpp.in` です。修正後に次のコマンドで展開します。生成範囲の外にある solve / main や AHC 探索実装は保持します。

```sh
python3 tools/sync_templates.py
python3 tools/sync_templates.py --check
python3 tests/test_templates.py
```

提出用ファイルには定義を展開済みなので、提出時の追加ヘッダーは不要です。

- ALGO の `int` は64bit、AHCの `int` は32bitを維持します。比較や `floor_div` / `ceil_div` の引数は各テンプレートの `int` の範囲内で使用してください。
- `INF` はALGOが `2'000'000'000'000'000'000LL`、AHCが `1 << 30` です。`INF64` は双方で `1LL << 60` です。
- 方向文字列は `DIR`（`string_view`）に統一しました。`di` / `dj` は `constexpr array<int,4>` で、添字で参照できます。
- `YES` / `NO` / `YN` / `dame` は改行のみで、flushを行いません。
- AHCでもALGOと同じ型エイリアス・入出力補助・マクロを利用できます。`mint` / `vmint` / `vvmint` / `vvvmint` はACLが使える場合のみ定義します。ICPCは自前mintを使います。
- AHCのデバッグ関数もグローバルの `print_debug` / `debug_print_value` に統一しました。探索機能の `ahc` 名前空間は維持します。

## ライブラリ用スニペットのヘッダー

ライブラリ用スニペットは `#include` と `#pragma once` を取り除いて生成します。AtCoder / ICPC / AHCの各テンプレートが `<bits/stdc++.h>` を読み込むので、追加時に標準ヘッダーが重複しません。テンプレート自身のincludeは維持します。スニペットをテンプレート以外へ展開する場合は必要な標準ヘッダーを先に読み込んでください。単独include用の `algo/` 内のヘッダーは従来どおり必要なincludeを保持します。
