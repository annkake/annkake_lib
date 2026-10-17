// 参照URL: https://github.com/annkake/annkake_lib
// AHC用 単一ファイル版（このファイルでは下記ヘッダーを連結して提出できる）
#include "core.hpp"

// ============================================================
// ★本番時に最初に変更する場所：コンパイルコマンドを変える必要なし
// ============================================================
enum class Strategy { Greedy, SA, TreeBeam, MonteCarlo, All };
constexpr Strategy SEARCH_STRATEGY = Strategy::All;
constexpr bool SHOW_DEBUG = false;
constexpr bool CHECK_UNDO = true;  // 初期開発時はtrue。検証後の高速化時はfalse。
constexpr double TIME_LIMIT = 1.90;
constexpr ahc::Goal GOAL = ahc::Goal::Maximize;

// ============================================================
// ★ここから問題固有。例題：N個からちょうどK個選んで総和を最大化する
// 実際のAHCでは Input / Output / State / Move / Action を置き換える。
// ============================================================
struct Input {
    int N=0,K=0;
    vector<int> A;
    bool read(){
        if(!(cin>>N>>K))return false;
        if(N<0||K<0||K>N)throw invalid_argument("入力のN,Kが不正");
        A.resize(N);
        for(int& a:A)cin>>a;
        return true;
    }
};
struct Output {
    vector<int> selected;
    void print()const{
        for(size_t i=0;i<selected.size();++i)
            cout<<(i?" ":"")<<selected[i];
        cout<<'\n';
    }
    auto debug_fields()const{return tie(selected);}
};
struct Action {
    int index=0;
    auto debug_fields()const{return tie(index);}
};
struct Move {
    int take=0,drop=0;
    auto debug_fields()const{return tie(take,drop);}
};
// SAでは完成解の局所変更を行う。選択側と非選択側をそれぞれ配列保持する。
struct SAState {
    const Input* in;
    vector<int> selected,unselected;
    int score=0; // 探索用スコア（この例では公式スコアと同じ）
    explicit SAState(const Input& in):in(&in){
        for(int i=0;i<in.N;++i){
            if(i<in.K){selected.push_back(i);score+=in.A[i];}
            else unselected.push_back(i);
        }
    }
    struct Undo {int old_score;};
    Undo apply(const Move& m){
        Undo u{score};
        int old=selected[m.take],now=unselected[m.drop];
        score+=in->A[now]-in->A[old];
        swap(selected[m.take],unselected[m.drop]);
        return u;
    }
    void undo(const Move& m,const Undo& u){
        swap(selected[m.take],unselected[m.drop]);
        score=u.old_score;
    }
    auto debug_snapshot()const{return tuple{selected,unselected,score};}
    bool is_complete()const{return int(selected.size())==in->K;}
    int absolute_score()const{return score;}
    Output make_output()const{auto a=selected;sort(all(a));return {a};}
};
// Beam・MCでは少しずつK個選ぶ。インデックス昇順に遷移して重複を削減する。
struct BeamState {
    const Input* in;
    vector<int> selected;
    int last=-1;
    int score=0;
    uint64_t hash_value=0;
    explicit BeamState(const Input& in):in(&in){}
    static uint64_t mix(uint64_t x){
        x+=0x9e3779b97f4a7c15ULL;
        x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;
        return x^(x>>31);
    }
    void enumerate_actions(vector<Action>& actions)const{
        const int left=in->K-int(selected.size());
        for(int i=last+1;i+left<=in->N;++i)actions.push_back({i});
    }
    struct Undo {int old_last;int old_score;uint64_t old_hash;};
    Undo apply(const Action& a){
        Undo u{last,score,hash_value};
        selected.push_back(a.index);
        last=a.index;
        score+=in->A[a.index];
        hash_value^=mix(uint64_t(a.index));
        return u;
    }
    void undo(const Action&,const Undo& u){
        selected.pop_back();last=u.old_last;score=u.old_score;hash_value=u.old_hash;
    }
    uint64_t hash()const{return hash_value;}
    auto debug_snapshot()const{return tuple{selected,last,score,hash_value};}
    bool is_complete()const{return int(selected.size())==in->K;}
    int absolute_score()const{return score;}
    Output make_output()const{return {selected};}
};

// 提出用の合法解を必ず最初に保存する。以降の全戦略は公式スコアで競争する。
Output greedy(const Input& in){
    vector<int> order(in.N);
    iota(all(order),0);
    sort(all(order),[&](int a,int b){return in.A[a]>in.A[b];});
    order.resize(in.K);sort(all(order));
    return {order};
}
int official_score(const Input& in,const Output& out){
    int s=0;for(int i:out.selected)s+=in.A[i];return s;
}

