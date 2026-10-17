#include "../examples/official_score/case_b_adapter.hpp"
int main(int argc,char** argv){
    if(argc!=3)return 1;
    std::ifstream input(argv[1]),output(argv[2]);
    auto in=problem::read_input(input);
    std::ostringstream text;text<<output.rdbuf();
    auto parsed=translated_b::parse_output(in,text.str());
    if(auto* out=std::get_if<problem::Output>(&parsed)){
        auto result=problem::official_score(in,*out);
        std::cout<<"Score = "<<(result?*result.score:0)<<'\n';
    }else std::cout<<"Score = 0\n";
}
