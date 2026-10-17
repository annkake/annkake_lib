# 公式スコア関数の取り込み

追加された `ahc_patch_v4` を既存のAHC共通部へ統合しました。完成した解を公式由来の評価関数へ渡し、合法な候補の中から最良の解を保存します。スコアが0でも合法なら保存します。

## ファイル

| ファイル | 役割 |
|---|---|
| [official_bridge.hpp](../heuristic/official_bridge.hpp) | `ScoreResult<Score>`、`OfficialRecorder<Output,Score>` |
| [core.hpp](../heuristic/core.hpp) | 既存の探索・Goal・BestOutput。別コピーは作らない |
| [translated_case_b.hpp](../examples/official_score/translated_case_b.hpp) | 提供された公式変換コードのInput・Output・シミュレーター・採点関数 |
| [case_b_adapter.hpp](../examples/official_score/case_b_adapter.hpp) | 問題固有の入出力・合法性検査・初期解・最適化方向 |
| [main_case_b.cpp](../examples/official_score/main_case_b.cpp) | 公式評価を使う最小の実行例 |
| [official_submit.cpp](../heuristic/official_submit.cpp) | 上記の依存を展開した単一ファイル例（生成物） |

これはcase Bの入出力・採点に接続した例です。別の問題では `translated_case_b.hpp` とアダプターをその問題のコードに置き換えます。Greedyの完成解を保存するところまで実装しており、SA・Beam・MCの問題固有の探索は別途接続します。

## 評価値と合法性

```cpp
ahc::OfficialRecorder<problem::Output, long long> recorder(
    problem::GOAL,
    [&](const auto& out) { return problem::official_score(in, out); }
);
recorder.consider(problem::greedy(in));
// 新たな完成解: recorder.consider(candidate);
if (recorder.best.output) problem::print_output(cout, *recorder.best.output);
```

`official_score` は `ScoreResult::valid(score)` または `ScoreResult::invalid(error)` を返します。`consider` の返り値は「合法かつ最良解を更新したか」であり、falseは無効解だけでなく同点・悪化も含みます。初期解は最初に評価し、その後も合法解が得られなければ出力しないようにします。評価関数の例外は呼び出し元へ伝播します。

case Bのアダプターは出力の要素数を確認し、提出形式に直した候補を同梱の `parse_output` でも検査してから採点します。これにより、座標範囲や `[0,M)` のaction制約を、採点関数単体より先に確認します。評価対象は完成したOutputです。高速な探索評価とは分けて呼び出してください。

`Goal::Maximize` / `Goal::Minimize` は返した数値の大小方向に合わせます。スコア型は公式計算の範囲に合わせて指定し、公式変換コードに `#define int long long` を追加しないでください。

## 実行・単一ファイル化

ライブラリのルートで実行します。

```sh
g++ -std=c++20 -O2 examples/official_score/main_case_b.cpp -o main
printf '2 3\n0 0\n0 1\n1 0\n1 1\n' | ./main
python3 tools/make_submit.py examples/official_score/main_case_b.cpp -o dist/official_submit.cpp
g++ -std=c++20 -O2 dist/official_submit.cpp -o submit
```

`-DLOCAL` 指定時は既存のLocalOutputによりcerr/coutを分離して表示します。未指定時は解だけをstdoutへ出力します。例の入力処理はstdin全体を読み込む非対話型です。

`make_submit.py` はライブラリ内の引用includeを依存順に一度だけ展開し、`#pragma once` と重複した `<bits/stdc++.h>` を除きます。循環includeはエラーにします。このライブラリの無条件の引用includeを対象としており、条件付きの引用includeやマクロを使うincludeの一般的なプリプロセッサーではありません。

既定の生成物を更新する場合:

```sh
python3 tools/sync_templates.py
python3 tools/sync_snippets.py
```

## スニペット

- `ahc_official`: 公式スコア接続の単一ファイル例を展開する。新しいファイルで使い、case Bの問題固有部分を変更する。
- `ahc_official_bridge`: `ScoreResult` と `OfficialRecorder` だけを展開する。既存の `ahc` テンプレートへ追加する場合に使う。AHC共通部（Goal・BestOutput）が定義済みである必要がある。

`ahc_official_bridge` にincludeはありません。通常のALGO/ICPCテンプレートだけではGoalとBestOutputが定義されないため、AHC用テンプレートと組み合わせます。`ahc_official` は共通部とbridgeを既に含むので、同じ定義を追加で展開しないでください。

## 検証・出典

```sh
python3 tests/test_official_score.py
```

最大化・最小化・0点・不正解・同点、case Bのランダムな合法/不合法出力、複数翻訳単位での利用、分割版と単一ファイル版・スニペットの出力一致を検証しています。同梱C++評価と接続部に加え、ローカルAHC070のRust製ビジュアライザーと合法・不合法30ケースのスコア一致を確認しました。追加の照合は `python3 tests/test_official_score.py --rust-vis /path/to/tools/target/release/vis` で実行できます。この有限個の照合は、公式配布物の原本同一性や全入力での同値性を保証するものではありません。

指定されたページは [AHC071 A](https://atcoder.jp/contests/ahc071/tasks/ahc071_a) ですが、同梱case Bの `N/M/targets` 形式はローカルAHC070ツールに対応し、ローカルAHC071ツールの `W/H/costs/holes` 形式とは異なります。出典候補は [AHC070 A](https://atcoder.jp/contests/ahc070/tasks/ahc070_a) と記録しました。ブラウズツールではページ・配布物を取得できず、確認したローカルツールにも明示的なライセンスは見つからなかったため、配布条件は未確認です。公式変換コードにリポジトリのCC0宣言を自動適用していません。[取り込み記録](../provenance/official_score_import.json) を参照してください。

追加時の元のREADME・チャットガイド・ZIPは `../.local/imports/` に保存しています。説明にあったcase Aや別コピーのcore、生成ツールは同梱されていなかったため、公開ガイドでは現在存在するファイルとAPIに整理しました。
