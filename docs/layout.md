# ディレクトリ構成と公開手順

公開リポジトリのルートは `kyopro/lib/annkake_lib/` です。非公開資料はその外側の `kyopro/lib/.local/` に保持しています。以下の公開ファイルのパスはリポジトリルートからの相対パスです。GitHubへの公開方法は [公開手順](publishing.md) を参照してください。

| 以前の場所 | 現在の場所 |
|---|---|
| `ALGO/DataStructure/` | `algo/data_structure/` |
| `ALGO/DP/` | `algo/dp/` |
| `ALGO/Graph/` | `algo/graph/` |
| `ALGO/Math/` | `algo/math/` |
| `ALGO/String/` | `algo/string/` |
| `ALGO/Technic/` | `algo/technique/` |
| `ALGO/templete/atcoder.cpp` | `templates/atcoder.cpp` |
| `ALGO/templete/ICPC.cpp` | `templates/icpc.cpp` |
| `tools/template_common.hpp.in` | `templates/common.hpp.in` |
| `Heuristic/v3/ahc_core_v3.hpp` | `heuristic/core.hpp` |
| `Heuristic/v3/ahc_template_v3.cpp` | `heuristic/template.cpp` |
| `Heuristic/v3/ahc_submit_v3.cpp` | `heuristic/submit.cpp` |
| AHCのselftest / bad_undo | `tests/heuristic_test.cpp` / `tests/heuristic_bad_undo.cpp` |
| `ALGO/CheetCode/` | `examples/cheats/`（内容は変更せず公開対象） |
| `ALGO/Notes/` | `../.local/notes/`（非公開・内容は保持） |
| `ALGO/DataStructure/lazy_segtree.md` | `../.local/reference/lazy_segtree.md`（非公開・内容は保持） |
| `ALGO/README.md` | `docs/algorithms.md` |
| `ALGO/templete/README.md` | `docs/templates.md` |
| AHCのREADME | `docs/heuristic.md` |
| `ALGO/LICENSE_REVIEW.md` | `docs/licensing.md` |
| `ALGO/LICENSE` / `ALGO/LICENSES/` | `LICENSE` / `LICENSES/`（適用範囲は従来どおり限定） |
| 出典記録のJSON | `provenance/` |
| `snippet.json` | `snippets/cpp.code-snippets` |

旧ディレクトリへの互換コピーは置いていません。include・生成ツール・テスト・ドキュメント・スニペットは新しいパスへ更新しています。出典記録の `old_path` は履歴上の名前を保持し、`new_path` は現在の `algo/` 内の相対パスです。

## 公開するファイル

ルートのREADME・LICENSE・Git設定と、`algo/`、`heuristic/`、`templates/`、`snippets/`、`docs/`、`provenance/`、`LICENSES/`、`tools/`、`tests/`、`examples/` を許可リストとして配布します。この許可リストはZIP作成時の設定です。Gitへの追加は別途 `git add` で指定するため、コミット前に対象を確認してください。

`python3 tools/package.py` はこのルートから実行します。生成物は `dist/annkake_lib.zip` です。収録するファイルとSHA-256はZIP内の `release-manifest.json` で確認できます。

- `.local/`、Notes、Pythonキャッシュ、コンパイル生成物は除外します。
- シンボリックリンクは収録せずエラーにします。
- `.gitignore` は新規コミットへの混入防止、`.gitattributes` は `git archive` での除外に使います。既に追跡されているファイルや過去のコミットには自動適用されません。
- 公開用ZIPにはGit履歴を収録しません。既存リポジトリの履歴削除・外部へのアップロードは行っていません。

公開用ZIPにCheetCodeを含める指定は、第三者由来のコードをCC0へ変更する指定とは扱っていません。既存のライセンス適用範囲は [LICENSE](../LICENSE) と [権利確認記録](licensing.md) に記載しています。

追加された `ahc/ahc_patch_v4/` は、接続部を `heuristic/official_bridge.hpp`、実行例を `examples/official_score/`、ガイドを `docs/official_score.md` に統合しました。元の説明書とZIPは `../.local/imports/` に保存し、公開用ZIPでは統合後のコードを配布します。
