// 壊れたundoを故意に実装。非ゼロ終了が正しい結果。
#include "../heuristic/core.hpp"
struct Action {int step=1;};
struct BadState {
    int x=0;
    int apply(const Action& a){int old=x;x+=a.step;return old;}
    void undo(const Action&,int){ /* 誤って何も戻さない */ }
    auto debug_snapshot()const{return tuple{x};}
};
int main(){BadState state;ahc::check_apply_undo(state,Action{},true,"故意のバグ");}
