// 参照URL: https://github.com/annkake/annkake_lib
/**
 * @file lazy_segtree.hpp
 * @brief 遅延評価セグメント木と加算/代入×和/最小値の4構成。
 * @details 構築O(n)、apply/prod/set/getはO(log n)、all_prodはO(1)、空間O(n)。
 *          0始まり・半開区間。集約演算の順序を保つので非可換演算も使用できる。
 * @note SPDX-License-Identifier: CC0-1.0
 * @see ../../docs/algorithms.md 遅延操作の設定例。ACL本体には依存しない。
 * @par 使用例: 1. 区間加算・区間和
 * @code{.cpp}
 * // 葉のsizeは必ず1。LazyAddSum<int>(n)ではsize=0の単位元になるので注意。
 * std::vector<LazyValue<int>> initial(5,{0,1});
 * LazyAddSum<int> seg(initial);
 * seg.apply(1,4,3); // [1,4)に3を加算: {0,3,3,3,0}。
 * int sum = seg.prod(0,5).value; // 9
 * int single = seg.get(2).value; // 3
 * seg.set(2,{10,1}); // 一点代入。区間長1を維持する。
 * (void)sum; (void)single;
 * @endcode
 * @par 使用例: 2. 区間代入・区間和
 * @code{.cpp}
 * std::vector<LazyValue<int>> initial(5,{0,1});
 * LazySetSum<int> seg(initial);
 * seg.apply(0,3,std::optional<int>{5}); // {5,5,5,0,0}。
 * seg.apply(1,2,std::optional<int>{0}); // 0も普通に代入できる。
 * seg.apply(0,5,std::nullopt); // 恒等操作。何もしない。
 * int sum = seg.all_prod().value; // 10
 * (void)sum;
 * @endcode
 * @par 使用例: 3. 区間加算・区間最小値
 * @code{.cpp}
 * std::vector<LazyValue<int>> initial{{5,1},{2,1},{7,1}};
 * LazyAddMin<int> seg(initial);
 * seg.apply(0,2,-3); // {2,-1,7}。
 * int minimum = seg.prod(0,3).value; // -1
 * (void)minimum;
 * // 空区間はsize=0、valueは型Tの最大値。
 * @endcode
 * @par 使用例: 4. 区間代入・区間最小値
 * @code{.cpp}
 * std::vector<LazyValue<int>> initial{{5,1},{2,1},{7,1}};
 * LazySetMin<int> seg(initial);
 * seg.apply(0,2,std::optional<int>{8}); // {8,8,7}。
 * int minimum = seg.prod(0,3).value; // 7
 * (void)minimum;
 * // Composition(f,g)は「古いg→新しいf」。上書きは新しいfを優先する。
 * // 汎用LazySegTreeでは独自のS/Fと各関数も指定できる。
 * @endcode
 */
#pragma once
#include <algorithm>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>
/**
 * @tparam S 集約値の型。Opは結合的、E()はその単位元。
 * @tparam F 遅延操作の型。Id()は恒等操作。
 * @tparam Mapping Mapping(f,x)は集約値xへ操作fを適用する。
 * @tparam Composition Composition(f,g)は古いgの後に新しいfを適用する。
 * @pre MappingはOpと整合すること。演算の中間値・結果が各型で表現可能なこと。
 */