Output solve(const Input& in,const ahc::Timer& timer){
    ahc::RNG rng;
    ahc::BestOutput<Output,int> best;
    auto fallback=greedy(in);
    best.consider(official_score(in,fallback),GOAL,[&](){return fallback;});
    if(in.K==0 || in.K==in.N)return *best.output;
    if constexpr(SEARCH_STRATEGY==Strategy::Greedy)return *best.output;

    // 異なる探索の公式スコアを1つのBestOutputに集約する。
    auto record=[&](const auto& s){
        if(s.is_complete()){
            bool updated=best.consider(s.absolute_score(),GOAL,[&](){return s.make_output();});
            if(updated)AHC_DEBUG(SHOW_DEBUG,*best.score);
        }
    };

    const double now=timer.elapsed();
    const double remain=max(0.,TIME_LIMIT-now);
    // Allのときは時間を事前に分配。各探索のdeadlineは共通Timerからの絶対時刻。
    const double sa_end=SEARCH_STRATEGY==Strategy::All?now+remain*0.40:TIME_LIMIT;
    const double beam_end=SEARCH_STRATEGY==Strategy::All?now+remain*0.75:TIME_LIMIT;
    const double mc_end=TIME_LIMIT;

    if constexpr(SEARCH_STRATEGY==Strategy::All||SEARCH_STRATEGY==Strategy::SA){
        ahc::SAConfig cfg;
        cfg.deadline_sec=sa_end;
        cfg.start_temp=30;cfg.end_temp=0.1;
        cfg.neighborhood_weights={1};
        cfg.time_bins=10;cfg.log_stats=true;cfg.check_undo=CHECK_UNDO;
        auto propose=[](const SAState& s,int,ahc::RNG& rnd)->optional<Move>{
            if(s.selected.empty()||s.unselected.empty())return nullopt;
            return Move{rnd.uniform_int(0,int(s.selected.size())),
                        rnd.uniform_int(0,int(s.unselected.size()))};
        };
        auto res=ahc::simulated_annealing(SAState(in),cfg,timer,rng,propose,record);
        AHC_DEBUG(SHOW_DEBUG,res.stats.total.accepted);
    }
    if constexpr(SEARCH_STRATEGY==Strategy::All||SEARCH_STRATEGY==Strategy::TreeBeam){
        ahc::BeamConfig cfg;
        cfg.deadline_sec=beam_end;
        cfg.max_turn=in.K;cfg.width=100;
        cfg.deduplicate=true;cfg.track_diversity=true;
        cfg.log_stats=true;cfg.check_undo=CHECK_UNDO;
        auto res=ahc::tree_beam_search<Action>(BeamState(in),cfg,timer,record);
        AHC_DEBUG(SHOW_DEBUG,res.reached_turn);
    }
    if constexpr(SEARCH_STRATEGY==Strategy::All||SEARCH_STRATEGY==Strategy::MonteCarlo){
        ahc::MCStats total;
        BeamState state(in);
        for(int turn=0;turn<in.K;++turn){
            if(timer.elapsed()>=mc_end)break;
            ahc::MonteCarloConfig cfg;
            cfg.deadline_sec=mc_end;
            cfg.simulations_per_action=12;
            cfg.top_k=3;cfg.log_stats=true;cfg.check_undo=CHECK_UNDO;
            auto rollout=[&](BeamState& s,ahc::RNG& gen)->double{
                // 先の合法手をランダムに選び、完成解の得点を評価する。
                vector<Action> actions;
                while(!s.is_complete()){
                    actions.clear();s.enumerate_actions(actions);
                    if(actions.empty())return -1e18;
                    const Action action=actions[gen.uniform_int(0,int(actions.size()))];
                    ahc::check_apply_undo(s,action,CHECK_UNDO,"MCロールアウト");
                    s.apply(action);
                }
                record(s);
                return double(s.absolute_score());
            };
            auto r=ahc::monte_carlo<Action>(state,cfg,timer,rng,rollout);
            total.add(r.simulations,r.elapsed_sec);
            if(!r.best_action)break;
            state.apply(*r.best_action);
            record(state);
        }
        total.print();
    }
    AHC_DEBUG(SHOW_DEBUG,best.updates);
    return *best.output;
}
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
#ifdef LOCAL
    LocalOutput local_output;
#endif
    ahc::Timer timer;
    Input input;
    if(!input.read())return 0;
    auto answer=solve(input,timer);
    answer.print(); // 標準出力には回答だけを出す
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
