# 桁DPのtransitionとbase


`Transition` はテンプレートの型名、`transition` は渡した関数（ラムダ）です。型は自動推論されるので、`Transition` という型を自分で実装する必要はありません。

```cpp
auto update = [](int& next, const int& previous, int position, int digit) {
    (void)position;
    (void)digit;
    next += previous; // 問題固有の更新式。ここでは個数を合算する。
};
auto dp = digit_dp<int>("101", 0, 1, update, 2);
// 最後の2がbase。上限101(2)=5なので、0を含めて6個。
// dp.back()[0]は上限と一致するもの、dp.back()[1]は上限未満のもの。
```

`next` は `dp[i+1][次の状態]` への参照、`previous` は `dp[i][現在の状態]` への参照です。単なる上書きではなく、最大化なら最大値、個数なら合算など、目的に応じてnextを更新します。到達不能の初期値を使う場合は、previousが到達不能ならreturnする判定も入れてください。関数内の3つの遷移はそのまま利用できます。

`base` は最後の引数で、省略すると10。文字列は2〜36進数に対応し、`0-9` / `A-Z` / `a-z` を受け付けます。36を超える基数では上位桁から並べた `std::vector<int>` を使います。

```cpp
auto dp = digit_dp<int>(std::vector<int>{1,0}, 0, 1, update, 100);
// 100進法の{1,0}=100まで。各桁は0以上base未満。
```

