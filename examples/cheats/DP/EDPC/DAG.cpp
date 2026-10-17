#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

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
using mint = modint998244353;
// using mint = modint1000000007;
// using mint = modint;//使うときはコメントアウトを外す
// mint::set_mod(m);//使うときはコメントアウトを外して、solve()内でやろう

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
using vmint = vc<mint>;
using vvmint = vv<mint>;
using vvvmint = vv<vmint>;

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

// DAG
vvl g;
vl dp;

ll dfs(ll v)
{
    if (dp[v] != -1)
        return dp[v];
    ll res = 0;
    for (auto nex : g[v])
    {
        res = max(res, dfs(nex) + 1);
    }
    return dp[v] = res;
}

void solve()
{
    ll n, m;
    cin >> n >> m;
    vl x(m), y(m);
    g.assign(n, vl());
    dp.assign(n, -1);
    set<ll> candStart;
    rep(i, n)
    {
        candStart.insert(i);
    }
    rep(i, m)
    {
        cin >> x[i] >> y[i];
        x[i]--;
        y[i]--;
        g[x[i]].pb(y[i]);
        candStart.erase(y[i]);
    }

    ll ans = 0;
    for (auto s : candStart)
    {
        ans = max(ans, dfs(s));
    }
    cout << ans << endl;

    return;
}

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
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
