// 参照URL: https://github.com/annkake/annkake_lib
#include <bits/stdc++.h>
using namespace std;

const long long MOD1 = 1000000007;
const long long MOD2 = 998244353;
#define eps 1e-9
#define PI acos(-1)
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

template <class T>
istream &operator>>(istream &s, vector<T> &v)
{
    for (auto &e : v)
        s >> e;
    return s;
}
template <class T>
ostream &operator<<(ostream &s, const vector<T> &v)
{
    for (auto &e : v)
        s << e << ' ';
    return s;
}
template <class T, class U>
ostream &operator<<(ostream &s, const pair<T, U> &p)
{
    s << p.first << ' ' << p.second;
    return s;
}

/*=========================================mint=======================================*/
const int mintMod = MOD2; // ここを変更
class mint
{
    long long x;

public:
    mint(long long x = 0) : x((x % mintMod + mintMod) % mintMod) {}
    long long val() const { return x; }
    mint operator-() const
    {
        return mint(-x);
    }
    mint &operator+=(const mint &a)
    {
        if ((x += a.x) >= mintMod)
            x -= mintMod;
        return *this;
    }
    mint &operator-=(const mint &a)
    {
        if ((x += mintMod - a.x) >= mintMod)
            x -= mintMod;
        return *this;
    }
    mint &operator*=(const mint &a)
    {
        (x *= a.x) %= mintMod;
        return *this;
    }
    mint operator+(const mint &a) const
    {
        mint res(*this);
        return res += a;
    }
    mint operator-(const mint &a) const
    {
        mint res(*this);
        return res -= a;
    }
    mint operator*(const mint &a) const
    {
        mint res(*this);
        return res *= a;
    }
    mint pow(ll t) const
    {
        if (!t)
            return 1;
        mint a = pow(t >> 1);
        a *= a;
        if (t & 1)
            a *= *this;
        return a;
    }
    // for prime mod
    mint inv() const
    {
        return pow(mintMod - 2);
    }
    mint &operator/=(const mint &a)
    {
        return (*this) *= a.inv();
    }
    mint operator/(const mint &a) const
    {
        mint res(*this);
        return res /= a;
    }

    friend ostream &operator<<(ostream &os, const mint &m)
    {
        os << m.x;
        return os;
    }
    friend istream &operator>>(istream &is, mint &m)
    {
        long long v;
        is >> v;
        m = mint(v);
        return is;
    }
};
/*========================================================================================*/

// ========= デバッグ用テンプレート =========
// 前方宣言
template <typename T>
void print_val(const T &x);
template <typename T, typename U>
void print_val(const pair<T, U> &p);

// コンテナかどうかの判定（stringはコンテナから除外してそのまま出力する）
template <typename T, typename = void>
struct is_container : false_type
{
};

template <typename T>
struct is_container<T, void_t<decltype(declval<T>().begin()), decltype(declval<T>().end())>>
    : conditional_t<is_same_v<T, string>, false_type, true_type>
{
};

template <typename T>
constexpr bool is_container_v = is_container<T>::value;

// mintなどの .val() を持つかどうかの判定
template <typename T, typename = void>
struct has_val : false_type
{
};

template <typename T>
struct has_val<T, void_t<decltype(declval<T>().val())>> : true_type
{
};

template <typename T>
constexpr bool has_val_v = has_val<T>::value;

// pair型の出力
template <typename T, typename U>
void print_val(const pair<T, U> &p)
{
    cout << "(";
    print_val(p.first);
    cout << ", ";
    print_val(p.second);
    cout << ")";
}

// 一般の型の出力（単一の値 or コンテナ or 生配列 or mint）
template <typename T>
void print_val(const T &x)
{
    if constexpr (std::is_array_v<T>)
    {
        // 生配列の場合 (int ans[100] や int ans[100][100] など)
        using value_type = std::remove_extent_t<T>;
        bool is_2d = std::is_array_v<value_type> || is_container_v<value_type>;

        for (size_t i = 0; i < std::extent_v<T>; ++i)
        {
            if (i != 0)
            {
                if (is_2d)
                    cout << "\n"; // 二次元配列なら改行で区切る
                else
                    cout << " "; // 一次元配列ならスペースで区切る
            }
            print_val(x[i]);
        }
    }
    else if constexpr (is_container_v<T>)
    {
        // コンテナの場合（vector, set, mapなど）
        using value_type = std::remove_reference_t<decltype(*x.begin())>;
        bool is_2d = std::is_array_v<value_type> || is_container_v<value_type>;

        for (auto it = x.begin(); it != x.end(); ++it)
        {
            if (it != x.begin())
            {
                if (is_2d)
                    cout << "\n"; // 二次元配列なら改行で区切る
                else
                    cout << " "; // 一次元配列ならスペースで区切る
            }
            print_val(*it);
        }
    }
    else if constexpr (has_val_v<T>)
    {
        // mintなどの .val() を持つ型の場合
        cout << x.val();
    }
    else
    {
        // スカラー値やstringなどの場合
        cout << x;
    }
}

