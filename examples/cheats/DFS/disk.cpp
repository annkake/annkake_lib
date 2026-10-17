// 参照URL: https://github.com/annkake/annkake_lib
#include <bits/stdc++.h>
using namespace std;

const long long MOD1 = 1000000007;
const long long MOD2 = 998244353;
#define all(v) v.begin(), v.end()
#define rall(a) a.rbegin(), a.rend()
#define fi first
#define se second
#define endl "\n"

#define chmax(x, y) x = max(x, y)
#define chmin(x, y) x = min(x, y)

template <typename T>
using vc = vector<T>; // prioriy_queueに必要なのでここにこれ書いてます
template <typename T>
using vv = vc<vc<T>>;
#define int long long
using ll = long long;
ll INF = 2e18;

#ifdef ONLINE_JUDGE
// ジャッジ環境ではデバッグマクロを無効化
#define debug_x(x) \
    do             \
    {              \
    } while (0)
#define debug_vc(v) \
    do              \
    {               \
    } while (0)
#define debug_vv(v) \
    do              \
    {               \
    } while (0)
#else
// ローカルではデバッグ出力
#define debug_x(x) cout << #x << " = " << (x) << endl
#define debug_vc(v)                    \
    {                                  \
        cout << #v << endl;            \
        ll n = size(v);                \
        rep(i, n) cout << v[i] << ' '; \
        cout << endl;                  \
    } // 一次元配列を出力する
#define debug_vv(v)                                        \
    {                                                      \
        cout << #v << endl;                                \
        ll n = size(v);                                    \
        rep(i, n)                                          \
        {                                                  \
            rep(j, size(v[i])) { cout << v[i][j] << ' '; } \
            cout << endl;                                  \
        }                                                  \
    } // 二次元配列を出力する
#endif

#define rep(i, n) for (ll i = 0; i < (n); ++i)
#define rrep(i, n) for (ll i = 1; i <= (n); ++i)
#define drep(i, n) for (ll i = (n) - 1; i >= 0; --i)
#define nfor(i, s, n) for (ll i = s; i < n; i++)        // i=s,s+1...n-1 ノーマルfor
#define dfor(i, s, n) for (ll i = (s) - 1; i >= n; i--) // s-1スタートでnまで落ちる

using ld = long double;
using bl = bool;

template <class T>
using pq = priority_queue<T, vc<T>>; // 大きい順
template <class T>
using pq_g = priority_queue<T, vc<T>, greater<T>>; // 小さい順

using vl = vc<ll>;
using vvl = vv<ll>;
using vvvl = vv<vl>;
using vvvvl = vv<vvl>;
using vs = vc<string>;
using vvs = vv<string>;
using vb = vc<bl>;
using vvb = vv<bl>;
using vvvb = vv<vb>;
using vld = vc<ld>;
using vvld = vv<ld>;
using vvvld = vv<vld>;
using P = pair<ll, ll>;
using vP = vc<P>;
using vvP = vc<vP>;
using vvvP = vv<vP>;

#define pb push_back

#define YES cout << "Yes" << endl
#define NO cout << "No" << endl
#define YN                     \
    {                          \
        cout << "Yes" << endl; \
    }                          \
    else                       \
    {                          \
        cout << "No" << endl;  \
    } // if(a==b)YN;
#define dame cout << -1 << endl

string dir = "DRUL";   // 第4象限の場合
vl di = {1, 0, -1, 0}; // vl di={1,1,0,-1,-1,-1,0,1};
vl dj = {0, 1, 0, -1}; // vl dj={0,1,1,1,0,-1,-1,-1};

bool out_grid(ll i, ll j, ll h, ll w)
{ // trueならcontinue
    return (!(0 <= i && i < h && 0 <= j && j < w));
}

ll n;
vl x, y, r, c;
vb visited;

ll toState(vb &eliminated)
{
    ll idx = 0;
    rep(i, n)
    {
        if (eliminated[i])
        {
            idx += 1 << i;
        }
    }
    return idx;
}

