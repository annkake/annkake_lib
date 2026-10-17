// 参照URL: https://github.com/annkake/annkake_lib
#include <bits/stdc++.h>
using namespace std;

#define int long long

/*=========================================mint=======================================*/
const int mintMod = 998244353; // ここを変更
class mint
{
    int x;

public:
    mint(int x = 0) : x((x % mintMod + mintMod) % mintMod) {}
    int val() const { return x; } 
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
    mint pow(int t) const
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
        int v;
        is >> v;
        m = mint(v);
        return is;
    }
};
/*========================================================================================*/

// BEGIN GENERATED COMMON
// 参照URL: https://github.com/annkake/annkake_lib
// 共通定義の原本: templates/common.hpp.in。変更後は tools/sync_templates.py を実行する。
const int64_t MOD1 = 1000000007;
const int64_t MOD2 = 998244353;
#define eps 1e-9
#define PI acos(-1)
#define all(v) (v).begin(), (v).end()
#define rall(v) (v).rbegin(), (v).rend()
#define fi first
#define se second
#ifdef LOCAL
#define endl "\n"
// main 内で sync_with_stdio(false) の後に生成する。
// 通常終了時、両ストリームを元の cerr へ順番にまとめて表示する。
class LocalOutput {
    ostringstream out, err;
    streambuf *old_out, *old_err;
public:
    LocalOutput()
        : old_out(cout.rdbuf(out.rdbuf())), old_err(cerr.rdbuf(err.rdbuf())) {}
    ~LocalOutput() {
        cout.flush();
        cerr.flush();
        cout.rdbuf(old_out);
        cerr.rdbuf(old_err);
        const auto errors = err.str(), answers = out.str();
        cerr << "[cerr]\n" << errors;
        if (!errors.empty() && errors.back() != '\n') cerr << '\n';
        cerr << "[cout]\n" << answers;
        if (!answers.empty() && answers.back() != '\n') cerr << '\n';
        cerr.flush();
    }
    LocalOutput(const LocalOutput&) = delete;
    LocalOutput& operator=(const LocalOutput&) = delete;
};
#endif


template <typename T>
using vc = vector<T>; // prioriy_queueに必要なのでここにこれ書いてます
template <typename T>
using vv = vc<vc<T>>;

// 異なる整数型はテンプレートの int にそろえて比較する。
template<class A, class B>
requires (is_integral_v<A> && is_integral_v<B> && !is_same_v<A, B>)
int max(A a, B b) {
    return std::max<int>(static_cast<int>(a), static_cast<int>(b));
}
template<class A, class B>
requires (is_integral_v<A> && is_integral_v<B> && !is_same_v<A, B>)
int min(A a, B b) {
    return std::min<int>(static_cast<int>(a), static_cast<int>(b));
}
template<class T, class U>
requires (is_integral_v<T> && is_integral_v<U>)
bool chmax(T& x, U y) {
    int a = static_cast<int>(x), b = static_cast<int>(y);
    if (a >= b) return false;
    x = static_cast<T>(b);
    return true;
}
template<class T, class U>
requires (is_integral_v<T> && is_integral_v<U>)
bool chmin(T& x, U y) {
    int a = static_cast<int>(x), b = static_cast<int>(y);
    if (a <= b) return false;
    x = static_cast<T>(b);
    return true;
}
// 浮動小数点など、同じ非整数型での既存の使い方も維持する。
template<class T> requires (!is_integral_v<T>)
bool chmax(T& x, const T& y) { if (x < y) { x = y; return true; } return false; }
template<class T> requires (!is_integral_v<T>)
bool chmin(T& x, const T& y) { if (y < x) { x = y; return true; } return false; }

constexpr int INF = sizeof(int) >= 8 ? static_cast<int>(2'000'000'000'000'000'000LL) : (1 << 30);
constexpr int64_t INF64 = (1LL << 60);
using ull = unsigned long long;

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

template<class T> concept HasFields = requires(const T& x) { x.debug_fields(); };
template<class T> concept Range = requires(const T& x) { begin(x); end(x); };
template<class T> concept Streamable = requires(ostream& os,const T& x) { os << x; };
template<class T> struct IsPair : false_type {};
template<class A,class B> struct IsPair<pair<A,B>> : true_type {};
template<class T> struct IsOptional : false_type {};
template<class T> struct IsOptional<optional<T>> : true_type {};
template<class T> struct IsVariant : false_type {};
template<class... Ts> struct IsVariant<variant<Ts...>> : true_type {};
template<class T> struct IsTuple : false_type {};
template<class... Ts> struct IsTuple<tuple<Ts...>> : true_type {};
template<class T, size_t N> struct IsTuple<array<T,N>> : true_type {};

