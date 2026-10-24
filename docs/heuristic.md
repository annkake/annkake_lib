# AHCテンプレート v3 — 戦略切替・差分更新・計測・復元検査

## 0. ファイル

- `heuristic/submit.cpp`：**そのまま単一ファイルとしてコンパイルできる**提出用（共通ライブラリも内蔵）。
- `heuristic/template.cpp`：開発用。`#include "core.hpp"` で分割管理。
- `heuristic/core.hpp`：共通部品。別のAHC問題へコピーして再利用する。
- `heuristic/rng.hpp`：乱数生成器だけを使う単独ヘッダー。スニペットprefixは `rng`（別名 `RNG`）。
- `tests/heuristic_test.cpp`：各部品の単体テスト。
- `tests/heuristic_bad_undo.cpp`：わざと壊した `undo` を検出できることを示すテスト（失敗が正常）。

## 乱数生成器だけを使う

```cpp
#include "heuristic/rng.hpp"

RNG rng;
int index = rng.uniform_int(0, 10); // [0,10) の整数
double p = rng.uniform01();       // [0,1)
double positive = rng.uniform01_open(); // (0,1)
```

スニペットは `rng` で構造体をsolve/mainの外側へ挿入します。標準ヘッダーを読み込んだ後なら、`#define int long long` の後に展開・includeしても使えます。`uniform_int(lo,hi)` の上下限と戻り値も64bitになり、最小値から最大値付近までの範囲を扱えます（上端は含みません）。`lo >= hi` や符号なし版の上限0は例外になります。

AHCテンプレートには同じ実装が `ahc::RNG` として内蔵されているため、そのまま利用できます。原本は `heuristic/rng.hpp` で、`tools/sync_templates.py` が内蔵版と提出版へ同期します。乱数の初期状態は固定なので、同じ操作列からは同じ乱数列が得られます。

**注意：付属の問題は説明用の架空例題です。** N個からK個を選んで整数の総和を最大化します。実際のAHCでは `Input`、`Output`、`Action`、`Move`、`SAState`、`BeamState`、`greedy`、`official_score` と各探索の近傍・ロールアウトを置き換えてください。

## 1. 提出時にすぐ戦略を切り替える

ファイル冒頭で、**コンパイルコマンドを変更せず**に切り替えます。

```cpp
enum class Strategy { Greedy, SA, TreeBeam, MonteCarlo, All };
constexpr Strategy SEARCH_STRATEGY = Strategy::All;
constexpr bool SHOW_DEBUG = false;
constexpr bool CHECK_UNDO = true;
constexpr double TIME_LIMIT = 1.90;
```

- `Greedy`: とりあえず動く解。開発初期のAC確認に利用。
- `SA`: 完成解への局所変更（焼きなまし）。
- `TreeBeam`: 木上でDFSして操作をapply/undoする差分更新ビーム。
- `MonteCarlo`: 各操作をシミュレーションして平均評価値の良い操作を選ぶ。
- `All`: 初期貪欲解を保存した後、**SA→木上Beam→MC** の順に実行し、公式スコア最大の合法な完成解を選ぶ。

`All` モードは残り時間の40%をSA、次の35%をBeam、最後の25%をMCに割り当てます。実問題では分配比率を調整してください。`Timer` を共有し、終了時刻はコンテスト開始からの絶対秒で扱います。

`CHECK_UNDO = true` は開発用の安全設定です。大きなStateでは各操作の試し実行とスナップショットコピーが重いので、**十分に検査してから提出用の最適化時に `false` に切り替えてください**。`false` の場合でも木上ビームはscoreの最低限の整合性検査を行います。

## 2. 実行とテスト

以下は `kyopro/lib/annkake_lib` をカレントディレクトリとして実行します。