template<class S, auto Op, auto E, class F, auto Mapping, auto Composition, auto Id>
class LazySegTree {
    int count;
    std::vector<S> data;
    std::vector<F> lazy;
    std::vector<bool> pending;
    void build(std::size_t node, int left, int right, const std::vector<S>& values) {
        if (right-left == 1) { data[node] = values[left]; return; }
        int mid = left+(right-left)/2;
        build(node*2,left,mid,values); build(node*2+1,mid,right,values);
        pull(node);
    }
    void pull(std::size_t node) { data[node] = Op(data[node*2],data[node*2+1]); }
    void cover(std::size_t node, const F& f) {
        data[node] = Mapping(f,data[node]);
        lazy[node] = Composition(f,lazy[node]); pending[node] = true;
    }
    void push(std::size_t node) {
        if (!pending[node]) return;
        cover(node*2,lazy[node]); cover(node*2+1,lazy[node]);
        lazy[node] = Id(); pending[node] = false;
    }
    void update(std::size_t node, int left, int right, int from, int to, const F& f) {
        if (to <= left || right <= from) return;
        if (from <= left && right <= to) { cover(node,f); return; }
        push(node); int mid = left+(right-left)/2;
        update(node*2,left,mid,from,to,f); update(node*2+1,mid,right,from,to,f); pull(node);
    }
    S query(std::size_t node, int left, int right, int from, int to) {
        if (to <= left || right <= from) return E();
        if (from <= left && right <= to) return data[node];
        push(node); int mid = left+(right-left)/2;
        return Op(query(node*2,left,mid,from,to),query(node*2+1,mid,right,from,to));
    }
    void replace(std::size_t node, int left, int right, int index, const S& value) {
        if (right-left == 1) {
            data[node] = value; lazy[node] = Id(); pending[node] = false; return;
        }
        push(node); int mid = left+(right-left)/2;
        if (index < mid) replace(node*2,left,mid,index,value);
        else replace(node*2+1,mid,right,index,value);
        pull(node);
    }
    void check(int left, int right) const {
        if (left < 0 || left > right || right > count) throw std::out_of_range("interval");
    }
    static std::vector<S> initial(int n) {
        if (n < 0) throw std::invalid_argument("negative size");
        return std::vector<S>(n,E());
    }
public:
    /// @brief n個のE()で初期化する。区間長が必要な構成では値の配列で初期化すること。
    explicit LazySegTree(int n = 0) : LazySegTree(initial(n)) {}
    explicit LazySegTree(const std::vector<S>& values)
        : count(static_cast<int>(values.size())),
          data(values.empty() ? 1 : values.size()*4,E()),
          lazy(data.size(),Id()), pending(data.size(),false) {
        if (count) build(1,0,count,values);
    }
    void apply(int left, int right, const F& f) {
        check(left,right); if (left != right) update(1,0,count,left,right,f);
    }
    void apply(int index, const F& f) { get_index(index); apply(index,index+1,f); }
    S prod(int left, int right) {
        check(left,right); return left == right ? E() : query(1,0,count,left,right);
    }
    S all_prod() const { return count == 0 ? E() : data[1]; }
    void set(int index, const S& value) { get_index(index); replace(1,0,count,index,value); }
    S get(int index) { get_index(index); return prod(index,index+1); }
    int size() const { return count; }
private:
    void get_index(int index) const {
        if (index < 0 || index >= count) throw std::out_of_range("index");
    }
};

/// @brief 和・最小値と区間長を保持する。葉は{初期値,1}、空区間はsize=0。
template<class T = int>
struct LazyValue { T value; int size; };
namespace lazy_detail {
template<class T> LazyValue<T> sum_op(LazyValue<T> a, LazyValue<T> b) { return {a.value+b.value,a.size+b.size}; }
template<class T> LazyValue<T> sum_e() { return {T{},0}; }
template<class T> LazyValue<T> min_op(LazyValue<T> a, LazyValue<T> b) {
    if (a.size == 0) return b;
    if (b.size == 0) return a;
    return {std::min(a.value,b.value),a.size+b.size};
}
template<class T> LazyValue<T> min_e() { return {std::numeric_limits<T>::max(),0}; }
template<class T> LazyValue<T> add_sum(T f, LazyValue<T> x) { return {x.value+f*x.size,x.size}; }
template<class T> LazyValue<T> add_min(T f, LazyValue<T> x) {
    if (x.size != 0) x.value += f;
    return x;
}
template<class T> T add_compose(T f, T g) { return f+g; }
template<class T> T add_id() { return T{}; }
template<class T> LazyValue<T> set_sum(std::optional<T> f, LazyValue<T> x) {
    if (f && x.size != 0) x.value = *f*x.size;
    return x;
}
template<class T> LazyValue<T> set_min(std::optional<T> f, LazyValue<T> x) {
    if (f && x.size != 0) x.value = *f;
    return x;
}
template<class T> std::optional<T> set_compose(std::optional<T> f, std::optional<T> g) { return f ? f : g; }
template<class T> std::optional<T> set_id() { return std::nullopt; }
} // namespace lazy_detail

template<class T = int>
using LazyAddSum = LazySegTree<LazyValue<T>,lazy_detail::sum_op<T>,lazy_detail::sum_e<T>,
    T,lazy_detail::add_sum<T>,lazy_detail::add_compose<T>,lazy_detail::add_id<T>>;
template<class T = int>
using LazySetSum = LazySegTree<LazyValue<T>,lazy_detail::sum_op<T>,lazy_detail::sum_e<T>,
    std::optional<T>,lazy_detail::set_sum<T>,lazy_detail::set_compose<T>,lazy_detail::set_id<T>>;
template<class T = int>
using LazyAddMin = LazySegTree<LazyValue<T>,lazy_detail::min_op<T>,lazy_detail::min_e<T>,
    T,lazy_detail::add_min<T>,lazy_detail::add_compose<T>,lazy_detail::add_id<T>>;
template<class T = int>
using LazySetMin = LazySegTree<LazyValue<T>,lazy_detail::min_op<T>,lazy_detail::min_e<T>,
    std::optional<T>,lazy_detail::set_min<T>,lazy_detail::set_compose<T>,lazy_detail::set_id<T>>;
