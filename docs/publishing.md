# GitHubへの公開手順

公開するGitリポジトリのルートは `kyopro/lib/annkake_lib/` です。親の `kyopro/lib/` では初期化しません。

```text
kyopro/lib/
├── .local/          非公開のNotes・参考資料・元のインポート資料
└── annkake_lib/     公開するリポジトリ
    ├── algo/
    ├── heuristic/
    ├── templates/
    └── ...
```

## 1. 公開する内容の確認

以下はローカル環境で実行します。Git・Python 3・C++20対応のg++が必要です。

```sh
cd /home/annkake/projects/kyopro/lib/annkake_lib
python3 tools/sync_templates.py --check
python3 tools/sync_snippets.py --check
python3 tests/test_package.py
```

ライセンスは [LICENSE](../LICENSE) と [権利確認記録](licensing.md) の範囲に限定しています。`examples/cheats/`・既存のテンプレート・ヒューリスティックコード・公式由来の変換コードなどは、一括のCC0適用対象ではありません。許諾未確認のコードは権利確認が残っています。公開フォルダーに移したことや、ZIPの検証に成功したことは、権利確認の完了を意味しません。

## 2. ローカルGitリポジトリを作る

初回のみ実行します。必要なら事前に `git config --global user.name` と `git config --global user.email` でコミット名義を設定してください。

```sh
git init -b main
git add .
git status --short
git diff --cached --name-only
```

表示された追加対象を確認してください。`../.local/` はリポジトリの外側です。`dist/` とPythonキャッシュなどは `.gitignore` により追加されません。確認後にコミットします。

```sh
git commit -m "Initial release of annkake_lib"
```

## 3. GitHubでPublicリポジトリを作り、送信する

GitHub CLI（`gh`）を導入済みの場合は、次のコマンドで認証・リポジトリ作成・初回pushを行えます。これはインターネットへの公開操作です。

```sh
gh auth login
gh repo create annkake_lib --public --source=. --remote=origin --push
gh repo view --web
```

GitHub CLIを使わない場合は、GitHubの「New repository」で名前を `annkake_lib`、公開設定を「Public」にします。README・.gitignore・Licenseの自動追加はすべて選択せず、空のリポジトリとして作成します。その後、GitHub用の認証を済ませ、表示されたURLを指定します。下記の `YOUR_USERNAME` は実際のアカウント名に置き換えてください。

```sh
git remote add origin https://github.com/YOUR_USERNAME/annkake_lib.git
git remote -v
git push -u origin main
```

CLIを使う方法とブラウザーで作る方法は、どちらか一方だけ実行します。

## 4. 公開後の更新

原本を編集したら生成物を同期し、READMEの検証コマンドを実行したうえでコミット・pushします。

```sh
python3 tools/sync_templates.py
python3 tools/sync_snippets.py
git add .
git diff --cached --stat
git commit -m "Update library"
git push
```

ZIP配布も行う場合は `python3 tools/package.py` で `dist/annkake_lib.zip` を生成し、GitHubのReleases画面から添付できます。ZIPには許可リスト内のファイルだけを含めます。この許可リストはGitへの追加には適用されません。

## 公式資料

- [GitHub: ローカルコードの追加](https://docs.github.com/en/migrations/importing-source-code/using-the-command-line-to-import-source-code/adding-locally-hosted-code-to-github)
- [GitHub CLI: gh repo create](https://cli.github.com/manual/gh_repo_create)