// マクロ定義
// -DLOCALでコンパイル
#ifdef LOCAL
// ローカル環境でのみ出力
#define debug(x)                    \
    do                              \
    {                               \
        cout << "[" << #x << "]\n"; \
        print_val(x);               \
        cout << "\n";               \
    } while (0)
#else
// 提出時は無効化（出力時間によるTLEを防ぐため）
#define debug(x) \
    do           \
    {            \
    } while (0)
#endif
const string ans_partition = "====================================================================================";
// ==========================================

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
using vmint = vc<mint>;
using vvmint = vv<mint>;
using vvvmint = vv<vmint>;

#define pb push_back
#define eb emplace_back

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

// 切り上げ
long long ceil(long long N, long long d)
{
    if (d < 0)
        N *= -1, d *= -1;

    if (N > 0)
        return (N - 1) / d + 1;
    else
        return N / d;
}

int solve_onlyS(vl &onlyS, int s1, int s1_sz, vvl &g)
{
    if (onlyS[s1] != -1)
        return onlyS[s1];

    int res;
    rep(i, s1_sz)
    {
        if (s1 & (1 << i))
        {
            res = solve_onlyS(onlyS, 1 << i, s1_sz, g) & solve_onlyS(onlyS, s1 ^ (1 << i), s1_sz, g);
            break;
        }
    }
    return onlyS[s1] = res;
}

int solve_max_sz(vl &max_sz,int s2,int s2_sz,int s1_sz,vvb &c){
    if(max_sz[s2]!=-1)return max_sz[s2];
    int res = 0;
    rep(i,s2_sz){
        if(s2&(1<<i)){
            chmax(res,solve_max_sz(max_sz,s2^(1<<i),s2_sz,s1_sz,c));
        }
    }
    bl ok = true;
    rep(i,s2_sz){
        rep(j,s2_sz){
            if((s2&(1<<i))&&(s2&(1<<j))&&c[s1_sz+i][s1_sz+j]){
                ok = false;
                i = s2_sz;
                j = s2_sz;
                break;
            }
        }
    }
    if(ok){
        res = max<int>(res,__builtin_popcount(s2));
    }
    return max_sz[s2] = res; 
}

bool solve()
{
    int n, m;
    cin >> n >> m;
    if (n == 0 && m == 0)
    {
        /*終了条件*/
        return false;
    }
    vvl a(n, vl(m));
    rep(i, n)
    {
        rep(j, m)
        {
            cin >> a[i][j];
        }
    }
    vvl g(m);
    vvb c(m, vb(m));
    // グラフの構成
    rep(i, m)
    {
        rep(j, m)
        {
            if (i >= j)
                continue;
            vP tmp_ab(n);
            rep(k, n)
            {
                tmp_ab[k] = {a[k][i], a[k][j]};
            }
            sort(all(tmp_ab));
            int prev = -1;
            rep(k, n)
            {
                if (prev > tmp_ab[k].se)
                {
                    // 広義単調増加が相いれないペアが隣接
                    g[i].pb(j);
                    g[j].pb(i);
                    c[i][j] = 1;
                    c[j][i] = 1;
                    break;
                }
                prev = tmp_ab[k].se;
            }
        }
    }

    // 最大独立集合の要素数を求める
    // 半分全列挙

    int s1_sz = m / 2;
    int s2_sz = ceil(m, 2);
    // 前計算
    // ok[S_1] := S_1が独立集合であるか判定する
    // O(n^2 2^n)
    vb ok(1 << s1_sz, true);
    rep(i, 1 << s1_sz)
    {
        rep(u, s1_sz)
        {
            if (i & (1 << u))
            {
                for (auto v : g[u])
                {
                    if (i & (1LL << v))
                        ok[i] = false;
                }
            }
        }
    }

    // // onlyS[S_1] := S_1と隣接しないS_2の集合
    vl onlyS(1 << s1_sz, -1);
    {
        onlyS[0] = 0;
        rep(j, s2_sz)
        {
            onlyS[0] += (1 << j);
        }
    }
    rep(i, s1_sz)
    {
        onlyS[1 << i] = 0;
        rep(j, s2_sz)
        {
            onlyS[1 << i] += (1 << j) * (c[i][s1_sz + j] == 0);
        }
    }
    rep(i, 1 << s1_sz)
    {
        solve_onlyS(onlyS, i, s1_sz, g);
    }

    // // max_sz[S_2] := S_2の最大独立集合のサイズ
    vl max_sz(1<<s2_sz,-1);
    rep(i,1<<s2_sz){
        solve_max_sz(max_sz,i,s2_sz,s1_sz,c);
    }

    int ans = 0;
    rep(i,1<<s1_sz){
        if(!ok[i])continue;
        chmax(ans,__builtin_popcount(i)+max_sz[onlyS[i]]);
    }
    cout << ans << endl;
    return true;
}

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    while (solve())
        ;
    return 0;
}