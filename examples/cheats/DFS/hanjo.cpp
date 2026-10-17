#include <bits/stdc++.h>
using namespace std;
#include <atcoder/all>
using namespace atcoder;
#define rep(i,n) for (int i = 0; i < (n); ++i)
using ll = long long;
using P = pair<int,int>;

int h, w;
bool used[16][16];

// 縦 H メートル、横 W メートルの長方形の部屋があります。
// この部屋に 2 メートル × 1 メートルの区別できない畳 (長方形のタイル) 
// A 枚と、1 メートル × 1 メートルの区別できない半畳 (正方形のタイル) B 枚を敷き詰めます。 
// 2 メートル × 1 メートルの畳は縦長にも横長にも使うことができます。
// 敷き詰める方法は何通りあるでしょうか？
// なお、
// 2A+B=HW であることが保証されます。 また、回転や反転を行うことで初めて一致するような敷き詰め方は区別します。

ll dfs(int i, int j, int a, int b) {
  if (a < 0 || b < 0) return 0;
  if (j == w) j = 0, ++i;
  if (i == h) return 1;
  if (used[i][j]) return dfs(i, j+1, a, b);
  ll res = 0;
  used[i][j] = true;
  res += dfs(i, j+1, a, b-1);
  if (j+1 < w && !used[i][j+1]) {
    used[i][j+1] = true;
    res += dfs(i, j+1, a-1, b);
    used[i][j+1] = false;
  }
  if (i+1 < h && !used[i+1][j]) {
    used[i+1][j] = true;
    res += dfs(i, j+1, a-1, b);
    used[i+1][j] = false;
  }
  used[i][j] = false;
  return res;
}

int main() {
  cin >> h >> w;
  int a, b;
  cin >> a >> b;
  cout << dfs(0,0,a,b) << endl;
  return 0;
}