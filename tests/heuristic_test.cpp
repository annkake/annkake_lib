#include "../heuristic/core.hpp"

struct Step {int to; auto debug_fields()const{return tie(to);} };
struct Game {
    int pos=0;
    int score=0;
    struct Undo {int pos;int score;};
    Undo apply(const Step& s){Undo b{pos,score};pos=s.to;score+=s.to;return b;}
    void undo(const Step&,const Undo& b){pos=b.pos;score=b.score;}
    void enumerate_actions(vector<Step>& a)const{
        if(pos<4){a.push_back({pos+1});a.push_back({pos+2});}
    }
    uint64_t hash()const{return uint64_t(pos);}
    auto debug_snapshot()const{return tuple{pos,score};}
    bool complete()const{return pos>=3;}
};
struct Decrease {
    int score=5;
    int pos=0;
    struct Undo{int score;int pos;};
    Undo apply(const Step&){auto old=Undo{score,pos};--score;++pos;return old;}
    void undo(const Step&,const Undo& old){score=old.score;pos=old.pos;}
    auto debug_snapshot()const{return tuple{score,pos};}
};

int main(){
    // debug: ネストしたvector/pair/tuple/optional/ユーザー構造体
    {
        ostringstream out;
        vector<pair<int,optional<Step>>> data{{1,Step{2}},{3,nullopt}};
        debug_print_value(out,data);
        assert(out.str()=="{(1, some((2))), (3, nullopt)}");
    }
    // BitPacker: 5+5+3+2+4+4=23bit の例
    {
        ahc::BitPacker<6> b({{{0,19},{0,19},{0,7},{0,3},{0,15},{0,15}}});
        assert(b.total_bits==23);
        assert((b.shift==array<int,6>{0,5,10,13,15,19}));
        assert(b.mask[0]==31 && b.mask[1]==(31ULL<<5));
        array<int64_t,6> in{18,11,7,2,13,5};
        uint64_t packed=b.encode(in);
        assert(b.decode(packed)==in);
        assert(b.get(b.set(packed,2,1),2)==1);
        assert(b.get(packed,2)==7);
        ahc::BitPacker<3> b2({{{-10,-2},{5,5},{0,63}}});
        assert(b2.decode(b2.encode({-4,5,63}))==(array<int64_t,3>{-4,5,63}));
        ahc::BitPacker<2> b3({{{INT64_MIN,INT64_MAX},{-1,-1}}});
        assert(b3.total_bits==64);
        assert(b3.decode(b3.encode({INT64_MIN,-1}))==(array<int64_t,2>{INT64_MIN,-1}));
        assert(b3.decode(b3.encode({INT64_MAX,-1}))==(array<int64_t,2>{INT64_MAX,-1}));
        bool thrown=false;
        try{b.encode({20,0,0,0,0,0});}catch(const out_of_range&){thrown=true;}
        assert(thrown);
        thrown=false;
        try{ahc::BitPacker<2> too_big({{{0,INT64_MAX},{0,7}}});}catch(const invalid_argument&){thrown=true;}
        assert(thrown);
    }
    // スタンプBFS：100回初期化なしで探索し、距離・パス・統計を検証
    {
        ahc::StampedBFS bfs(30);
        assert(bfs.distance(0)==-1); // 一度も走っていない場合
        for(int goal=0;goal<100;++goal){
            int dest=goal%30;
            auto r=bfs.run(0,[](int v,auto push){if(v+1<30)push(v+1);},
                         [&](int v){return v==dest;});
            assert(r.found && r.goal==dest);
            assert(bfs.distance(dest)==dest);
            assert(int(bfs.path(dest).size())==dest+1);
        }
        assert(bfs.stats.calls==100);
        bfs.stamp=UINT32_MAX; // スタンプが周回しても正しい
        auto r=bfs.run(0,[](int,auto){},[](int v){return v==0;});
        assert(r.found && bfs.stamp==1);
        assert(bfs.distance(29)==-1);
    }
    // 時空間BFS：時刻1だけ位置1が通れず、位置0で待機する必要あり。
    {
        ahc::TimeSpaceBFS bfs(3,5);
        auto r=bfs.run(0,0,[](int v,int,auto push){if(v+1<3)push(v+1);},
            [](int t,int v){return !(t==1&&v==1);},
            [](int,int v){return v==2;},true);
        assert(r.found);
        auto path=bfs.path(r.goal);
        assert((path==vector<pair<int,int>>{{0,0},{1,0},{2,1},{3,2}}));
    }
    // apply->undo は必ず元へ戻る。SAは棄却候補をobserverに渡す。
    {
        ahc::Timer timer;ahc::RNG rng;
        ahc::BestOutput<int,int> best;
        auto record=[&](const Decrease& s){
            best.consider(-s.score,ahc::Goal::Maximize,[&](){return s.pos;});
        };
        auto result=ahc::simulated_annealing(Decrease{},
            ahc::SAConfig{.deadline_sec=0.006,.start_temp=0,.end_temp=0,
                          .goal=ahc::Goal::Maximize,.neighborhood_weights={1,2},
                          .check_interval=1,.time_bins=4,.log_stats=false,.check_undo=true},
            timer,rng,
            [](const Decrease&,int,ahc::RNG&)->optional<Step>{return Step{0};},
            record);
        // 探索スコアは下がるので全棄却。ただし公式スコア=-scoreは改善。
        assert(best.score && *best.score==-4);
        assert(best.output && *best.output==1);
        assert(result.stats.total.tried>0);
        assert(result.stats.total.accepted==0);
        assert(result.best_state.score==5);
        uint64_t b=0;for(const auto& x:result.stats.phases)b+=x.tried;
        assert(b==result.stats.total.tried);
    }
    // 木上ビーム：候補の戻し・枝刈り・親経由パス復元
    {
        ahc::Timer timer;
        ahc::BeamConfig cfg;
        cfg.max_turn=2;cfg.width=2;cfg.log_stats=false;cfg.check_undo=true;
        auto res=ahc::tree_beam_search<Step>(Game{},cfg,timer);
        assert(res.completed && res.reached_turn==2);
        assert(res.actions.size()==2);
        Game s;for(auto a:res.actions)s.apply(a);
        assert(s.score==res.score);
        assert(res.stats.total_transitions>=2);
        assert(res.stats.turns.size()==2);
        assert(res.stats.turns[0].selected<=2);
    }
    // MonteCarlo：2種の操作を繰り返し評価し、上位行動を抽出する。
    {
        ahc::Timer timer;ahc::RNG rng;
        ahc::MonteCarloConfig cfg;cfg.simulations_per_action=6;cfg.log_stats=false;cfg.check_undo=true;
        auto r=ahc::monte_carlo<Step>(Game{},cfg,timer,rng,
            [](Game& s,ahc::RNG&)->double{return double(s.score);});
        assert(r.best_action && r.best_action->to==2);
        assert(r.ranking.size()==2);
        assert(r.simulations==12);
        assert(r.ranking.front().score.stdev()==0);
        ahc::MCStats totals;totals.add(r.simulations,r.elapsed_sec);
        assert(totals.decisions==1&&totals.rollouts==12);
    }
    cerr<<"全テスト成功\n";
}
