#include "case_b_adapter.hpp"
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
