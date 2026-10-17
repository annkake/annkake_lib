# アルゴリズムライブラリ

C++20。各ヘッダーは単独でインクルードでき、`rep`・`INF`・`ll`・`vl` などのテンプレート定義を必要としません。各ファイル冒頭にDoxygen形式の説明・計算量・使用例があります。

以下のパス・コマンドはライブラリのルート（`kyopro/lib/annkake_lib` またはZIPの展開先）を基準にしています。

```cpp
#include "algo/technique/compress.hpp"
#include "algo/technique/rle.hpp"

std::vector<std::string> input{"z", "a", "z"};
auto [ranks, values] = compress(input); // ranks={1,0,1}, values={"a","z"}
auto runs = rle_encode("aaabb");
auto original = rle_decode(runs);
```

グラフ・データ構造の添字は0始まり、区間は原則 `[left,right)` です。頂点番号はグラフの範囲内を指定してください。

## 一覧と旧ファイルの対応

旧名は移行前の `ALGO/` 内の相対パスです。現在のディレクトリ変更は [構成対応表](layout.md) にあります。互換ラッパーは置いていません。

| 現在のファイル | 主なAPI | 整備前のファイル |
|---|---|---|
| [data_structure/bit.hpp](../algo/data_structure/bit.hpp) | `BIT<T>: add / sum` | `Graph/BIT` |
| [data_structure/lazy_segtree.hpp](../algo/data_structure/lazy_segtree.hpp) | `LazySegTree / LazyAddSum / LazySetSum / LazyAddMin / LazySetMin` | 追加 |
| [data_structure/segtree.hpp](../algo/data_structure/segtree.hpp) | `SegTree<T>: set / min` | `DataStructure/segtree` |
| [dp/digit_dp.hpp](../algo/dp/digit_dp.hpp) | `digit_dp / digit_sum_count` | `DP/digitDP` |
| [dp/lis.hpp](../algo/dp/lis.hpp) | `LIS<true> / LIS<false>` | 追加 |
| [graph/bfs.hpp](../algo/graph/bfs.hpp) | `bfs(g,source)` | `Graph/BFS` |
| [graph/dfs.hpp](../algo/graph/dfs.hpp) | `dfs(g,source)` | `Graph/DFS` |
| [graph/dijkstra.hpp](../algo/graph/dijkstra.hpp) | `dijkstra(g,source,inf)` | `Graph/Dijkstra` |
| [graph/doubling.hpp](../algo/graph/doubling.hpp) | `Doubling: jump` | 追加 |
| [graph/dsu.hpp](../algo/graph/dsu.hpp) | `DSU: root / same / unite / size / members` | `Graph/UnionFind` |
| [graph/euler_tour.hpp](../algo/graph/euler_tour.hpp) | `EulerTour: order / tour / tin / tout` | 追加 |
| [graph/floyd.hpp](../algo/graph/floyd.hpp) | `floyd(d,inf)` | `Graph/Warshall` |
| [graph/lca.hpp](../algo/graph/lca.hpp) | `LCA: query / dist / on_path` | `Graph/LCA` |
| [graph/scc.hpp](../algo/graph/scc.hpp) | `SCC: add_edge / build / dag` | `Graph/SCC` |
| [math/comb.hpp](../algo/math/comb.hpp) | `Comb: query` | `Math/Comv` |
| [math/div_ceil.hpp](../algo/math/div_ceil.hpp) | `div_ceil` | `Math/Ceil` |
| [math/div_floor.hpp](../algo/math/div_floor.hpp) | `div_floor` | `Math/Floor` |
| [math/divisors.hpp](../algo/math/divisors.hpp) | `divisors` | `Math/yakusuu` |
| [math/ext_gcd.hpp](../algo/math/ext_gcd.hpp) | `ext_gcd` | `Math/exGCD` |
| [math/factor.hpp](../algo/math/factor.hpp) | `factor` | `Math/PFD` |
| [math/gcd.hpp](../algo/math/gcd.hpp) | `gcd` | `Math/GCD` |
| [math/gf2.hpp](../algo/math/gf2.hpp) | `gf2_solve / gf2_rank` | `Math/F2_Gauss_Jordan` |
| [math/mat_pow.hpp](../algo/math/mat_pow.hpp) | `mat_pow` | `Math/MatrixPow` |
| [math/mod_inv.hpp](../algo/math/mod_inv.hpp) | `mod_inv` | `Math/modinv` |
| [math/mod_pow.hpp](../algo/math/mod_pow.hpp) | `mod_pow` | `Math/modpow` |
| [math/modint.hpp](../algo/math/modint.hpp) | `ModInt<Mod>` | `Math/modint` |
| [math/prime_sieve.hpp](../algo/math/prime_sieve.hpp) | `prime_sieve` | `Math/isPrime` |
| [math/sieve.hpp](../algo/math/sieve.hpp) | `Sieve: is_prime / factor / divisors` | `Math/fast_PFD` |
| [string/calc_next.hpp](../algo/string/calc_next.hpp) | `calc_next` | 追加 |
| [string/rolling_hash.hpp](../algo/string/rolling_hash.hpp) | `RollingHash: get` | `String/Hash` |
| [string/trie.hpp](../algo/string/trie.hpp) | `Trie: insert / count / lcp` | `String/Trie` |
| [technique/compress.hpp](../algo/technique/compress.hpp) | `compress` | `Technic/Compress` |
| [technique/rle.hpp](../algo/technique/rle.hpp) | `rle_encode / rle_decode` | `Technic/RunLength` |

