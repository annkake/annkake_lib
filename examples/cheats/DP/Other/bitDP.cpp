// 参照URL: https://github.com/annkake/annkake_lib
#include <bits/stdc++.h>
using namespace std;

#define rep(i,k,n) for (int i = k; i < (n); ++i)
const long long MOD1 = 1000000007;
const long long MOD2 = 998244353;
typedef long long ll;
typedef pair<ll, ll> P;
const long long INF = 1e17;

int main(){
    while(true){
        int n;
        cin >> n;
        if(n==0){
            break;
        }
        //t[i][j]:i番目のステージをj番目の装備で
        //攻略する時間(j=0は装備なし)
        vector t(n+10,vector<int>(n+10));

        for(int i=1;i<=n;i++){
            for(int j=0;j<=n;j++){
                cin >> t[i][j];
            }
        }

        //dp[S]:Sをクリアするのに必要な最小時間
        vector<int> dp(1<<n,1e9);
        dp[0] = 0;
        for(int s=0;s<1<<n;s++){
            for(int x=0;x<n;x++){
                if(s&(1<<x)){
                    int ss = s^(1<<x);
                    dp[s] = min(dp[s],dp[ss]+t[x+1][0]);
                    for(int y=0;y<n;y++){
                        if(ss&(1<<y))dp[s] = min(dp[s],dp[ss]+t[x+1][y+1]);
                    }
                }
            }
        }

        cout << dp[(1<<n)-1] << endl;


    }
    return 0;
}