```bash
# 提出用の1ファイル
 g++ -std=c++20 -O2 -Wall -Wextra heuristic/submit.cpp -o main
 ./main < input.txt > output.txt 2> stats.log

# 開発用（include版）
 g++ -std=c++20 -O2 heuristic/template.cpp -o main

# 単体テスト
 g++ -std=c++20 -O2 -Wall -Wextra tests/heuristic_test.cpp -o selftest
 ./selftest

# sanitizerを付ける場合
 g++ -std=c++20 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer tests/heuristic_test.cpp -o selftest_san
 ./selftest_san

# 不正なundoの検出（非ゼロ終了・assertが正解）
 g++ -std=c++20 -O1 -g tests/heuristic_bad_undo.cpp -o bad
 ./bad
```

サンプル入力:

```text
6 3
4 9 2 8 1 7
```

サンプル出力: `1 3 5` （0-indexed）。

## 3. print_debugで何が出せる？

`debug(x)` / `AHC_DEBUG(enabled, x)` は `-DLOCAL` 指定時のみ有効です。サンプルの main は LOCAL 時に `LocalOutput` を生成し、通常終了後に `[cerr]`、`[cout]` の順で元の標準エラーへまとめて表示します。LOCAL 未指定時は通常の stdout/stderr を使用します。`endl` の改行文字列への置き換えも LOCAL 時のみです。出力分離の詳細は [ALGO側のREADME](templates.md) を参照してください。

| 型 | 対応 | 表示例 |
|---|---|---|
| 数値・bool・文字列 | ○ | `42`, `1`, `abc` |
| `pair` | ○ | `(1, 2)` |
| `tuple` / `array` | ○ | `(1, 2, 3)` |
| `vector`, `set`, `map`, `unordered_map` | ○ | `{(1, 5), (2, 9)}` |
| `vector<vector<int>>` など多重コンテナ | ○ | `{{1, 2}, {3, 4}}` |
| `optional` | ○ | `some(10)` / `nullopt` |
| `variant` | ○ | `variant#1(10)` |
| 独自の `struct` / `class` | **○（設定が必要）** | `debug_fields()` を実装する |
| `queue` / `priority_queue` など | 自動では未対応 | `vector`へ移して表示する |

例えば構造体は以下のように実装します。

```cpp
struct State {
    int turn = 0;
    int score = 0;
    vector<int> board;

    auto debug_fields() const {
        return make_tuple(
            pair{"turn", turn},
            pair{"score", score},
            pair{"board", board}
        );
    }
};

State state;
print_debug(state);             // 無条件でcerrに表示
AHC_DEBUG(SHOW_DEBUG, state);         // LOCAL指定かつSHOW_DEBUGがtrueのときだけ
AHC_DEBUG(SHOW_DEBUG, state.score);
```

`debug_fields()` の代わりに `operator<<(ostream&, const State&)` も利用できます。通常の `debug_fields` は参照を返してもその場で表示するだけなので構いません。

**注意：`debug_snapshot()` はこれと異なり、必ず全可変値のコピーを返します。** `tie(board, score)` のような参照タプルにすると復元失敗を検出できません。`tuple{board, score, hash, cache, ...}` のように書いてください。

## 4. apply・undo復元デバッグ（重要）

State側で以下の3つを用意します。

```cpp
struct State {
    vector<int> board;
    int score = 0;

    struct Undo { /* 元の値を格納 */ };
    Undo apply(const Action& a);
    void undo(const Action& a, const Undo& backup);

    auto debug_snapshot() const {
        return tuple{board, score}; // 全可変情報の値コピー
    }
};
```

`CHECK_UNDO=true` 時にはライブラリ内の `ahc::check_apply_undo(state, action, true, "場所")` が

1. 操作前の全スナップショットを保存
2. `apply(action)` を実行
3. `undo(action, backup)` を実行
4. `assert(before == after)` を検査（`-DNDEBUG` 環境では例外で検出）
5. 問題なければ本来の探索を実行

という順で動きます。SAは**採用される場合にも事前検査**を行い、棄却時には実際のundoを追加検査します。木上ビームでは候補生成およびDFSの各辺で同様の検査を行い、実際のundo後も照合します。MCの最初の操作も検査します。

