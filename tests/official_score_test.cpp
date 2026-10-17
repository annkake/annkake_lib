#include "../examples/official_score/case_b_adapter.hpp"
#include <cassert>
#include <random>
struct Candidate {int value;};
int main(){
    using Result=ahc::ScoreResult<long long>;
    ahc::OfficialRecorder<Candidate,long long> maximum(ahc::Goal::Maximize,
        [](const Candidate& x){return x.value<0?Result::invalid("negative"):Result::valid(x.value);});
    assert(!maximum.consider(Candidate{-1}) && !maximum.best.output);
    assert(maximum.consider(Candidate{0}) && *maximum.best.score==0);
    Candidate candidate{7}; assert(maximum.consider(candidate)); candidate.value=100;
    assert(maximum.best.output->value==7);
    assert(!maximum.consider(Candidate{7}) && !maximum.consider(Candidate{4}));
    assert(!maximum.consider(Candidate{-1}) && *maximum.best.score==7);
    ahc::OfficialRecorder<Candidate,long long> minimum(ahc::Goal::Minimize,
        [](const Candidate& x){return Result::valid(x.value);});
    assert(minimum.consider(Candidate{5}));assert(minimum.consider(Candidate{-3}));
    assert(!minimum.consider(Candidate{0}) && *minimum.best.score==-3);
    ahc::OfficialRecorder<Candidate,long long> broken(ahc::Goal::Maximize,
        [](const Candidate&)->Result{throw std::runtime_error("evaluation failed");});
    bool caught=false;
    try{broken.consider(Candidate{0});}catch(const std::runtime_error&){caught=true;}
    assert(caught && !broken.best.output);

    std::mt19937 random(71);
    for(std::size_t n=1;n<=5;++n)for(std::size_t m=1;m<=5;++m){
        problem::Input in{n,m,{}};
        for(std::size_t i=0;i<n*n;++i)in.targets.emplace_back(random()%n,random()%n);
        auto out=problem::greedy(in);
        for(auto& [x,y]:out.m){x=random()%n;y=random()%n;}
        for(auto& action:out.actions)action=random()%std::min<std::size_t>(m,3);
        auto expected=translated_b::compute_score(in,out);
        auto actual=problem::official_score(in,out);
        assert(actual && expected.second.empty() && *actual.score==expected.first);
        std::ostringstream printed;problem::print_output(printed,out);
        assert(std::holds_alternative<problem::Output>(translated_b::parse_output(in,printed.str())));
        auto bad=out;bad.actions.pop_back();assert(!problem::official_score(in,bad));
        bad=out;bad.m.pop_back();assert(!problem::official_score(in,bad));
        bad=out;bad.m[0].first=-1;assert(!problem::official_score(in,bad));
        bad=out;bad.m[0].second=static_cast<int>(n);assert(!problem::official_score(in,bad));
        bad=out;bad.actions[0]=m;assert(!problem::official_score(in,bad));
    }
    std::istringstream sample("2 3\n0 0\n0 1\n1 0\n1 1\n");
    auto in=problem::read_input(sample);auto out=problem::greedy(in);
    auto [cost,score,error,simulation]=translated_b::compute_score_details(in,out,4);
    assert(cost==6 && score==1142857 && error.empty());
    assert(problem::official_score(in,out).score==score);
    std::cout<<"official score bridge tests passed\n";
}
