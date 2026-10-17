// CC0-1.0: new algorithm regression tests.
#include <bits/stdc++.h>
#ifdef ALGO_INT64
#define int long long
#endif
#include "../algo/technique/rle.hpp"
#include "../algo/technique/compress.hpp"
#include "../algo/data_structure/bit.hpp"
#include "../algo/data_structure/segtree.hpp"
#include "../algo/graph/bfs.hpp"
#include "../algo/graph/dfs.hpp"
#include "../algo/graph/dijkstra.hpp"
#include "../algo/graph/floyd.hpp"
#include "../algo/graph/dsu.hpp"
#include "../algo/graph/lca.hpp"
#include "../algo/graph/scc.hpp"
#include "../algo/math/factor.hpp"
#include "../algo/math/divisors.hpp"
#include "../algo/math/sieve.hpp"
#include "../algo/math/prime_sieve.hpp"
#include "../algo/math/gcd.hpp"
#include "../algo/math/ext_gcd.hpp"
#include "../algo/math/mod_pow.hpp"
#include "../algo/math/mod_inv.hpp"
#include "../algo/math/comb.hpp"
#include "../algo/math/modint.hpp"
#include "../algo/math/div_floor.hpp"
#include "../algo/math/div_ceil.hpp"
#include "../algo/math/mat_pow.hpp"
#include "../algo/math/gf2.hpp"
#include "../algo/string/trie.hpp"
#include "../algo/string/calc_next.hpp"
#include "../algo/string/rolling_hash.hpp"
#include "../algo/dp/digit_dp.hpp"
#include "../algo/dp/lis.hpp"
#include "../algo/graph/euler_tour.hpp"
#include "../algo/graph/doubling.hpp"
#include "../algo/data_structure/lazy_segtree.hpp"
using namespace std;
mt19937 engine(20261017);
int rnd(int n) { return engine() % n; }
template<class Exception, class F> void throws(F operation) {
    bool caught = false;
    try { operation(); } catch (const Exception&) { caught = true; }
    assert(caught);
}

