# annkake_lib

C++20向けのアルゴリズム・ヒューリスティック用ライブラリです。各アルゴリズムのヘッダーにはDoxygen形式の説明と使用例があります。

```text
algo/                 アルゴリズムのヘッダー
  data_structure/     BIT・セグ木・遅延セグ木
  dp/                 桁DP・LIS
  graph/              グラフ・木・DSU・ダブリング
  math/               整数・剰余・行列
  string/             Trie・ハッシュ・次出現位置
  technique/          座標圧縮・ランレングス符号化
templates/            AtCoder / ICPCテンプレート・共通定義の原本
heuristic/            AHCの共通部品・開発版・単一ファイル提出版
snippets/             VS Code用C++スニペット
examples/cheats/       既存CheetCode（内容は維持）
docs/                 利用方法・API一覧・権利確認記録
provenance/           旧実装の出典・置換記録
LICENSES/             ライセンス条文
tools/                同期・生成・公開用ZIP作成
tests/                自動検証
dist/                 生成した公開用ZIP（Git対象外）
```

- [アルゴリズム一覧とAPI](docs/algorithms.md)
- [テンプレートとローカル実行](docs/templates.md)
- [AHCの使い方](docs/heuristic.md)
- [公式スコア関数の取り込み](docs/official_score.md)
- [桁DPのtransitionとbase](docs/digit_dp.md)
- [ディレクトリ構成と旧パスとの対応](docs/layout.md)
- [GitHubへの公開手順](docs/publishing.md)
- [ライセンスの適用範囲](LICENSE) / [出典調査](docs/licensing.md)

## 利用

以下のコマンドは `kyopro/lib/annkake_lib` をカレントディレクトリとして実行します。公開用ZIPを展開した場合は展開先が同じルートになります。

```cpp
#include "algo/graph/dsu.hpp"
#include "algo/string/calc_next.hpp"

DSU dsu(5);
dsu.unite(0,2);
auto next = calc_next("abac");
```

AtCoder用は `templates/atcoder.cpp`、ICPC用は `templates/icpc.cpp`、AHCの単一ファイル版は `heuristic/submit.cpp` を使用します。

## C++スニペット

`snippets/cpp.code-snippets` をVS Codeのユーザースニペット用フォルダー、または公開先プロジェクトの `.vscode/` にコピーしてください。C++の `cpp.json` に内容をコピーすることもできます。

- テンプレート: `atcoder`（別名 `ma` / `m`）、`icpc`（`ICPC`）、`ahc`（`mahc`）。
- 公式評価接続: `ahc_official`（全体例）、`ahc_official_bridge`（既存AHCテンプレートへの追加）。
- ライブラリ: `dsu`、`digit_dp`、`calc_next` などファイル名と同じprefix。
- 関数・クラス定義はsolve/mainの外側へ挿入します。全prefixの対応表は各テンプレート末尾にあります。
- ライブラリ用スニペットは `#include` を含みません。テンプレートが読み込む `<bits/stdc++.h>` を利用します。テンプレートを使わない場合は、標準ヘッダーを先に読み込むか、必要な `.hpp` を直接includeしてください。元の `.hpp` には単独利用に必要なincludeを残しています。
- `comb` の前に `mod_pow`、`modint` の前に `mod_inv` / `mod_pow` を各1回だけ挿入します。
- 定義は原本から生成します。スニペットファイルを直接編集する必要はありません。

## 更新と検証

```sh
python3 tools/sync_templates.py
python3 tools/sync_snippets.py
python3 tools/sync_templates.py --check
python3 tools/sync_snippets.py --check
python3 tests/test_algo.py
python3 tests/test_snippets.py
python3 tests/test_templates.py
python3 tests/test_official_score.py
python3 tests/test_package.py
```

## 公開用パッケージ

```sh
python3 tools/package.py
```

`dist/annkake_lib.zip` に公開対象だけを収録します。`../.local/notes/` と `../.local/reference/` は収録しません。ZIP内の `release-manifest.json` に収録ファイルとSHA-256を記録します。

非公開資料はリポジトリの外側にある `../.local/` に保持しています。防御用に `.gitignore` と `git archive` の設定でも `.local/` を除外します。

`examples/cheats/` は指定により公開パッケージへ含めますが、既存コードの個別の権利確認は今回行っていません。CC0の一括適用対象ではありません。