template<class T> void debug_print_value(ostream& os,const T& v);
template<class Tuple,size_t... I> void debug_print_tuple(ostream& os,const Tuple& t,index_sequence<I...>){
    os << '(';
    ((os << (I==0 ? "" : ", "), debug_print_value(os,get<I>(t))), ...);
    os << ')';
}
template<class T> void debug_print_value(ostream& os,const T& v){
    using V = decay_t<T>;
    if constexpr (requires { v.val(); }) {
        os << v.val();
    } else if constexpr (is_convertible_v<const T&, string_view>) {
        os << string_view(v);
    } else if constexpr (HasFields<T>) {
        debug_print_value(os,v.debug_fields());
    } else if constexpr (IsPair<V>::value) {
        os << '('; debug_print_value(os,v.first); os << ", "; debug_print_value(os,v.second); os << ')';
    } else if constexpr (IsOptional<V>::value) {
        if(v) { os << "some("; debug_print_value(os,*v); os << ')'; } else os << "nullopt";
    } else if constexpr (IsVariant<V>::value) {
        os << "variant#" << v.index() << '(';
        visit([&](const auto& a){ debug_print_value(os,a); },v); os << ')';
    } else if constexpr (IsTuple<V>::value) {
        debug_print_tuple(os,v,make_index_sequence<tuple_size_v<V>>{});
    } else if constexpr (Range<T> && !is_same_v<V,string> && !is_same_v<V,string_view>) {
        os << '{'; bool first=true;
        for(const auto& x:v){if(!first)os << ", "; first=false; debug_print_value(os,x);} os << '}';
    } else if constexpr (Streamable<T>) {
        os << v;
    } else {
        os << "<表示不可: debug_fields() または operator<< が必要>";
    }
}

template<class T> void print_debug(const T& x,ostream& os=cerr){debug_print_value(os,x); os << '\n';}
#ifdef LOCAL
#define debug(expr) do { cerr << "[DEBUG] " << #expr << " = "; print_debug((expr)); } while (0)
#define AHC_DEBUG(enabled, expr) do { if (enabled) { debug(expr); } } while (0)
#else
#define debug(expr) do {} while (0)
#define AHC_DEBUG(enabled, expr) do {} while (0)
#endif


// 境界は一度だけ評価。逆順は比較後に減算し、最小値でのアンダーフローを防ぐ。
#define rep(i, n) for (int i = 0, i##_end = (n); i < i##_end; ++i)
#define rrep(i, n) for (int i = 0, i##_end = (n); i < i##_end && (++i, true); )
#define drep(i, n) for (int i = max<int>(0, (n)); i > 0 && (--i, true); )
#define nfor(i, s, n) for (int i = (s), i##_end = (n); i < i##_end; ++i)
#define dfor(i, s, n) for (int i = (s), i##_end = (n); i > i##_end && (--i, true); )
// 64bit対応（__builtin_popcount は32bitで切り捨てるため popcountll を使用）。
#define popcount(x) __builtin_popcountll(static_cast<unsigned long long>(x))
#define fix(n) (cout << fixed << setprecision(n))

namespace template_detail {
inline int divide(int a, int b, bool upward) {
    if (b == 0) throw domain_error("division by zero");
    if (a == numeric_limits<int>::min() && b == -1)
        throw overflow_error("integer division overflow");
    int q = a / b, r = a % b;
    if (r != 0) {
        if (upward) q += ((r > 0) == (b > 0));
        else q -= ((r > 0) != (b > 0));
    }
    return q;
}
}
#define floor_div(a, b) template_detail::divide((a), (b), false)
#define ceil_div(a, b) template_detail::divide((a), (b), true)
// max/min は top() が返す値を表す。
template<class T> using pq_max = priority_queue<T, vector<T>>;
template<class T> using pq_min = priority_queue<T, vector<T>, greater<T>>;

using ld = long double;
using bl = bool;
// ICPC用の自前mintは共通定義の前で定義する。

using vl = vc<int>;
using vvl = vv<int>;
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
using P = pair<int, int>;
using vP = vc<P>;
using vvP = vc<vP>;
using vvvP = vv<vP>;
using vmint = vc<mint>;
using vvmint = vv<mint>;
using vvvmint = vv<vmint>;

