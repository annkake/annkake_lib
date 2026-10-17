/**
 * @file official_bridge.hpp
 * @brief 公式評価の合法性とスコアを共通の最良解管理へ接続する。
 * @details heuristic/core.hppのGoalとBestOutputを利用する。評価は完成したOutputに対して呼ぶ。
 * @note 提供されたahc_patch_v4の接続部を統合。ライセンス条件はprovenance記録を参照。
 * @par 使用例（AHC共通部と一緒に使用）
 * @code{.cpp}
 * struct Output { int value; };
 * ahc::OfficialRecorder<Output, int> recorder(ahc::Goal::Maximize,
 *     [](const Output& out) {
 *         if (out.value < 0) return ahc::ScoreResult<int>::invalid("negative value");
 *         return ahc::ScoreResult<int>::valid(out.value); // 0点でも合法。
 *     });
 * recorder.consider(Output{0});
 * recorder.consider(Output{10});
 * int best = recorder.best.output->value; // 10。
 * @endcode
 */
#pragma once
#include <bits/stdc++.h>
#include "core.hpp"
namespace ahc {
template<class Score> struct ScoreResult {
    std::optional<Score> score;
    std::string error;
    /// @brief scoreの有無で合法性を判定する。0点は無効解ではない。
    explicit operator bool() const { return score.has_value(); }
    static ScoreResult valid(Score x) { return {x,{}}; }
    static ScoreResult invalid(std::string e) { return {std::nullopt,std::move(e)}; }
};
// スコアを計算するのは解が完成したときだけ。
// 評価不能な解はBestOutputへ渡さない。
template<class Output,class Score=long long>
class OfficialRecorder {
    Goal goal_;
    std::function<ScoreResult<Score>(const Output&)> score_;
public:
    BestOutput<Output,Score> best;
    explicit OfficialRecorder(Goal goal,std::function<ScoreResult<Score>(const Output&)> f)
        :goal_(goal),score_(std::move(f)){}
    /// @return 合法かつ最良解を更新したときtrue。無効解や同点・悪化はfalse。
    /// @note 評価関数が投げた例外は呼び出し元へ伝播する。
    bool consider(const Output& out,bool verbose=false) {
        auto r=score_(out);
        if(!r){if(verbose)std::cerr<<"[公式評価] 無効解: "<<r.error<<'\n';return false;}
        bool updated=best.consider(*r.score,goal_,[&]{return out;});
        if(updated && verbose)std::cerr<<"[公式評価] 最良更新: "<<*r.score<<'\n';
        return updated;
    }
};
}