void test_sequences() {
    assert(rle_encode("").empty());
    throws<invalid_argument>([]{ rle_decode({{'a',-1}}); });
    for (int trial=0; trial<100; ++trial) {
        string s;
        for (int i=0,n=rnd(100); i<n; ++i) s += char('a'+rnd(4));
        assert(rle_decode(rle_encode(s)) == s);
        vector<int> input;
        for (char c : s) input.push_back(c-'a'-2);
        auto [rank,values] = compress(input);
        for (size_t i=0; i<input.size(); ++i) assert(values[rank[i]] == input[i]);
        vector<string> words{"z","a","z"};
        auto [r,v] = compress(words);
        assert((r == vector<int>{1,0,1}) && (v == vector<string>{"a","z"}));
        RollingHash hash(s);
        for (int i=0; i<50; ++i) {
            int l = rnd(s.size()+1), r = l+rnd(s.size()-l+1);
            RollingHash substring(s.substr(l,r-l));
            assert(hash.get(l,r) == substring.get(0,r-l));
        }
    }
    for(int trial=0; trial<100; ++trial) {
        std::string text;
        for(int i=0,n=rnd(50); i<n; ++i) text+=char('a'+rnd(26));
        auto next=calc_next(text);
        assert(next.size()==text.size()+1);
        for(size_t i=0;i<=text.size();++i) for(int c=0;c<26;++c) {
            auto expected=text.find(char('a'+c),i);
            assert(next[i][c]==static_cast<int>(expected==std::string::npos?text.size():expected));
        }
    }
    assert(calc_next("")==vector<vector<int>>(1,vector<int>(26,0)));
    throws<invalid_argument>([]{calc_next("A");});
    Trie trie;
    vector<string> words{"", "ab", "ab", "abc", "ax", "z"};
    for (const auto& word : words) trie.insert(word);
    assert(trie.count("") == 1 && trie.count("ab") == 2 && trie.count("a") == 0);
    assert(trie.lcp("abc",2) == 2 && trie.lcp("absent") == 2);
    assert(trie.lcp("zz",2) == 0 && trie.lcp("missing") == 0);
    throws<invalid_argument>([&]{trie.insert("AB");});
    assert(trie.count("ab") == 2);
}
void test_structures() {
    BIT<> empty(0); SegTree<> empty_tree(0);
    assert(empty.sum(0) == 0 && empty_tree.min(0,0) == numeric_limits<int>::max());
    throws<out_of_range>([&]{empty.add(0,1);});
    for (int n=1; n<50; ++n) {
        vector<int> a(n,0); BIT<> bit(n); SegTree<> seg(a); DSU dsu(n);
        vector<int> component(n); iota(component.begin(),component.end(),0);
        for (int step=0; step<200; ++step) {
            int v=rnd(n), delta=rnd(21)-10;
            a[v]+=delta; bit.add(v,delta); seg.set(v,a[v]);
            int l=rnd(n+1), r=rnd(n+1); if (l>r) swap(l,r);
            assert(bit.sum(l,r) == accumulate(a.begin()+l,a.begin()+r,static_cast<int>(0)));
            assert(seg.min(l,r) == (l==r ? numeric_limits<int>::max() : *min_element(a.begin()+l,a.begin()+r)));
            int x=rnd(n), y=rnd(n), old=component[y], target=component[x];
            assert(dsu.unite(x,y) == (old != target));
            for (int& c : component) if(c==old)c=target;
            assert(dsu.same(x,y));
            auto members = dsu.members(dsu.root(x));
            std::sort(members.begin(),members.end());
            std::vector<int> expected;
            for (int v=0; v<n; ++v) if(component[v]==target) expected.push_back(v);
            assert(members == expected && dsu.members(x) == dsu.members(dsu.root(x)));
            assert(dsu.size(x) == count(component.begin(),component.end(),target));
        }
    }
}
void test_graphs() {
    vector<vector<int>> empty; assert(floyd(empty));
    for (int trial=0; trial<100; ++trial) {
        int n=1+rnd(12), inf=1000000;
        vector<vector<int>> g(n), matrix(n,vector<int>(n,inf));
        vector<vector<pair<int,int>>> weighted(n);
        SCC scc(n);
        for (int i=0;i<n;++i) matrix[i][i]=0;
        for (int i=0;i<n;++i) for (int j=0;j<n;++j) if(rnd(4)==0) {
            int cost=rnd(10); g[i].push_back(j); weighted[i].emplace_back(j,cost);
            matrix[i][j]=min(matrix[i][j],cost); scc.add_edge(i,j);
        }
        auto all=matrix; assert(floyd(all,inf)); scc.build();
        auto dag=scc.dag();
        for (size_t i=0;i<dag.size();++i) for(int j:dag[i]) assert(static_cast<int>(i)<j);
        for(int source=0;source<n;++source) {
            assert(dijkstra(weighted,source,inf) == all[source]);
            auto distances=bfs(g,source); auto visited=dfs(g,source);
            for(int v=0;v<n;++v) {
                assert(visited[v] == (distances[v]>=0));
                bool together=all[source][v]!=inf && all[v][source]!=inf;
                assert((scc.id[source]==scc.id[v]) == together);
            }
        }
        auto first=scc.id; scc.build(); assert(scc.id==first);
    }
    vector<vector<int>> negative{{0,1,100},{100,0,-3},{1,100,0}};
    assert(!floyd(negative,100));
    vector<vector<int>> disconnected{{0,100,100},{100,0,-5},{100,100,0}};
    assert(floyd(disconnected,100) && disconnected[0][2]==100);
    throws<invalid_argument>([]{dijkstra({{{0,-1}}});});
    for (int trial=0;trial<50;++trial) {
        int n=1+rnd(100); vector<vector<int>> tree(n); vector<int> parent(n,0), depth(n,0);
        for(int v=1;v<n;++v) {int p=rnd(v);parent[v]=p;depth[v]=depth[p]+1;tree[v].push_back(p);tree[p].push_back(v);}
        LCA lca(tree);
        EulerTour euler(tree);
        assert(euler.order.size()==size_t(n) && euler.tour.size()==size_t(2*n-1));
        for(int v=0;v<n;++v) {
            assert(euler.order[euler.tin[v]]==v && euler.tour[euler.first[v]]==v);
            for(int u=0;u<n;++u) {
                int a=u;
                while(depth[a]>depth[v])a=parent[a];
                bool descendant = a==v;
                bool inside = euler.tin[v]<=euler.tin[u] && euler.tin[u]<euler.tout[v];
                assert(descendant==inside);
            }
        }
        for(size_t i=1;i<euler.tour.size();++i) {
            int a=euler.tour[i-1],b=euler.tour[i];
            assert(parent[a]==b || parent[b]==a);
        }
        for(int i=0;i<100;++i) {
            int a=rnd(n),b=rnd(n),x=a,y=b;
            while(depth[x]>depth[y])x=parent[x];
            while(depth[y]>depth[x])y=parent[y];
            while(x!=y){x=parent[x];y=parent[y];}
            assert(lca.query(a,b)==x);
            assert(lca.dist(a,b)==depth[a]+depth[b]-2*depth[x]);
            assert(lca.on_path(a,b,x));
        }
    }
    // 再帰ではスタックが深くなる形状。
    int n=100000; vector<vector<int>> path(n); SCC chain(n);
    for(int i=1;i<n;++i){path[i-1].push_back(i);path[i].push_back(i-1);chain.add_edge(i-1,i);}
    EulerTour euler(path); assert(euler.tout[0]==n && euler.tour.size()==size_t(2*n-1));
    LCA lca(path); assert(lca.query(n/2,n-1)==n/2); chain.build();
    assert(chain.groups.size()==size_t(n) && bfs(path)[n-1]==n-1 && dfs(path)[n-1]);
}
void test_math() {
    Sieve sieve(1000); auto primes=prime_sieve(1000);
    assert(prime_sieve(0)==vector<bool>{false}); assert(!Sieve(0).is_prime(0));
    for(int n=1;n<=1000;++n) {
        auto f=factor(n); assert(sieve.factor(n)==f);
        int product=1;
        for(auto [p,e]:f) for(int i=0;i<e;++i)product*=p;
        assert(product==n);
        vector<int> div;
        for(int d=1;d<=n;++d)if(n%d==0)div.push_back(d);
        assert(divisors(n)==div && sieve.divisors(n)==div);
        assert(primes[n]==(n>=2 && div.size()==2) && sieve.is_prime(n)==primes[n]);
    }
    for(int a=-30;a<=30;++a) for(int b=-30;b<=30;++b) {
        auto [g,x,y]=ext_gcd(a,b); assert(g==std::gcd(a,b) && a*x+b*y==g && ::gcd(a,b)==g);
        if(b) {
            assert(div_floor(a,b)==floor(double(a)/b));
            assert(div_ceil(a,b)==ceil(double(a)/b));
        }
    }
    throws<overflow_error>([]{div_floor(numeric_limits<int>::min(),-1);});
    throws<overflow_error>([]{div_ceil(numeric_limits<int>::min(),-1);});
    throws<domain_error>([]{div_floor(0,0);});
    for(int mod=1;mod<50;++mod)for(int a=-20;a<20;++a) {
        int power=1%mod,base=(a%mod+mod)%mod;
        for(int exponent=0;exponent<20;++exponent) {assert(mod_pow(a,exponent,mod)==power);power=power*base%mod;}
        if(mod>1 && std::gcd(a,mod)==1) assert((a*mod_inv(a,mod)%mod+mod)%mod==1);
        else if(mod>1) throws<domain_error>([&]{mod_inv(a,mod);});
    }
    Comb comb(50,101); vector<int> pascal{1};
    for(int n=0;n<=50;++n) {
        for(int k=0;k<=n;++k)assert(comb.query(n,k)==pascal[k]);
        vector<int> next(n+2,1);
        for(int k=1;k<=n;++k)next[k]=(pascal[k-1]+pascal[k])%101;
        pascal=next;
    }
    assert(comb.query(-1,0)==0 && comb.query(5,6)==0);
    using Mint=ModInt<101>;
    for(int a=0;a<101;++a)for(int b=0;b<101;++b) {
        assert((Mint(a)+Mint(b)).val()==(a+b)%101);
        assert((Mint(a)-Mint(b)).val()==(a-b+101)%101);
        assert((Mint(a)*Mint(b)).val()==(a*b)%101);
        if(b)assert((Mint(a)/Mint(b)*Mint(b)).val()==a);
    }
    throws<domain_error>([]{ModInt<8>(2).inv();});
    int max=numeric_limits<int>::max();
    assert(mod_pow(max-1,2,max)==1);
    assert(mod_inv(max-1,max)==max-1);
#ifdef ALGO_INT64
    using Large=ModInt<9223372036854775783LL>;
#else
    using Large=ModInt<2147483647>;
#endif
    assert((Large(max-2)+Large(max-2)-Large(max-2))==Large(max-2));
    vector<vector<int>> fib{{1,1},{1,0}};
    for(int k=0,a=0,b=1;k<20;++k){assert(mat_pow(fib,k,100000)[0][1]==a);int c=a+b;a=b;b=c;}
    assert(mat_pow({{5}},0,1)[0][0]==0);
}
void test_gf2_dp() {
    for(int trial=0;trial<300;++trial) {
        int h=rnd(6),w=rnd(7); vector<vector<int>> a(h,vector<int>(w)); vector<int>b(h);
        for(int i=0;i<h;++i){b[i]=rnd(2);for(int j=0;j<w;++j)a[i][j]=rnd(2);}
        auto result=gf2_solve(a,b,w); bool found=false;
        for(int bits=0;bits<(1<<w);++bits){
            bool valid=true;
            for(int i=0;i<h;++i){int x=0;for(int j=0;j<w;++j)x^=a[i][j]*((bits>>j)&1);valid&=x==b[i];}
            found|=valid;
        }
        assert(result.solvable==found && gf2_rank(a,w)==result.rank);
        if(found)for(int i=0;i<h;++i){int x=0;for(int j=0;j<w;++j)x^=a[i][j]*result.solution[j];assert(x==b[i]);}
    }
    auto shifted=gf2_solve({{0,1}}, {1},2); assert((shifted.solution==vector<int>{0,1}));
    for(int upper=0;upper<500;++upper) {
        auto generic = digit_dp<int>(to_string(upper),0,1,
            [](int& next,const int& previous,int,int){next+=previous;});
        assert(generic.back()[0]==1 && generic.back()[1]==upper);
        vector<int> counts(28,0);
        for(int x=0;x<=upper;++x){int sum=0;for(int y=x;y;y/=10)sum+=y%10;++counts[sum];}
        for(int target=0;target<28;++target)
            assert(digit_sum_count("00"+to_string(upper),target,7)==counts[target]%7);
    }
    auto add_count=[](int& to,const int& from,int,int){to+=from;};
    for(int base=2;base<=36;++base) for(int upper=0;upper<100;++upper) {
        string digits;
        for(int x=upper;;x/=base){int d=x%base;digits.insert(digits.begin(),char(d<10?'0'+d:'A'+d-10));if(x<base)break;}
        auto count=digit_dp<int>(digits,0,1,add_count,base);
        assert(count.back()[0]==1 && count.back()[1]==upper);
        vector<int> expected(200,0);
        for(int x=0;x<=upper;++x){int sum=0;for(int y=x;y;y/=base)sum+=y%base;++expected[sum];}
        for(int target=0;target<2*base;++target)
            assert(digit_sum_count(digits,target,97,base)==expected[target]%97);
    }
    auto large_base=digit_dp<int>(vector<int>{1,0},0,1,add_count,100);
    assert(large_base.back()[0]+large_base.back()[1]==101);
    assert(digit_dp<int>(string("ff"),0,1,add_count,16).back()[1]==255);
    throws<invalid_argument>([&]{digit_dp<int>(string("2"),0,1,add_count,2);});
    throws<invalid_argument>([&]{digit_dp<int>(string("0"),0,1,add_count,1);});
    throws<invalid_argument>([&]{digit_dp<int>(string("0"),0,1,add_count,37);});
    throws<invalid_argument>([&]{digit_dp<int>(vector<int>{100},0,1,add_count,100);});
    throws<invalid_argument>([&]{digit_dp<int>(vector<int>{},0,1,add_count,10);});
    assert(digit_sum_count("0",0)==1 && digit_sum_count("9",10)==0);
    throws<invalid_argument>([]{digit_sum_count("abc",0);});
}