#define pb push_back
#define eb emplace_back

#define YES (cout << "Yes\n")
#define NO (cout << "No\n")
#define YN                     \
    {                          \
        cout << "Yes\n"; \
    }                          \
    else                       \
    {                          \
        cout << "No\n";  \
    } // if(a==b)YN;
#define dame (cout << "-1\n")

constexpr string_view DIR = "DRUL";
constexpr array<int, 4> di{1, 0, -1, 0}, dj{0, 1, 0, -1};

inline bool out_grid(int i, int j, int h, int w) {
    return i < 0 || i >= h || j < 0 || j >= w;
}
// END GENERATED COMMON

bool solve()
{
    /*終了条件*/
    return false;

    return true;
}

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
#ifdef LOCAL
    LocalOutput local_output;
#endif
    while (solve());
    return 0;
}

// BEGIN GENERATED SNIPPET INDEX
// C++スニペット prefix対応表（原本: snippets/cpp.code-snippets）
// 更新: python3 tools/sync_snippets.py
// 関数・クラス定義はsolve/mainの外へ展開する。依存prefixは先に各1回だけ挿入。
// ライブラリ用はincludeなし。テンプレートの<bits/stdc++.h>を前提とする。
// 通常は一覧の先頭prefixを使う。括弧内は旧prefixの別名。
// atcoder (ma, m)                : AtCoder用テンプレート（ACL使用）
// icpc (ICPC)                    : ICPC用テンプレート（ACL不要）
// ahc (mahc)                     : AHC用の単一ファイルテンプレート
// ahc_official                   : 公式スコア接続の単一ファイル例（case Bの入出力は問題ごとに変更）
// bit                            : 0始まりのFenwick木による加算と区間和。
// lazy_segtree                   : 遅延評価セグメント木と加算/代入×和/最小値の4構成。
// segtree (seg)                  : 一点更新・区間最小値の反復型セグメント木。
// digit_dp (digitDP)             : dp[桁数][小さいことが確定したか] の構成を保つ桁DPテンプレート。
// lis                            : 最長増加部分列のインデックスを復元する（狭義・広義を選択可能）。
// bfs                            : 無重みグラフの単一始点最短距離。
// dfs                            : 明示的なスタックによる深さ優先探索。
// dijkstra                       : 非負重みグラフのDijkstra法。
// doubling                       : 各頂点からの一意な遷移を二分持ち上げで繰り返す。
// dsu                            : 経路圧縮とサイズによる併合を使う素集合データ構造。
// euler_tour                     : 木のDFS訪問順・オイラー巡回順と部分木の半開区間。
// floyd (warshall)               : Floyd法による全点対最短距離。
// lca (LCA)                      : 二分持ち上げによる木の最近共通祖先。
// scc                            : 反復DFSによる強連結成分分解と縮約DAG。
// comb (comv)                    : 階乗テーブルによる二項係数の剰余計算。 / 先に: mod_pow
// div_ceil (ceil)                : 符号付き整数除算の切り上げ。
// div_floor (floor)              : 符号付き整数除算の切り下げ。
// divisors                       : 正の約数の昇順列挙。
// ext_gcd (extGCD)               : 拡張Euclid法によるBezout係数。
// factor (PFD)                   : 試し割りによる素因数分解。
// gcd (GCD)                      : 非負の最大公約数。
// gf2                            : GF(2)上の連立一次方程式と行列ランク。
// mat_pow (matrixpow)            : 正方行列の剰余べき乗。
// mod_inv                        : 拡張Euclid法による剰余逆元。
// mod_pow (modpow)               : 二分累乗による剰余べき乗。
// modint                         : コンパイル時に法を指定する剰余整数型。 / 先に: mod_inv, mod_pow
// prime_sieve (isprime)          : エラトステネスの篩による素数判定表。
// sieve                          : 最小素因数表による繰り返し素因数分解。
// calc_next                      : 各位置以降で小文字英字が最初に出現する位置を前計算する。
// rolling_hash (Hash)            : 2つの法を使う文字列のローリングハッシュ。
// trie (Trie)                    : 小文字英字のTrieと共有接頭辞長の検索。
// compress (Compress)            : 任意の比較可能な値の座標圧縮。
// rle (runlength)                : 文字列のランレングス符号化と復元。
// ahc_official_bridge            : 公式スコアと最良解の接続（AHCテンプレート共通部が必要）
// END GENERATED SNIPPET INDEX
