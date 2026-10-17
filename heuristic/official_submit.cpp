// BEGIN FILE examples/official_score/main_case_b.cpp
// 参照URL: https://github.com/annkake/annkake_lib
// BEGIN FILE examples/official_score/case_b_adapter.hpp
// 参照URL: https://github.com/annkake/annkake_lib
// BEGIN FILE examples/official_score/translated_case_b.hpp
// 参照URL: https://github.com/annkake/annkake_lib
// User-supplied translation of official scoring tools (case B).
// Source supplied by owner: https://atcoder.jp/contests/ahc071/tasks/ahc071_a
// Case B matches local AHC070 Rust tools on 30 valid/invalid test cases.
// The supplied AHC071 URL has a different input format; upstream license remains unverified.
// This file is NOT covered by the repository's CC0 dedication.
// Scoring logic is preserved; non-template header functions are marked inline.
#include <bits/stdc++.h>
namespace translated_b {
using namespace std;




using i64 = int64_t;
using u64 = uint64_t;
using usize = size_t;

template <class T>
bool setmin(T& a, const T& b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

template <class T>
bool setmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

struct Input {
    usize N, M;
    vector<pair<usize, usize>> targets;
};

inline ostream& operator<<(ostream& os, const Input& input) {
    os << input.N << ' ' << input.M << '\n';
    for (auto [x, y] : input.targets) {
        os << x << ' ' << y << '\n';
    }
    return os;
}

inline Input parse_input(const string& s) {
    istringstream is(s);
    Input input;

    if (!(is >> input.N >> input.M)) {
        throw runtime_error("Invalid input");
    }

    input.targets.resize(input.N * input.N);
    for (auto& [x, y] : input.targets) {
        if (!(is >> x >> y)) {
            throw runtime_error("Invalid input");
        }
    }

    return input;
}

template <class T>
variant<T, string> read(
    optional<string> token,
    T lo,
    T hi,
    bool inclusive_hi = false
) {
    if (!token) {
        return string("Unexpected EOF");
    }

    T value{};
    istringstream is(*token);
    is >> noskipws >> value;

    if (!is || is.peek() != char_traits<char>::eof()) {
        return string("Parse error: ") + *token;
    }

    if (value < lo || (inclusive_hi ? value > hi : value >= hi)) {
        return string("Out of range: ") + to_string(value);
    }

    return value;
}

struct Output {
    vector<pair<int, int>> m;
    vector<usize> actions;
};

using OutputResult = variant<Output, string>;

inline OutputResult parse_output(const Input& input, const string& s) {
    istringstream is(s);

    auto next = [&]() -> optional<string> {
        string token;
        if (is >> token) return token;
        return nullopt;
    };

    Output out;

    for (int i = 0; i < 3; ++i) {
        auto x = read<int>(next(), 0, static_cast<int>(input.N));
        if (holds_alternative<string>(x)) {
            return get<string>(x);
        }

        auto y = read<int>(next(), 0, static_cast<int>(input.N));
        if (holds_alternative<string>(y)) {
            return get<string>(y);
        }

        out.m.emplace_back(get<int>(x), get<int>(y));
    }

    for (usize i = 0; i < input.N * input.N; ++i) {
        auto action = read<usize>(next(), 0, input.M);
        if (holds_alternative<string>(action)) {
            return get<string>(action);
        }
        out.actions.push_back(get<usize>(action));
    }

    if (auto extra = next()) {
        return string("Unexpected token: ") + *extra;
    }

    return out;
}

struct Sim {
    pair<usize, usize> prev_pos{0, 0};
    pair<usize, usize> cur_pos{0, 0};
    pair<usize, usize> cur_target{0, 0};

    vector<vector<optional<usize>>> visited_at;
    usize unique_count = 0;
    vector<i64> stepwise_costs;

    explicit Sim(const Input& input)
        : visited_at(
            input.N,
            vector<optional<usize>>(input.N, nullopt)
        ) {}

    i64 total_cost(usize t) const {
        if (t > stepwise_costs.size()) {
            throw out_of_range("Invalid turn");
        }

        return accumulate(
            stepwise_costs.begin(),
            stepwise_costs.begin() + t,
            i64(0)
        );
    }
};

using ScoreDetails = tuple<i64, i64, string, Sim>;

inline ScoreDetails compute_score_details(
    const Input& input,
    const Output& out,
    usize t
) {
    usize T = input.N * input.N;

    if (out.actions.size() != T) {
        return {
            0,
            0,
            "Invalid action length: " + to_string(out.actions.size()) +
                " (expected " + to_string(T) + ")",
            Sim(input)
        };
    }

    vector<pair<usize, usize>> black_cells;
    Sim sim(input);
    optional<Sim> sim_at_t;

    if (t == 0) {
        sim_at_t = sim;
    }

    for (usize step = 0; step < T; ++step) {
        usize action = out.actions[step];

        if (action >= out.m.size()) {
            return {
                0,
                0,
                "Invalid action index " + to_string(action) +
                    " at turn " + to_string(step),
                Sim(input)
            };
        }

        auto [dx, dy] = out.m[action];
        auto [x, y] = sim.cur_pos;

        auto mod = [](i64 a, i64 n) -> usize {
            return static_cast<usize>((a % n + n) % n);
        };

        usize x1 = mod(static_cast<i64>(x) + dx, input.N);
        usize y1 = mod(static_cast<i64>(y) + dy, input.N);

        if (!sim.visited_at[x1][y1]) {
            sim.visited_at[x1][y1] = step;
            black_cells.emplace_back(x1, y1);
            ++sim.unique_count;
        }

        sim.prev_pos = sim.cur_pos;
        sim.cur_pos = {x1, y1};

        auto [a, b] = input.targets.at(step);

        usize min_dist = numeric_limits<usize>::max();

        for (auto [xx, yy] : black_cells) {
            usize dist =
                (xx > a ? xx - a : a - xx) +
                (yy > b ? yy - b : b - yy);

            min_dist = min(min_dist, dist);
        }

        i64 step_cost = static_cast<i64>(
            static_cast<double>(min_dist) *
            sqrt(static_cast<double>(step + 1))
        );

        sim.stepwise_costs.push_back(step_cost);

        if (step + 1 == t) {
            sim_at_t = sim;
            sim_at_t->cur_target = input.targets[step];
        }
    }

    i64 cost = sim.total_cost(T);

    i64 score = static_cast<i64>(
        round(
            1e6 * pow(static_cast<double>(input.N), 3.0) /
            (1.0 + static_cast<double>(cost))
        )
    );

    if (!sim_at_t) {
        throw out_of_range("Invalid visualization turn");
    }

    return {cost, score, "", move(*sim_at_t)};
}

inline pair<i64, string> compute_score(
    const Input& input,
    const Output& out
) {
    auto [cost, score, err, sim] =
        compute_score_details(input, out, input.N * input.N);

    if (!err.empty()) {
        score = 0;
    }

    return {score, err};
}


} // namespace translated_b
// END FILE examples/official_score/translated_case_b.hpp
// BEGIN FILE heuristic/official_bridge.hpp
// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file official_bridge.hpp
 * @brief 公式評価の合法性とスコアを共通の最良解管理へ接続する。
 * @details heuristic/core.hppのGoalとBestOutputを利用する。評価は完成したOutputに対して呼ぶ。
 * @note 提供されたahc_patch_v4の接続部を統合。ライセンス条件はprovenance記録を参照。
 * @par 使用例（AHC共通部と一緒に使用）
 * @code{.cpp}
 * struct Output { int value; };
 * ahc::OfficialRecorder<Output, int> recorder(ahc::Goal::Maximize,
 *     [](const Output& out) {
 *         if (out.value < 0) return ahc::ScoreResult<int>::invalid("negative value");
 *         return ahc::ScoreResult<int>::valid(out.value); // 0点でも合法。
 *     });
 * recorder.consider(Output{0});
 * recorder.consider(Output{10});
 * int best = recorder.best.output->value; // 10。
 * @endcode
 */
// BEGIN FILE heuristic/core.hpp
// 参照URL: https://github.com/annkake/annkake_lib
// AHC 共通ライブラリ v3（C++20）
// 問題ごとの Input / State / Action / Output はこのファイルに書かない。
#if __has_include(<atcoder/all>)
#include <atcoder/all>
#endif

using namespace std;
#if __has_include(<atcoder/all>)
using namespace atcoder;
#endif

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
#if __has_include(<atcoder/all>)
using mint = atcoder::modint998244353;
#endif

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
#if __has_include(<atcoder/all>)
using vmint = vc<mint>;
using vvmint = vv<mint>;
using vvvmint = vv<vmint>;
#endif

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

namespace ahc {

struct Timer {
    using Clock=chrono::steady_clock;
    Clock::time_point start=Clock::now();
    double elapsed() const {return chrono::duration<double>(Clock::now()-start).count();}
    void reset(){start=Clock::now();}
};
struct RNG {
    uint32_t x=123456789,y=362436069,z=521288629,w=88675123;
    uint32_t next_u32(){uint32_t t=x^(x<<11);x=y;y=z;z=w;return w=(w^(w>>19))^(t^(t>>8));}
    uint64_t next_u64(){return (uint64_t(next_u32())<<32)|next_u32();}
    uint32_t uniform_u32(uint32_t b){if(!b)throw invalid_argument("乱数範囲が空");uint32_t threshold=uint32_t(-b)%b;for(;;){auto r=next_u32();if(r>=threshold)return r%b;}}
    uint64_t uniform_u64(uint64_t b){if(!b)throw invalid_argument("乱数範囲が空");uint64_t threshold=uint64_t(-b)%b;for(;;){auto r=next_u64();if(r>=threshold)return r%b;}}
    int uniform_int(int lo,int hi){if(lo>=hi)throw invalid_argument("乱数の上下限が不正");return int(int64_t(lo)+uniform_u64(uint64_t(int64_t(hi)-lo)));}
    double uniform01(){return next_u32()*(1.0/4294967296.0);}
    double uniform01_open(){return (next_u32()+0.5)*(1.0/4294967296.0);}
};

enum class Goal { Maximize, Minimize };
template<class T> bool better(T a,T b,Goal g){return g==Goal::Maximize?a>b:a<b;}
inline double rate(uint64_t a,uint64_t b){return b?100.0*double(a)/double(b):0.;}

// 公式スコアの良い合法な完成解のみ保存する。観測対象の判定は呼び出し側で行う。
template<class Output,class Score=int> struct BestOutput {
    optional<Output> output;
    optional<Score> score;
    uint64_t updates=0;
    template<class Make> bool consider(Score candidate,Goal goal,Make&& make){
        if(score && !better(candidate,*score,goal))return false;
        Output next=make(); // 成功するまで既存の最良解を壊さない
        output=move(next);score=candidate;++updates;return true;
    }
};

// ---------- apply/undo 整合性チェック ----------
// 有効時は「適用 -> 復元」を試し、全可変情報のスナップショット一致を要求。
// assert に加え例外を投げるため、-DNDEBUG でも異常を検出する。
template<class State> concept HasSnapshot = requires(const State& s) {
    s.debug_snapshot();
    { s.debug_snapshot()==s.debug_snapshot() } -> convertible_to<bool>;
};
template<class State,class Action>
void check_apply_undo(State& state,const Action& action,bool enabled,const char* place){
    if(!enabled)return;
    if constexpr(HasSnapshot<State>){
        const auto before=state.debug_snapshot();
        const auto backup=state.apply(action);
        state.undo(action,backup);
        const bool ok=(state.debug_snapshot()==before);
        assert(ok && "apply/undo により盤面が破損しました");
        if(!ok)throw logic_error(string("apply/undo 不一致: ")+place);
    }else{
        throw logic_error("復元チェックには State::debug_snapshot() const が必要");
    }
}

// 実際の undo 直後に、操作前の盤面へ戻ったか検査するガード。
// チェック有効時だけスナップショットを保持する。
template<class State,bool Has=HasSnapshot<State>> struct RestoreGuard {
    RestoreGuard(const State&,bool enabled){
        if(enabled)throw logic_error("debug_snapshot() が必要です");
    }
    void verify(const State&,const char*)const{}
};
template<class State> struct RestoreGuard<State,true> {
    using Snapshot=decltype(declval<const State&>().debug_snapshot());
    optional<Snapshot> before;
    RestoreGuard(const State& state,bool enabled){
        if(enabled)before.emplace(state.debug_snapshot());
    }
    void verify(const State& state,const char* place)const{
        if(!before)return;
        bool ok=(state.debug_snapshot()==*before);
        assert(ok && "undo後にStateが元へ戻っていません");
        if(!ok)throw logic_error(string("undo復元失敗: ")+place);
    }
};

struct WeightedChoice {
    vector<uint64_t> prefix;
    uint64_t sum=0;
    explicit WeightedChoice(const vector<int>& ws){
        if(ws.empty())throw invalid_argument("近傍の重みが空");
        for(int w:ws){if(w<0 || sum>UINT64_MAX-uint64_t(w))throw invalid_argument("近傍重みが不正");sum+=uint64_t(w);prefix.push_back(sum);}
        if(sum==0)throw invalid_argument("正の近傍重みが必要");
    }
    int pick(RNG& rng) const {return int(upper_bound(all(prefix),rng.uniform_u64(sum))-prefix.begin());}
};

// ---------- 焼きなまし ----------
struct SAConfig {
    double deadline_sec=1.9; // Timer生成時からの絶対終了時刻
    double start_temp=100, end_temp=1;
    bool exponential=false;
    Goal goal=Goal::Maximize;
    vector<int> neighborhood_weights{1};
    int check_interval=128;
    int time_bins=10;
    bool log_stats=true;
    bool check_undo=false;
};
struct SACounts {
    uint64_t tried=0,valid=0,accepted=0;
    vector<uint64_t> nt,nv,na;
    SACounts()=default;
    explicit SACounts(size_t n):nt(n),nv(n),na(n){}
    void record(int k,bool exists,bool accept){
        ++tried;++nt[k];if(exists){++valid;++nv[k];}
        if(accept){++accepted;++na[k];}
    }
};
struct SAStats {
    SACounts total;
    vector<SACounts> phases;
    uint64_t best_heuristic_updates=0;
    double elapsed_sec=0;
};
template<class State> struct SAResult {State best_state; SAStats stats;};
inline void print_sa_stats(const SAStats& s,ostream& os=cerr){
    auto f=os.flags();auto p=os.precision();os<<fixed<<setprecision(2);
    os<<"[SA] 回数="<<s.total.tried<<" 有効="<<s.total.valid<<" 採用="<<s.total.accepted
      <<" 採用率="<<rate(s.total.accepted,s.total.valid)<<"% 探索最良更新="<<s.best_heuristic_updates
      <<" 時間="<<s.elapsed_sec<<"s\n";
    for(size_t k=0;k<s.total.nt.size();++k)
        os<<"[SA] 近傍"<<k<<" 試行="<<s.total.nt[k]<<" 有効="<<s.total.nv[k]
          <<" 採用="<<s.total.na[k]<<" 採用率="<<rate(s.total.na[k],s.total.nv[k])<<"%\n";
    for(size_t b=0;b<s.phases.size();++b){
        auto& v=s.phases[b];
        os<<"[SA] 進捗"<<100*b/s.phases.size()<<"-"<<100*(b+1)/s.phases.size()<<"%"
          <<" 試行="<<v.tried<<" 採用率="<<rate(v.accepted,v.valid)<<"%";
        for(size_t k=0;k<v.nt.size();++k)
            os<<" 近傍"<<k<<"="<<rate(v.na[k],v.nv[k])<<"%("<<v.na[k]<<"/"<<v.nv[k]<<")";
        os<<'\n';
    }
    os.flags(f);os.precision(p);
}
inline double sa_temperature(const SAConfig& c,double progress){
    progress=clamp(progress,0.,1.);
    return c.exponential?c.start_temp*pow(c.end_temp/c.start_temp,progress)
                        :c.start_temp+(c.end_temp-c.start_temp)*progress;
}
inline bool sa_accept(double delta,double temperature,Goal goal,RNG& rng){
    const double gain=(goal==Goal::Maximize?delta:-delta);
    return gain>=0 || (temperature>0 && gain>=temperature*log(rng.uniform01_open()));
}
struct NoObserver {template<class State>void operator()(const State&) const{}};

// propose(state, 近傍番号, rng) -> optional<Move>
// observer(state) は棄却候補にも呼び、公式スコアの最良Outputを保持できる。
template<class State,class Propose,class Observer=NoObserver>
SAResult<State> simulated_annealing(State initial,const SAConfig& cfg,const Timer& timer,
                                     RNG& rng,Propose&& propose,Observer&& observer={}){
    if(cfg.check_interval<=0 || cfg.time_bins<=0 || !isfinite(cfg.start_temp) ||
       !isfinite(cfg.end_temp) || cfg.start_temp<0 || cfg.end_temp<0 ||
       (cfg.exponential && (cfg.start_temp<=0 || cfg.end_temp<=0)))
        throw invalid_argument("SA設定が不正");
    WeightedChoice choice(cfg.neighborhood_weights);
    const double start=timer.elapsed(), span=cfg.deadline_sec-start;
    State state=move(initial);
    SAResult<State> result{state,{}};
    result.stats.total=SACounts(cfg.neighborhood_weights.size());
    result.stats.phases.assign(cfg.time_bins,SACounts(cfg.neighborhood_weights.size()));
    observer(state);
    if(span<=0){if(cfg.log_stats)print_sa_stats(result.stats);return result;}
    double progress=0,temperature=cfg.start_temp;
    uint64_t steps=0;
    for(;;){
        if(steps%uint64_t(cfg.check_interval)==0){
            const double now=timer.elapsed();if(now>=cfg.deadline_sec)break;
            progress=clamp((now-start)/span,0.,1.);
            temperature=sa_temperature(cfg,progress);
        }
        const int k=choice.pick(rng);
        const int b=min(cfg.time_bins-1,int(progress*cfg.time_bins));
        ++steps;
        auto move_opt=propose(state,k,rng);
        if(!move_opt){result.stats.total.record(k,false,false);result.stats.phases[b].record(k,false,false);continue;}
        const auto& m=*move_opt;
        check_apply_undo(state,m,cfg.check_undo,"SA事前検査");
        const auto old=state.score;
        RestoreGuard<State> restore(state,cfg.check_undo);
        auto backup=state.apply(m);
        observer(state);
        const double delta=double((long double)state.score-(long double)old);
        bool accept=sa_accept(delta,temperature,cfg.goal,rng);
        if(accept){
            if(better(state.score,result.best_state.score,cfg.goal)){
                result.best_state=state;++result.stats.best_heuristic_updates;
            }
        }else{
            state.undo(m,backup);
            restore.verify(state,"SA棄却時");
        }
        result.stats.total.record(k,true,accept);
        result.stats.phases[b].record(k,true,accept);
    }
    result.stats.elapsed_sec=timer.elapsed()-start;
    if(cfg.log_stats)print_sa_stats(result.stats);
    return result;
}

// ---------- 木上の差分更新ビームサーチ ----------
struct BeamConfig {
    int max_turn=100;
    size_t width=100;
    Goal goal=Goal::Maximize;
    double deadline_sec=1.9;
    int check_interval=512;
    bool deduplicate=true;
    bool track_diversity=true;
    bool log_stats=true;
    bool check_undo=false;
};
struct BeamTurnStats {
    int turn=0;
    uint64_t frontier=0,transitions=0,selected=0;
    uint64_t unique_hashes=0,unique_parents=0;
    double step_sec=0,total_sec=0;
    double best_eval=0,mean_eval=0;
    bool committed=false; // 時間切れで途中まで列挙した層はfalse
};
struct BeamStats {
    vector<BeamTurnStats> turns;
    uint64_t total_transitions=0, pruned_nodes=0, peak_nodes=0;
    double elapsed_sec=0;
};
template<class Action,class Score> struct TreeBeamResult {
    vector<Action> actions;
    Score score{};
    bool completed=false;
    int reached_turn=0;
    BeamStats stats;
};
inline void print_beam_stats(const BeamStats& s,ostream& os=cerr){
    auto f=os.flags();auto p=os.precision();os<<fixed<<setprecision(3);
    for(auto& t:s.turns){
        os<<"[Beam] ターン="<<t.turn<<" 確定="<<(t.committed?"Yes":"No")<<" 展開元="<<t.frontier
          <<" 遷移="<<t.transitions<<" 採用="<<t.selected
          <<" 異なるhash="<<t.unique_hashes
          <<" 多様性="<<rate(t.unique_hashes,t.transitions)<<"%"
          <<" 親の種類="<<t.unique_parents
          <<" 最良評価="<<t.best_eval<<" 平均評価="<<t.mean_eval
          <<" ターン時間="<<t.step_sec<<"s 経過="<<t.total_sec<<"s\n";
    }
    os<<"[Beam] 合計遷移="<<s.total_transitions<<" 枝刈り="<<s.pruned_nodes
      <<" 最大ノード="<<s.peak_nodes<<" 探索時間="<<s.elapsed_sec<<"s\n";
    os.flags(f);os.precision(p);
}

// State に必要なもの：score, enumerate_actions(vector<Action>&),
// apply(action)->Undo, undo(action, Undo), hash()（多様性計測または重複除去時）。
// ノードは採用後に追加する。探索中の State は1個だけで、全盤面をコピーしない。
template<class Action,class State,class Observer=NoObserver>
auto tree_beam_search(State initial,const BeamConfig& cfg,const Timer& timer,
                      Observer&& observer={})
    -> TreeBeamResult<Action,decay_t<decltype(initial.score)>> {
    using Score=decay_t<decltype(initial.score)>;
    if(cfg.max_turn<0 || cfg.width==0 || cfg.check_interval<=0)
        throw invalid_argument("Beam設定が不正");
    if((cfg.deduplicate||cfg.track_diversity) &&
       !requires(const State& s){{s.hash()}->convertible_to<uint64_t>;})
        throw invalid_argument("重複除去/多様性計測にはState::hash()が必要");
    struct Node {
        int parent=-1,child=-1,prev=-1,next=-1,depth=0;
        Score score{};
        optional<Action> action;
    };
    struct Candidate {
        Score score;
        uint64_t hash;
        int parent;
        uint64_t serial;
        Action action;
    };
    State state=move(initial);
    TreeBeamResult<Action,Score> res;
    const double start=timer.elapsed();
    vector<Node> nodes(1);
    nodes[0].score=state.score;
    vector<int> free_nodes,frontier{0};
    uint64_t live_nodes=1,serial=0,check_counter=0;
    res.stats.peak_nodes=1;
    observer(state);

    auto add_node=[&](int p,const Candidate& c)->int{
        int id;
        if(free_nodes.empty()){
            if(nodes.size()>=size_t(INT_MAX))throw length_error("Beamノードが多すぎます");
            id=int(nodes.size());nodes.emplace_back();
        }else{id=free_nodes.back();free_nodes.pop_back();}
        nodes[id]=Node{};
        nodes[id].parent=p;
        nodes[id].depth=nodes[p].depth+1;
        nodes[id].score=c.score;
        nodes[id].action=c.action;
        nodes[id].next=nodes[p].child;
        if(nodes[id].next!=-1)nodes[nodes[id].next].prev=id;
        nodes[p].child=id;
        ++live_nodes;
        res.stats.peak_nodes=max(res.stats.peak_nodes,live_nodes);
        return id;
    };
    auto prune=[&](int id){
        while(id!=0 && nodes[id].child==-1){
            int p=nodes[id].parent,pr=nodes[id].prev,nx=nodes[id].next;
            if(pr>=0)nodes[pr].next=nx;else nodes[p].child=nx;
            if(nx>=0)nodes[nx].prev=pr;
            nodes[id]=Node{};free_nodes.push_back(id);
            --live_nodes;++res.stats.pruned_nodes;
            id=p;
        }
    };
    for(int turn=0;turn<cfg.max_turn;++turn){
        const double t0=timer.elapsed();
        if(t0>=cfg.deadline_sec)break;
        vector<Candidate> candidates;
        vector<Action> actions;
        bool timed_out=false;
        BeamTurnStats log;
        log.turn=turn;
        auto dfs=[&](auto&& self,int u)->void {
            if(timed_out)return;
            if(nodes[u].depth==turn){
                ++log.frontier;
                actions.clear();
                state.enumerate_actions(actions);
                for(const Action& a:actions){
                    // 候補も探索木移動も検査する。無効時は追加のapplyはしない。
                    check_apply_undo(state,a,cfg.check_undo,"Beam候補");
                    RestoreGuard<State> restore(state,cfg.check_undo);
                    const auto backup=state.apply(a);
                    observer(state);
                    uint64_t hash=0;
                    if(cfg.deduplicate||cfg.track_diversity){
                        if constexpr(requires(const State& s){{s.hash()}->convertible_to<uint64_t>;})
                            hash=uint64_t(state.hash());
                    }
                    const Score score=state.score;
                    state.undo(a,backup);
                    restore.verify(state,"Beam候補");
                    // score の不整合は提出時も検出（探索木の整合性に必須）。
                    if(state.score!=nodes[u].score)
                        throw logic_error("Beam候補のundoでscoreを復元できませんでした");
                    candidates.push_back({score,hash,u,serial++,a});
                    ++log.transitions;
                    if(++check_counter>=uint64_t(cfg.check_interval)){
                        check_counter=0;
                        if(timer.elapsed()>=cfg.deadline_sec){timed_out=true;return;}
                    }
                }
                return;
            }
            for(int v=nodes[u].child;v!=-1;v=nodes[v].next){
                const Action& action=*nodes[v].action;
                check_apply_undo(state,action,cfg.check_undo,"Beam探索木の辺");
                RestoreGuard<State> restore(state,cfg.check_undo);
                auto backup=state.apply(action);
                if(state.score!=nodes[v].score)
                    throw logic_error("Beam探索木のscoreが一致しません");
                self(self,v);
                state.undo(action,backup);
                restore.verify(state,"Beam探索木");
                if(state.score!=nodes[u].score)
                    throw logic_error("Beam探索木のundoでscoreを復元できませんでした");
                if(timed_out)return;
            }
        };
        dfs(dfs,0);
        // 計算時間不足で層が途中までの場合、その層は確定しない。
        if(timed_out||candidates.empty()){
            log.step_sec=timer.elapsed()-t0;
            log.total_sec=timer.elapsed()-start;
            res.stats.total_transitions+=log.transitions;
            res.stats.turns.push_back(log);
            break;
        }
        auto cmp=[&](const Candidate& a,const Candidate& b){
            if(a.score!=b.score)return better(a.score,b.score,cfg.goal);
            return a.serial<b.serial;
        };
        sort(all(candidates),cmp);
        unordered_set<uint64_t> distinct;
        if(cfg.track_diversity){
            distinct.reserve(candidates.size());
            for(const auto& c:candidates)distinct.insert(c.hash);
            log.unique_hashes=distinct.size();
        }
        unordered_set<uint64_t> seen;
        unordered_set<int> distinct_parents;
        vector<int> next;
        next.reserve(min(cfg.width,candidates.size()));
        for(const Candidate& c:candidates){
            if(cfg.deduplicate && !seen.insert(c.hash).second)continue;
            next.push_back(add_node(c.parent,c));
            distinct_parents.insert(c.parent);
            if(next.size()>=cfg.width)break;
        }
        if(next.empty())break;
        for(int u:frontier){if(nodes[u].child==-1)prune(u);}
        frontier.swap(next);
        ++res.reached_turn;
        log.selected=frontier.size();
        if(!frontier.empty()){
            auto best_score=nodes[frontier[0]].score;
            long double score_sum=0;
            for(int u:frontier){
                if(better(nodes[u].score,best_score,cfg.goal))best_score=nodes[u].score;
                score_sum+=(long double)nodes[u].score;
            }
            log.best_eval=double(best_score);
            log.mean_eval=double(score_sum/static_cast<long double>(frontier.size()));
        }
        log.committed=true;
        log.unique_parents=distinct_parents.size();
        log.step_sec=timer.elapsed()-t0;
        log.total_sec=timer.elapsed()-start;
        res.stats.total_transitions+=log.transitions;
        res.stats.turns.push_back(log);
    }
    res.completed=(res.reached_turn==cfg.max_turn);
    int best=frontier.front();
    for(int u:frontier)if(better(nodes[u].score,nodes[best].score,cfg.goal))best=u;
    res.score=nodes[best].score;
    for(int u=best;u!=0;u=nodes[u].parent)res.actions.push_back(*nodes[u].action);
    reverse(all(res.actions));
    res.stats.elapsed_sec=timer.elapsed()-start;
    if(cfg.log_stats)print_beam_stats(res.stats);
    return res;
}

// ---------- モンテカルロ法（各候補への繰り返しロールアウト） ----------
struct MonteCarloConfig {
    int simulations_per_action=30;
    double deadline_sec=1.9;
    Goal goal=Goal::Maximize;
    int top_k=5;
    bool log_stats=true;
    bool check_undo=false;
};
struct MCAccum {
    uint64_t n=0;
    double sum=0,sum_sq=0,min_score=numeric_limits<double>::infinity(),max_score=-numeric_limits<double>::infinity();
    void add(double v){++n;sum+=v;sum_sq+=v*v;min_score=min(min_score,v);max_score=max(max_score,v);}
    double mean()const{return n?sum/double(n):0.;}
    double stdev()const{return n?sqrt(max(0.,sum_sq/double(n)-mean()*mean())):0.;}
};
template<class Action> struct MCActionScore {Action action;MCAccum score;};
template<class Action> struct MCResult {
    optional<Action> best_action;
    vector<MCActionScore<Action>> ranking;
    MCAccum total_scores;
    uint64_t simulations=0;
    double elapsed_sec=0;
};
struct MCStats {
    uint64_t decisions=0,rollouts=0;
    double total_sec=0,max_decision_sec=0;
    void add(uint64_t n,double sec){++decisions;rollouts+=n;total_sec+=sec;max_decision_sec=max(max_decision_sec,sec);}
    void print(ostream& os=cerr)const{
        os<<"[MC] 決定ターン="<<decisions<<" 総試行="<<rollouts
          <<" 一ターン平均時間="<<(decisions?total_sec/double(decisions):0)
          <<"s 最大="<<max_decision_sec<<"s\n";
    }
};
template<class Action> void print_mc_result(const MCResult<Action>& r,int top_k,ostream& os=cerr){
    auto f=os.flags();auto p=os.precision();os<<fixed<<setprecision(3);
    os<<"[MC] 試行="<<r.simulations<<" 平均="<<r.total_scores.mean()
      <<" 標準偏差="<<r.total_scores.stdev()<<" 最小="
      <<(r.total_scores.n?r.total_scores.min_score:0.)<<" 最大="
      <<(r.total_scores.n?r.total_scores.max_score:0.)<<" 時間="<<r.elapsed_sec<<"s\n";
    for(int i=0;i<min(int(r.ranking.size()),top_k);++i){
        os<<"[MC] 上位"<<i+1<<" action=";
        debug_print_value(os,r.ranking[i].action);
        os<<" n="<<r.ranking[i].score.n<<" 平均="<<r.ranking[i].score.mean()
          <<" 標準偏差="<<r.ranking[i].score.stdev()<<'\n';
    }
    os.flags(f);os.precision(p);
}
// rollout(state, rng) -> double : Action適用後にコピーした状態を最後まで進め、評価値を返す。
// 毎試行Stateをコピーする素朴な版。大きな盤面には差分ロールアウト実装を推奨。
template<class Action,class State,class Rollout>
MCResult<Action> monte_carlo(const State& initial,const MonteCarloConfig& cfg,
                              const Timer& timer,RNG& rng,Rollout&& rollout){
    if(cfg.simulations_per_action<=0||cfg.top_k<0)throw invalid_argument("MC設定が不正");
    const double start=timer.elapsed();
    vector<Action> actions;
    initial.enumerate_actions(actions);
    MCResult<Action> r;
    r.ranking.reserve(actions.size());
    for(const auto& action:actions)r.ranking.push_back({action,{}});
    bool expired=false;
    // 1候補ずつまとめて試すと時間切れ時に先頭候補へ偏るため、ラウンドロビン。
    for(int rep=0;rep<cfg.simulations_per_action && !expired;++rep){
        for(auto& a:r.ranking){
            if(timer.elapsed()>=cfg.deadline_sec){expired=true;break;}
            State s=initial;
            check_apply_undo(s,a.action,cfg.check_undo,"MC最初の操作");
            s.apply(a.action);
            double score=rollout(s,rng);
            if(!isfinite(score))throw runtime_error("MCのスコアがNaN/Infです");
            a.score.add(score);r.total_scores.add(score);++r.simulations;
        }
    }
    erase_if(r.ranking,[](const auto& a){return a.score.n==0;});
    sort(all(r.ranking),[&](const auto& a,const auto& b){
        if(a.score.mean()!=b.score.mean())return better(a.score.mean(),b.score.mean(),cfg.goal);
        return a.score.n>b.score.n;
    });
    if(!r.ranking.empty())r.best_action=r.ranking.front().action;
    r.elapsed_sec=timer.elapsed()-start;
    if(cfg.log_stats)print_mc_result(r,cfg.top_k);
    return r;
}

// ---------- 初期化を省略するスタンプ式BFS ----------
struct BFSStats {
    uint64_t calls=0,visits=0;
    double total_sec=0,max_sec=0;
    void print(ostream& os=cerr)const {
        os<<"[BFS] 呼出="<<calls<<" 合計訪問="<<visits
          <<" 一回平均="<<(calls?total_sec/double(calls):0.)<<"s 最大="<<max_sec
          <<"s 総時間="<<total_sec<<"s\n";
    }
};
struct BFSResult {bool found=false;int goal=-1,visited=0;double elapsed_sec=0;};
struct StampedBFS {
    int n;
    vector<uint32_t> seen;
    vector<int> dist,parent,q;
    uint32_t stamp=0;
    BFSStats stats;
    static int validate_n(int count){
        if(count<0)throw invalid_argument("BFS頂点数が負");
        return count;
    }
    explicit StampedBFS(int count):n(validate_n(count)),seen(n,0),dist(n),parent(n){
        q.reserve(n);
    }
    // expand(v, push): push(to)で合法遷移を列挙。goal(v)は訪問した瞬間に判定。
    template<class Expand,class IsGoal>
    BFSResult run(int start,Expand&& expand,IsGoal&& goal){
        if(start<0||start>=n)throw out_of_range("BFS始点が範囲外");
        auto begin=Timer::Clock::now();
        // uint32_tが一周した時だけ初期化。それ以外は配列の再初期化なし。
        if(++stamp==0){fill(all(seen),0);stamp=1;}
        q.clear();q.push_back(start);
        seen[start]=stamp;dist[start]=0;parent[start]=-1;
        BFSResult result;
        for(size_t head=0;head<q.size();++head){
            ++result.visited;
            const int v=q[head];
            if(goal(v)){result.found=true;result.goal=v;break;}
            auto push=[&](int to){
                if(to<0||to>=n)throw out_of_range("BFS遷移先が範囲外");
                if(seen[to]==stamp)return;
                seen[to]=stamp;dist[to]=dist[v]+1;parent[to]=v;q.push_back(to);
            };
            expand(v,push);
        }
        result.elapsed_sec=chrono::duration<double>(Timer::Clock::now()-begin).count();
        ++stats.calls;stats.visits+=result.visited;
        stats.total_sec+=result.elapsed_sec;stats.max_sec=max(stats.max_sec,result.elapsed_sec);
        return result;
    }
    int distance(int v)const{
        if(v<0||v>=n)throw out_of_range("BFS頂点が範囲外");
        return stamp!=0 && seen[v]==stamp?dist[v]:-1;
    }
    vector<int> path(int goal)const{
        if(distance(goal)<0)return {};
        vector<int> p;
        for(int v=goal;v!=-1;v=parent[v])p.push_back(v);
        reverse(all(p));return p;
    }
};

// 時空間BFS: (時刻t, 位置v)を別状態として保持する。状態数=(T+1)*N。
// 障害物が時刻で変化する場合・待機が必要な場合に利用する。
struct TimeSpaceBFS {
    int positions,max_time;
    StampedBFS bfs;
    TimeSpaceBFS(int num_positions,int final_time)
        :positions(num_positions),max_time(final_time),
         bfs([&](){
            if(positions<=0||max_time<0||int64_t(positions)*(int64_t(max_time)+1)>INT_MAX)
                throw invalid_argument("時空間BFSのサイズが不正");
            return positions*(max_time+1);
         }()){}
    int encode(int t,int v)const{
        if(t<0||t>max_time||v<0||v>=positions)throw out_of_range("時空間座標が範囲外");
        return t*positions+v;
    }
    pair<int,int> decode(int id)const{return {id/positions,id%positions};}
    // neighbors(v, next_time, push_position), can_enter(time,position), goal(time,position)
    template<class Neighbors,class CanEnter,class IsGoal>
    BFSResult run(int start_pos,int start_time,Neighbors&& neighbors,CanEnter&& can_enter,
                  IsGoal&& goal,bool allow_wait=true){
        const int start=encode(start_time,start_pos);
        if(!can_enter(start_time,start_pos))return {};
        return bfs.run(start,[&](int id,auto&& push){
            auto [t,v]=decode(id);
            if(t==max_time)return;
            auto add=[&](int to){
                if(to<0||to>=positions)throw out_of_range("時空間BFS遷移先が範囲外");
                if(can_enter(t+1,to))push(encode(t+1,to));
            };
            if(allow_wait)add(v);
            neighbors(v,t+1,add);
        },[&](int id){auto [t,v]=decode(id);return goal(t,v);});
    }
    vector<pair<int,int>> path(int target_id)const{
        vector<pair<int,int>> out;
        for(int id:bfs.path(target_id))out.push_back(decode(id));
        return out;
    }
};

// ---------- 整数フィールドをビット列へパック ----------
// 各値を [min,max] のオフセットに直して圧縮する。
// 幅0（固定値）・幅64（64bit全体）に対応。範囲外の値は例外。
template<size_t N> struct BitPacker {
    using Bounds=array<pair<int64_t,int64_t>,N>;
    Bounds bounds{};
    array<int,N> shift{},width{};
    array<uint64_t,N> mask{};
    int total_bits=0;
    explicit BitPacker(const Bounds& ranges):bounds(ranges){
        for(size_t i=0;i<N;++i){
            auto [lo,hi]=bounds[i];
            if(lo>hi)throw invalid_argument("BitPacker: 下限が上限を超えています");
            __int128 span=(__int128)hi-(__int128)lo;
            uint64_t size_minus_one=uint64_t(span);
            const int bits=int(std::bit_width(size_minus_one));
            if(total_bits+bits>64)throw invalid_argument("BitPacker: 合計が64bitを超えています");
            shift[i]=total_bits;width[i]=bits;
            uint64_t ones=bits==64?~uint64_t(0):(bits==0?0:((uint64_t(1)<<bits)-1));
            mask[i]=(bits==64)?ones:(bits==0?0:(ones<<total_bits));
            total_bits+=bits;
        }
    }
    uint64_t encode(const array<int64_t,N>& values)const{
        uint64_t bits=0;
        for(size_t i=0;i<N;++i){
            if(values[i]<bounds[i].first||values[i]>bounds[i].second)
                throw out_of_range("BitPacker: 値が範囲外");
            uint64_t delta=uint64_t((__int128)values[i]-bounds[i].first);
            if(width[i])bits|=(delta<<shift[i]);
        }
        return bits;
    }
    int64_t get(uint64_t bits,size_t i)const{
        if(i>=N)throw out_of_range("BitPacker: 添字が範囲外");
        const uint64_t val=width[i]==0?0:((bits&mask[i])>>shift[i]);
        __int128 decoded=(__int128)bounds[i].first+val;
        return int64_t(decoded);
    }
    array<int64_t,N> decode(uint64_t bits)const{
        array<int64_t,N> values{};
        for(size_t i=0;i<N;++i)values[i]=get(bits,i);
        return values;
    }
    uint64_t set(uint64_t bits,size_t i,int64_t value)const{
        if(i>=N)throw out_of_range("BitPacker: 添字が範囲外");
        if(value<bounds[i].first||value>bounds[i].second)
            throw out_of_range("BitPacker: 値が範囲外");
        uint64_t v=uint64_t((__int128)value-bounds[i].first);
        return (bits&~mask[i])|(width[i]?v<<shift[i]:0);
    }
    void describe(ostream& os=cerr)const{
        os<<"[BitPacker] 合計="<<total_bits<<"bit\n";
        for(size_t i=0;i<N;++i)
            os<<"  field"<<i<<"=["<<bounds[i].first<<','<<bounds[i].second
              <<"] width="<<width[i]<<" shift="<<shift[i]
              <<" mask=0x"<<hex<<mask[i]<<dec<<'\n';
    }
};
} // namespace ahc
// END FILE heuristic/core.hpp
namespace ahc {
template<class Score> struct ScoreResult {
    std::optional<Score> score;
    std::string error;
    /// @brief scoreの有無で合法性を判定する。0点は無効解ではない。
    explicit operator bool() const { return score.has_value(); }
    static ScoreResult valid(Score x) { return {x,{}}; }
    static ScoreResult invalid(std::string e) { return {std::nullopt,std::move(e)}; }
};
// スコアを計算するのは解が完成したときだけ。
// 評価不能な解はBestOutputへ渡さない。
template<class Output,class Score=long long>
class OfficialRecorder {
    Goal goal_;
    std::function<ScoreResult<Score>(const Output&)> score_;
public:
    BestOutput<Output,Score> best;
    explicit OfficialRecorder(Goal goal,std::function<ScoreResult<Score>(const Output&)> f)
        :goal_(goal),score_(std::move(f)){}
    /// @return 合法かつ最良解を更新したときtrue。無効解や同点・悪化はfalse。
    /// @note 評価関数が投げた例外は呼び出し元へ伝播する。
    bool consider(const Output& out,bool verbose=false) {
        auto r=score_(out);
        if(!r){if(verbose)std::cerr<<"[公式評価] 無効解: "<<r.error<<'\n';return false;}
        bool updated=best.consider(*r.score,goal_,[&]{return out;});
        if(updated && verbose)std::cerr<<"[公式評価] 最良更新: "<<*r.score<<'\n';
        return updated;
    }
};
}
// END FILE heuristic/official_bridge.hpp
namespace problem {
using Input=translated_b::Input;
using Output=translated_b::Output;
constexpr ahc::Goal GOAL=ahc::Goal::Maximize;
inline Input read_input(std::istream& is){
    std::ostringstream buf;buf<<is.rdbuf();
    auto input=translated_b::parse_input(buf.str());
    if(input.N==0 || input.M==0)throw std::invalid_argument("N and M must be positive");
    return input;
}
inline void print_output(std::ostream& os,const Output& out){
    for(auto [x,y]:out.m)os<<x<<' '<<y<<'\n';
    for(auto action:out.actions)os<<action<<'\n';
}
inline ahc::ScoreResult<long long> official_score(const Input& in,const Output& out){
    if(in.N==0 || in.M==0)return ahc::ScoreResult<long long>::invalid("N and M must be positive");
    if(out.m.size()!=3 || out.actions.size()!=in.N*in.N)
        return ahc::ScoreResult<long long>::invalid("invalid output dimensions");
    // 探索が直接構築したOutputにも、公式パーサーと同じ形式検査を適用する。
    std::ostringstream text;print_output(text,out);
    auto parsed=translated_b::parse_output(in,text.str());
    if(auto* error=std::get_if<std::string>(&parsed))
        return ahc::ScoreResult<long long>::invalid(*error);
    auto [score,error]=translated_b::compute_score(in,std::get<Output>(parsed));
    if(!error.empty())return ahc::ScoreResult<long long>::invalid(error);
    return ahc::ScoreResult<long long>::valid(score);
}
// この問題で必ず成立する完成解。得点を狙うものではない。
inline Output greedy(const Input& in){
    Output out;
    out.m={{0,0},{0,0},{0,0}};
    out.actions.assign(in.N*in.N,0);
    return out;
}
}
// END FILE examples/official_score/case_b_adapter.hpp
using namespace std;
enum class Strategy {Greedy,SA,TreeBeam,MonteCarlo,All};
constexpr Strategy SEARCH_STRATEGY=Strategy::Greedy;
constexpr bool SHOW_DEBUG=true;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    bool verbose=false;
#ifdef LOCAL
    LocalOutput local_output;
    verbose=SHOW_DEBUG;
#endif
    try {
        auto in=problem::read_input(cin);
        ahc::OfficialRecorder<problem::Output,long long> recorder(
            problem::GOAL,[&](const auto& out){return problem::official_score(in,out);});
        auto fallback=problem::greedy(in);
        if(!recorder.consider(fallback,verbose))throw runtime_error("初期解が不正");
        // SA・Beam・MCの完成解は recorder.consider(out) に渡す。
        // SEARCH_STRATEGYの実装は問題固有の探索を実装してから接続する。
        if(!recorder.best.output)throw runtime_error("合法解なし");
        problem::print_output(cout,*recorder.best.output);
    }catch(const exception& e){cerr<<"[ERROR] "<<e.what()<<'\n';return 1;}
}
// END FILE examples/official_score/main_case_b.cpp

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