**重要：ロールアウト内部や自作の貪欲法で直接 `apply` を呼ぶ場合は、必要に応じて自分で `check_apply_undo` を呼んでください。** ライブラリから見えない操作は自動チェックできません。テンプレートのMCサンプルではロールアウト内部にもチェックを追加済みです。

スナップショットに含めなかったキャッシュ、不正な合法手列挙、真のスコア関数の誤りまでは自動検出できません。復元の検査をしたからといって問題固有の実装が全面的に正しいと保証されるわけではありません。

## 5. 絶対スコアと最良Outputの共有

```cpp
ahc::BestOutput<Output, int> best;
auto record = [&](const auto& state) {
    if (!state.is_complete()) return;      // 未完成・不正な解を記録しない
    best.consider(state.absolute_score(), ahc::Goal::Maximize,
                  [&] { return state.make_output(); });
};
```

最初に貪欲解を保存し、SA・Beam・MCが候補を生成したときに `record` に渡します。**SAで棄却された候補やビーム幅に選ばれなかった候補でも、完成した合法解なら記録可能**です。最後に `best.output->print()` とします。

**比較指標は探索用 `state.score` でなく公式の絶対スコア**です。公式の整数スコアに加え、実問題では合法性判定も必須です。MCではロールアウト側からも`record`を呼び出せます。

## 6. 統計・ログ

ログはすべて `cerr` に出力します。標準出力は答えだけです。

- SA：全試行・有効候補・採用数・採用率・近傍ごとの採用率、**0〜100%の時間帯ごと**の採用率と近傍別採用率。`cfg.time_bins=10`。
- 木上Beam：各ターンの展開数・遷移数・採用数・**異なるStateハッシュの数と比率**・採用候補が由来する親ノードの種類・各ターン時間・累積時間。部分的にしか列挙できなかった層は「確定=No」と表示。`cfg.track_diversity=true` は `State::hash()` が必要です。
- MC：候補ごとのロールアウト試行数・平均・標準偏差・**上位のActionとスコア**、全試行の最小/最大/平均/標準偏差。`MCStats::add` でターン数、平均時間、最大時間を追跡。
- BFS：`bfs.stats.print()` で呼出回数・合計訪問頂点数・一回当たり平均時間・最大時間。
- 時空間BFS：`time_space.bfs.stats.print()` を使用。

※ ハッシュの異なる数は「多様性の近似指標」です。異なる盤面が同じハッシュになる可能性があり、逆に「互いに似た盤面」の分析は行いません。`cfg.deduplicate=true` は衝突時に本来異なる状態を捨てる可能性があります。

## 7. 初期化しないBFS

```cpp
ahc::StampedBFS bfs(N); // 1回だけ作る
for (int start = 0; start < N; ++start) {
    auto res = bfs.run(start,
        [&](int v, auto push) {
            for (int to : graph[v]) push(to);
        },
        [&](int v) { return v == target; }
    );
    if (res.found) {
        int d = bfs.distance(target);
        vector<int> path = bfs.path(target);
    }
}
bfs.stats.print();
```

毎回 `seen` を0で埋めません。呼出ごとに32bitの `stamp` を増やすだけで、カウンタが周回するときだけ訪問配列を初期化します。`dist` と `parent` は訪問済みの頂点についてのみ有効です。

## 8. 時空間BFS

```cpp
ahc::TimeSpaceBFS bfs(H * W, T);
auto res = bfs.run(start_y * W + start_x, 0,
    [&](int v, int next_t, auto push) {
        int y = v / W, x = v % W;
        for (int k = 0; k < 4; ++k) {
            int ny = y + di[k], nx = x + dj[k];
            if (!out_grid(ny, nx, H, W)) push(ny * W + nx);
        }
    },
    [&](int t, int v) { return !blocked[t][v]; },
    [&](int t, int v) { return v == target; },
    true // 同じ位置での待機を許可する
);
if (res.found) {
    auto path = bfs.path(res.goal); // vector<pair<時刻, 位置>>
}
bfs.bfs.stats.print();
```

