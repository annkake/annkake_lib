// 参照URL: https://github.com/annkake/annkake_lib
#include <bits/stdc++.h>
using namespace std;

#define rep(i,k,n) for (int i = k; i < (n); ++i)
const long long MOD1 = 1000000007;
const long long MOD2 = 998244353;
typedef long long ll;
typedef pair<ll, ll> P;
const long long INF = 1e17;

string regstring;
int reg_idx;

void __init(string s){
    reg_idx = 0;
    regstring = s;
}

ll strtonum(){
    string t = "";
    while(regstring[reg_idx]!=']'){
        t += regstring[reg_idx];
        reg_idx++;
    }
    return stoll(t);
}


ll f(int i){
    ll res = 0LL;
    vector<ll> a;
    while(true){
        reg_idx++;
        if(regstring[reg_idx]=='['){
            a.push_back(f(i+1));
        }
        else if(regstring[reg_idx]==']'){
            sort(a.begin(),a.end());
            for(int j=0;j<(a.size()+1)/2;j++){
                res += a[j];
            }
            return res;
        }
        else{
            res += strtonum();
            return (res+1)/2;
        }
    }
}

int main(){
    int n;
    cin >> n;
    for(int i=0;i<n;i++){
        string s;
        cin >> s;
        __init(s);
        ll ans = f(0);
        cout << ans << endl;
    }

    return 0;
}

/*
#include<iostream>
#include<string>
#include<cctype>
#include<vector>
#include<algorithm>
using namespace std;
string s;
int now;
int rec(){
  if(isdigit(s[now])){
    int res = 0;
    while(now < s.size() && isdigit(s[now])) res = res * 10 + s[now++] - '0';
    return (res + 1) / 2;
  }else{
    vector<int> dat; int res = 0;
    while(s[now] == '[') now++ , dat.push_back(rec()) , now++;
    sort(dat.begin(),dat.end());
    for(int i = 0 ; i < ( dat.size() + 1 ) / 2 ; i++ ) res += dat[i];
    return res;
  }
}
int main(){
  int q;
  cin >> q;
  while(q--){
    now = 0;
    cin >> s;
    cout << rec() << endl;
  }
}


字句/構文解析.時間はかかったけど実装できて感動(ソフ演のおかげ).
選挙で勝つ最小得票数を返す再帰関数f(nest)を用意する.
解析したい文字を一文字ずつ見て、'['ならf(nest+1)を
配列にpush_back,']'なら配列の小さい方から過半数を
足し合わせて、それを返り値とする.数字ならベース
ステップであるから書かれた数字nの(n+1)/2を返り値とする.
「解析したい文字を一文字ずつ見る」のが大事そう.
*/