struct Affine { int a,b; };
using SumNode=LazyValue<int>;
SumNode affine_op(SumNode a,SumNode b){return {(a.value+b.value)%101,a.size+b.size};}
SumNode affine_e(){return {0,0};}
SumNode affine_map(Affine f,SumNode x){return {(f.a*x.value+f.b*x.size)%101,x.size};}
Affine affine_compose(Affine f,Affine g){return {f.a*g.a%101,(f.a*g.b+f.b)%101};}
Affine affine_id(){return {1,0};}
using AffineTree=LazySegTree<SumNode,affine_op,affine_e,Affine,affine_map,affine_compose,affine_id>;
string text_op(string a,string b){return a+b;}
string text_e(){return {};}
string text_map(optional<char> f,string x){return f ? string(x.size(),*f) : x;}
optional<char> text_compose(optional<char> f,optional<char> g){return f ? f:g;}
optional<char> text_id(){return nullopt;}
void test_additions() {
    assert(EulerTour({}).order.empty());
    throws<invalid_argument>([]{EulerTour t({{1},{0},{}});});
    throws<invalid_argument>([]{EulerTour t({{1,2},{0,2},{0,1}});});
    for(int trial=0;trial<100;++trial) {
        int n=1+rnd(30); vector<int> next(n);
        for(int& to:next)to=rnd(n+1)-1;
        Doubling db(next,200);
        for(int v=0;v<n;++v)for(int steps=0,at=v;steps<=200;++steps){
            assert(db.jump(v,steps)==at);
            if(at!=-1)at=next[at];
        }
    }
    Doubling full({1,2,0},numeric_limits<uint64_t>::max());
    assert(full.jump(0,numeric_limits<uint64_t>::max())==0);
    Doubling zero({0},0); assert(zero.jump(0,0)==0);
    throws<out_of_range>([&]{zero.jump(0,1);});
    assert(Doubling({},0).jump(-1,0)==-1);
    for(int trial=0;trial<300;++trial) {
        vector<int> a(rnd(12));for(int& x:a)x=rnd(7)-3;
        for(bool strict:{false,true}) {
            auto ids = strict?LIS<true>(a):LIS<false>(a);
            int best=0;
            for(int mask=0;mask<(1<<a.size());++mask){
                bool valid=true;int count=0,last=0;
                for(size_t i=0;i<a.size();++i)if((mask>>i)&1){
                    if(count && (strict ? last>=a[i] : last>a[i]))valid=false;
                    last=a[i];++count;
                }
                if(valid)best=max(best,count);
            }
            assert(ids.size()==size_t(best));
            for(size_t i=1;i<ids.size();++i){
                assert(ids[i-1]<ids[i]);
                assert(strict ? a[ids[i-1]]<a[ids[i]] : a[ids[i-1]]<=a[ids[i]]);
            }
        }
    }
    for(int n=0;n<25;++n) {
        vector<int> add(n,0),set(n,0),affine(n,0);
        vector<LazyValue<int>> initial(n,{0,1});
        LazyAddSum<> add_sum(initial); LazyAddMin<> add_min(initial);
        LazySetSum<> set_sum(initial); LazySetMin<> set_min(initial);
        AffineTree affine_tree(initial);
        for(int step=0;step<200;++step) {
            int l=rnd(n+1),r=rnd(n+1);if(l>r)swap(l,r);
            int delta=rnd(21)-10,assignment=rnd(21)-10;
            add_sum.apply(l,r,delta);add_min.apply(l,r,delta);
            set_sum.apply(l,r,assignment);set_min.apply(l,r,assignment);
            Affine f{rnd(3),rnd(10)};affine_tree.apply(l,r,f);
            for(int i=l;i<r;++i){add[i]+=delta;set[i]=assignment;affine[i]=(f.a*affine[i]+f.b)%101;}
            set_sum.apply(0,n,nullopt);set_min.apply(0,n,nullopt);
            if(n && step%7==0) {
                int i=rnd(n),v=rnd(21)-10;
                add[i]=v;set[i]=v;
                add_sum.set(i,{v,1});add_min.set(i,{v,1});
                set_sum.set(i,{v,1});set_min.set(i,{v,1});
                assert(add_sum.get(i).value==v && set_min.get(i).value==v);
            }
            l=rnd(n+1);r=rnd(n+1);if(l>r)swap(l,r);
            assert(add_sum.prod(l,r).value==accumulate(add.begin()+l,add.begin()+r,static_cast<int>(0)));
            assert(set_sum.prod(l,r).value==accumulate(set.begin()+l,set.begin()+r,static_cast<int>(0)));
            assert(affine_tree.prod(l,r).value==accumulate(affine.begin()+l,affine.begin()+r,static_cast<int>(0))%101);
            int add_expected=l==r?numeric_limits<int>::max():*min_element(add.begin()+l,add.begin()+r);
            int set_expected=l==r?numeric_limits<int>::max():*min_element(set.begin()+l,set.begin()+r);
            assert(add_min.prod(l,r).value==add_expected && set_min.prod(l,r).value==set_expected);
            assert(add_sum.all_prod().value==accumulate(add.begin(),add.end(),static_cast<int>(0)));
        }
    }
    using TextTree=LazySegTree<string,text_op,text_e,optional<char>,text_map,text_compose,text_id>;
    TextTree text(vector<string>{"a","b","c"});
    text.apply(1,2,'x');assert(text.prod(0,3)=="axc");
    text.apply(0,2,'z');assert(text.prod(0,3)=="zzc");
    LazySetMin<> max_value(vector<LazyValue<int>>{{numeric_limits<int>::max(),1}});
    max_value.apply(0,1,0);assert(max_value.get(0).value==0);
}

signed main() {
    test_sequences(); test_structures(); test_graphs(); test_math(); test_gf2_dp(); test_additions();
    cout << "algorithm tests passed (int=" << sizeof(int)*8 << ")\n";
}