## 整数型と併用

値・添字は `int` を使います。通常は32bit、ALGOテンプレートの `#define int long long` の後に展開する場合は64bitです。標準ヘッダーはこのdefineより前に読み込んでください（テンプレートの `<bits/stdc++.h>` で対応しています）。ライブラリ自身はマクロを定義・利用しません。

剰余積などの中間値には `__int128`、ハッシュ内部には `std::uint64_t` を使います。GCC/Clangの128bit整数拡張が必要です。通常の32bit環境で大きい値を扱う場合は値の範囲を確認してください。

`math/modint.hpp` / `math/comb.hpp` は同じディレクトリのヘッダーに依存します。直接コードをコピーする場合は依存先も含めてください。`ModInt` はテンプレートの `mint` と異なる名前です。

## 各APIの注意点

- `compress` は入力を変更せず、整数の順位と復元用の値を返します。文字列にも対応します。
- `BIT` は0始まり。`sum(r)` は `[0,r)`、`sum(l,r)` は `[l,r)` の和です。ヘッダーに転倒数の例があります。
- `DSU::members(v)` は成分の頂点列への読み取り専用参照です。小さい成分を大きい成分へ移すため、全マージの頂点移動はO(n log n)。unite後は参照を再取得します。
- `EulerTour` の部分木区間 `[tin[v],tout[v])` は各頂点1回の `order` 上の区間です。戻りを含む `tour`（2*n-1要素）とは区別します。
- `Doubling` の遷移先なしは-1。事前計算の上限を超える回数は例外です。
- 遅延セグ木の4構成は葉を `{初期値,1}` で初期化します。minでも結果は `.value` から取得します。代入の恒等操作は `std::nullopt`。合成順は `Composition(新しい操作,古い操作)` です。
- `LIS<true>` は狭義、`LIS<false>` は広義。返り値は選んだ要素の添字列です。
- 桁DPは元の3種類の遷移を保っています。基数とtransitionの書き方は [桁DPの説明](digit_dp.md) を参照してください。
- `calc_next` は各位置以降の小文字英字の出現位置を返します。存在しない場合は文字列長、末尾の行も同じ値です。
- `RollingHash::get` は半開区間です。固定基数で衝突の可能性があり、同値判定や敵対的入力への耐性を保証しません。
- `Trie::lcp(s)` は登録文字列とのLCP、`lcp(s,2)` は2回以上通過する接頭辞長です。sを先に登録して2を指定すると自分以外との最大LCPを得られます。
- `gf2_solve` は自由変数を0とした解を返します。動的な0/1行列の実装なので、大規模行列ではbitset実装より遅くなり得ます。
- `dijkstra` の到達不能は指定したinf。有限距離はinf未満が前提です。`floyd` は負閉路を検出するとfalseを返します。
- `SegTree` の空区間は型の最大値。`Comb` は素数の法を前提にし、前処理範囲外は例外です。

## 検証とスニペット

```sh
python3 tests/test_algo.py
```

全ヘッダーの単独コンパイル・使用例の実行・ランダム比較・32/64bit境界テストを行います。スニペットは [cpp.code-snippets](../snippets/cpp.code-snippets)、prefix一覧は各テンプレート末尾です。[導入・生成方法](../README.md#cスニペット) を参照してください。

## 公開条件

置換・追加実装に適用するCC0-1.0の範囲は [LICENSE](../LICENSE)、旧コードの調査結果は [権利確認記録](licensing.md) に記載しています。`examples/cheats/` とテンプレート、AHC、参考資料などを一括でCC0にするものではありません。