メモリ使用量は `O((T+1)*位置数)`。時刻が周期的に繰り返す問題では、必要に応じて時刻を周期でまとめる別実装の方が軽量です。

## 9. BitPacker: 範囲からshift/maskを自動計算

```cpp
// i,j: 0..19, k: 0..7, d: 0..3, l,f: 0..15
ahc::BitPacker<6> codec({{{0,19},{0,19},{0,7},{0,3},{0,15},{0,15}}});
codec.describe();
// width: {5,5,3,2,4,4}
// shift: {0,5,10,13,15,19}、合計23bit
uint64_t bits = codec.encode({18, 2, 7, 1, 15, 6});
auto values = codec.decode(bits);
int64_t j = codec.get(bits, 1);
bits = codec.set(bits, 1, 10);
uint64_t j_mask = codec.mask[1];
```

下限が0でない符号付きの範囲にも対応。範囲外や合計64bit超過は例外。固定値（幅0）と64bit単独フィールドにも対応。`__int128` を範囲計算に利用するためGCC/Clang向けの実装です。

## 10. モンテカルロ法のAPI

```cpp
ahc::MonteCarloConfig cfg;
cfg.simulations_per_action = 32;
cfg.deadline_sec = 1.90;
cfg.goal = ahc::Goal::Maximize;
cfg.top_k = 5;
cfg.log_stats = true;
cfg.check_undo = CHECK_UNDO;

auto rollout = [&](State& s, ahc::RNG& rng) -> double {
    // sには初手が適用されている。残りは自分でシミュレーションする。
    // 完成解はrecord(s)で保存してよい。
    return evaluate_final(s);
};
auto result = ahc::monte_carlo<Action>(state, cfg, timer, rng, rollout);
if (result.best_action) state.apply(*result.best_action);
```

この素朴版は「Actionごとに状態をコピーして、決められた回数だけシミュレーションする」方式です。時間切れによる候補間の偏りを減らすため、候補はラウンドロビンで試します。大きなStateや多数のActionではロールアウトを差分更新化するのが今後の高速化対象です。MCの戻り値は**候補の推定評価**であって、必ずしも絶対スコアではありません。

## 11. 本番の立ち回りに沿った作業順序

1. **AC確認**：`Greedy` を選んでInput・Output・合法性・公式スコア関数を実装。配布スコアラと自作スコアを小ケースで突き合わせる。
2. **考察・実験**：Stateを実装してSA / TreeBeam / MCをそれぞれ試す。時間帯ごとの採用率・候補多様性・MCの得点分布から改善方針を決める。
3. **複数戦略**：`All` で時間配分し、共通 `BestOutput` に解を集約する。
4. **高速化**：`CHECK_UNDO=true` で正当性を検査した後、差分更新、不要候補の枝刈り、BitPacker、Stamp BFSなどに切り替える。`CHECK_UNDO=false` で性能を再測定する。

### 現在の設計上の制限

- TreeBeamは探索木を再帰DFSします。極端に深い探索ではスタックが足りなくなるため、反復DFSに変更してください。
- TreeBeamは全候補を保存・ソートします。非常に大きな分岐数では上位K候補だけを保持するSelectorへの置換を検討してください。
- `debug_snapshot()` は全可変情報を値コピーして初めて検査として機能します。戻し忘れは検出できますが、誤った評価・不正な合法手まで保証しません。
- Time-limit判定はSA / Beamの一定間隔、MCは試行単位です。提出時は安全マージンを持たせてください。
- 公式スコアは可能なら配布プログラムとランダムケース比較してから採用してください。コンテストごとの生成AI利用・翻訳ルールも確認してください。

共通マクロと型エイリアスの原本・同期方法は [ALGO側のREADME](templates.md#共通定義の管理) を参照してください。AHCの `int` は通常の32bitを維持しています。

## 公式スコア関数との接続

`OfficialRecorder` を使う追加例を統合しました。[公式スコア取り込みガイド](official_score.md) を参照してください。既存の探索用coreを共有し、公式変換コードと問題固有のアダプターを分離しています。