ll dfs(vb &eliminated)
{
    // debug_vc(eliminated);
    ll ans = 0;
    vl board;
    rep(i, n)
    {
        if (eliminated[i])
            continue;
        bl ok = true;
        rep(j, i)
        {
            if (eliminated[j])
                continue;
            // ll j = board[ii];
            ll d = (x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]);
            if ((r[i] + r[j]) * (r[i] + r[j]) > d)
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            board.pb(i);
        }
    }

    rep(i, board.size())
    {
        rep(j, board.size())
        {
            if (i >= j)
                continue;
            if (c[board[i]] == c[board[j]])
            {
                eliminated[board[i]] = true;
                eliminated[board[j]] = true;
                ll state = toState(eliminated);
                if (!visited[state])
                {
                    visited[state] = true;
                    ans = max(ans, 2 + dfs(eliminated));
                }
                eliminated[board[i]] = false;
                eliminated[board[j]] = false;
            }
        }
    }

    // debug(board);
    // vvl colSet(4);
    // rep(i, board.size())
    // {
    //     colSet[c[board[i]] - 1].pb(board[i]);
    //     // rep(j, board.size())
    //     // {
    //     //     if (i >= j)
    //     //         continue;
    //     //     if (c[board[i]] == c[board[j]])
    //     //     {
    //     //         eliminated[board[i]] = true;
    //     //         eliminated[board[j]] = true;
    //     //         ans = max(ans, 2 + dfs(eliminated));
    //     //         eliminated[board[i]] = false;
    //     //         eliminated[board[j]] = false;
    //     //     }
    //     // }
    // }
    // bl update = false;
    // rep(i, 4)
    // {
    //     if (colSet[i].size() >= 2)
    //     {
    //         update = true;
    //     }
    // }
    // if (!update)
    //     return 0;
    // ll res = 0;
    // rep(i, 4)
    // {
    //     if (colSet[i].size() % 2 == 0)
    //     {
    //         // 全部取り除ける
    //         rep(j, colSet[i].size())
    //         {
    //             eliminated[colSet[i][j]] = true;
    //             res++;
    //         }
    //     }
    // }

    // bl dive = false;
    // rep(i, 4)
    // {
    //     if (colSet[i].size() % 2 == 1)
    //     {
    //         // 1枚余る
    //         rep(j, colSet[i].size())
    //         {
    //             eliminated[colSet[i][j]] = true;
    //             res++;
    //         }
    //         rep(j, colSet[i].size())
    //         {
    //             eliminated[colSet[i][j]] = false;
    //             res--;
    //             dive = true;
    //             ans = max(ans, res + dfs(eliminated));
    //             eliminated[colSet[i][j]] = true;
    //             res++;
    //         }
    //         rep(j, colSet[i].size())
    //         {
    //             eliminated[colSet[i][j]] = false;
    //             res--;
    //         }
    //     }
    // }

    // if (!dive)
    // {
    //     ans = max(ans, res + dfs(eliminated));
    // }

    // rep(i, 4)
    // {
    //     if (colSet[i].size() % 2 == 0)
    //     {
    //         // 全部取り除ける
    //         rep(j, colSet[i].size())
    //         {
    //             eliminated[colSet[i][j]] = false;
    //             res--;
    //         }
    //     }
    // }

    return ans;
}

bl solve()
{
    cin >> n;
    if (n == 0)
        return 0;
    x.resize(n);
    y.resize(n);
    r.resize(n);
    c.resize(n);
    rep(i, n)
    {
        cin >> x[i] >> y[i] >> r[i] >> c[i];
    }
    // 全探索(DFS)
    // 重なっている⇔2r>2頂点間の距離
    // bit全探索
    // rep(i,1<<n){
    //     rep(j,n){
    //         if(i&(1<<j))continue;
    //         rep(k,j){
    //             if(i&(1<<k))continue;
    //         }
    //     }
    // }
    visited.assign(1<<n,false);
    vb eliminated(n);
    cout << dfs(eliminated) << endl;

    return 1;
}

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    while (true)
    {
        if (!solve())
            break;
    }
    return 0;
}

/*
shift+alt+Fで成形
f2で変数名変更

スニペット
basic
 m:atcoder以外用
 ma:ABC/ARC用
 mahc:AHC用
math
 floor:床関数
 ceil:天井関数
 comv:二項係数
 modpow: a^b mod m
 GCD:最大公約数
 extGCD:拡張ユークリッド互除法
 PFD:素因数分解
 isprime:エラトステネスの篩
 matrixpow:行列累乗
data_structure
 seg:セグメント木
technic
 Compress:座標圧縮
 runlength:ランレングス圧縮
graph
 warshall:ワーシャルフロイド法
 dfs:深さ優先探索
 bfs:幅優先探索
 LCA:最近共通祖先
*/
