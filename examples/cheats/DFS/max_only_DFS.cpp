// 参照URL: https://github.com/annkake/annkake_lib
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int max_ans = 0;
vector<long long> adj;

// cand  : 現在選べる頂点（科目）の候補をビットマスクで管理
// count : 今までに選んだ科目の数
void dfs(long long cand, int count) {
    if (cand == 0) {
        if (count > max_ans) max_ans = count;
        return;
    }
    // 【枝刈り】今選んでいる数と、残りの候補を全部選んだとしても max_ans に届かない場合は打ち切る
    if (count + __builtin_popcountll(cand) <= max_ans) return;

    // 候補の中から最もインデックスの若いビット（頂点）を1つ選ぶ
    int v = __builtin_ctzll(cand);
    
    // ① 頂点 v を「使う」場合の分岐
    // v自身と、vと矛盾する頂点（adj[v]）を候補(cand)から一気に除外する
    dfs(cand & ~adj[v] & ~(1LL << v), count + 1);
    
    // ② 頂点 v を「使わない」場合の分岐
    // vだけを候補から除外する
    dfs(cand ^ (1LL << v), count);
}

bool solve() {
    int n, m;
    if (!(cin >> n >> m) || (n == 0 && m == 0)) {
        return false;
    }

    vector<vector<int>> a(n, vector<int>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
        }
    }

    vector<long long> original_adj(m, 0);

    // グラフの構築（矛盾する科目のペアに辺を張る）
    for (int i = 0; i < m; i++) {
        // 科目 i の点数で、日数を昇順ソートする
        vector<pair<int, int>> p(n);
        for (int k = 0; k < n; k++) {
            p[k] = {a[k][i], k};
        }
        sort(p.begin(), p.end());

        for (int j = i + 1; j < m; j++) {
            bool ok = true;
            int prev_max = -1;
            
            int cur_min = 1e9;
            int cur_max = -1;
            int cur_val = p[0].first;

            // 科目 i でソートされた順序に従って科目 j の点数を見ていく
            for (int k = 0; k < n; k++) {
                if (p[k].first != cur_val) {
                    // 同点のグループが切り替わるタイミングでチェック
                    if (prev_max > cur_min) {
                        ok = false;
                        break;
                    }
                    prev_max = max(prev_max, cur_max);
                    cur_val = p[k].first;
                    cur_min = 1e9;
                    cur_max = -1;
                }
                int val_j = a[p[k].second][j];
                if (val_j < cur_min) cur_min = val_j;
                if (val_j > cur_max) cur_max = val_j;
            }
            if (ok && prev_max > cur_min) {
                ok = false;
            }

            // 同時に単調増加にできない場合はお互いを隣接（矛盾）させる
            if (!ok) {
                original_adj[i] |= (1LL << j);
                original_adj[j] |= (1LL << i);
            }
        }
    }

    // 【高速化の工夫】次数が少ない頂点ほど独立集合に含めやすいので、
    // インデックスが小さくなるようにソートする（DFSの枝刈りが劇的に早く発動するようにする）
    vector<pair<int, int>> deg(m);
    for (int i = 0; i < m; i++) {
        deg[i] = {__builtin_popcountll(original_adj[i]), i};
    }
    sort(deg.begin(), deg.end());

    // ソート後のインデックスに合わせて隣接行列(ビットマスク)を再構築
    adj.assign(m, 0);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            if ((original_adj[deg[i].second] >> deg[j].second) & 1) {
                adj[i] |= (1LL << j);
            }
        }
    }

    // 最大独立集合の探索を実行
    max_ans = 0;
    dfs((1LL << m) - 1, 0);
    cout << max_ans << "\n";

    return true;
}

int main() {
    // 入出力の高速化
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    while (solve());
    
    return 0;
}