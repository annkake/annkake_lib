#pragma once
#include "translated_case_b.hpp"
#include "../../heuristic/official_bridge.hpp"
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
