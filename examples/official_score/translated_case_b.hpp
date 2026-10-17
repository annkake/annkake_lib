// User-supplied translation of official scoring tools (case B).
// Source supplied by owner: https://atcoder.jp/contests/ahc071/tasks/ahc071_a
// Case B matches local AHC070 Rust tools on 30 valid/invalid test cases.
// The supplied AHC071 URL has a different input format; upstream license remains unverified.
// This file is NOT covered by the repository's CC0 dedication.
// Scoring logic is preserved; non-template header functions are marked inline.
#pragma once
#include <bits/stdc++.h>
namespace translated_b {
using namespace std;




using i64 = int64_t;
using u64 = uint64_t;
using usize = size_t;

template <class T>
bool setmin(T& a, const T& b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

template <class T>
bool setmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

struct Input {
    usize N, M;
    vector<pair<usize, usize>> targets;
};

inline ostream& operator<<(ostream& os, const Input& input) {
    os << input.N << ' ' << input.M << '\n';
    for (auto [x, y] : input.targets) {
        os << x << ' ' << y << '\n';
    }
    return os;
}

inline Input parse_input(const string& s) {
    istringstream is(s);
    Input input;

    if (!(is >> input.N >> input.M)) {
        throw runtime_error("Invalid input");
    }

    input.targets.resize(input.N * input.N);
    for (auto& [x, y] : input.targets) {
        if (!(is >> x >> y)) {
            throw runtime_error("Invalid input");
        }
    }

    return input;
}

template <class T>
variant<T, string> read(
    optional<string> token,
    T lo,
    T hi,
    bool inclusive_hi = false
) {
    if (!token) {
        return string("Unexpected EOF");
    }

    T value{};
    istringstream is(*token);
    is >> noskipws >> value;

    if (!is || is.peek() != char_traits<char>::eof()) {
        return string("Parse error: ") + *token;
    }

    if (value < lo || (inclusive_hi ? value > hi : value >= hi)) {
        return string("Out of range: ") + to_string(value);
    }

    return value;
}

struct Output {
    vector<pair<int, int>> m;
    vector<usize> actions;
};

using OutputResult = variant<Output, string>;

inline OutputResult parse_output(const Input& input, const string& s) {
    istringstream is(s);

    auto next = [&]() -> optional<string> {
        string token;
        if (is >> token) return token;
        return nullopt;
    };

    Output out;

    for (int i = 0; i < 3; ++i) {
        auto x = read<int>(next(), 0, static_cast<int>(input.N));
        if (holds_alternative<string>(x)) {
            return get<string>(x);
        }

        auto y = read<int>(next(), 0, static_cast<int>(input.N));
        if (holds_alternative<string>(y)) {
            return get<string>(y);
        }

        out.m.emplace_back(get<int>(x), get<int>(y));
    }

    for (usize i = 0; i < input.N * input.N; ++i) {
        auto action = read<usize>(next(), 0, input.M);
        if (holds_alternative<string>(action)) {
            return get<string>(action);
        }
        out.actions.push_back(get<usize>(action));
    }

    if (auto extra = next()) {
        return string("Unexpected token: ") + *extra;
    }

    return out;
}

struct Sim {
    pair<usize, usize> prev_pos{0, 0};
    pair<usize, usize> cur_pos{0, 0};
    pair<usize, usize> cur_target{0, 0};

    vector<vector<optional<usize>>> visited_at;
    usize unique_count = 0;
    vector<i64> stepwise_costs;

    explicit Sim(const Input& input)
        : visited_at(
            input.N,
            vector<optional<usize>>(input.N, nullopt)
        ) {}

    i64 total_cost(usize t) const {
        if (t > stepwise_costs.size()) {
            throw out_of_range("Invalid turn");
        }

        return accumulate(
            stepwise_costs.begin(),
            stepwise_costs.begin() + t,
            i64(0)
        );
    }
};

using ScoreDetails = tuple<i64, i64, string, Sim>;

inline ScoreDetails compute_score_details(
    const Input& input,
    const Output& out,
    usize t
) {
    usize T = input.N * input.N;

    if (out.actions.size() != T) {
        return {
            0,
            0,
            "Invalid action length: " + to_string(out.actions.size()) +
                " (expected " + to_string(T) + ")",
            Sim(input)
        };
    }

    vector<pair<usize, usize>> black_cells;
    Sim sim(input);
    optional<Sim> sim_at_t;

    if (t == 0) {
        sim_at_t = sim;
    }

    for (usize step = 0; step < T; ++step) {
        usize action = out.actions[step];

        if (action >= out.m.size()) {
            return {
                0,
                0,
                "Invalid action index " + to_string(action) +
                    " at turn " + to_string(step),
                Sim(input)
            };
        }

        auto [dx, dy] = out.m[action];
        auto [x, y] = sim.cur_pos;

        auto mod = [](i64 a, i64 n) -> usize {
            return static_cast<usize>((a % n + n) % n);
        };

        usize x1 = mod(static_cast<i64>(x) + dx, input.N);
        usize y1 = mod(static_cast<i64>(y) + dy, input.N);

        if (!sim.visited_at[x1][y1]) {
            sim.visited_at[x1][y1] = step;
            black_cells.emplace_back(x1, y1);
            ++sim.unique_count;
        }

        sim.prev_pos = sim.cur_pos;
        sim.cur_pos = {x1, y1};

        auto [a, b] = input.targets.at(step);

        usize min_dist = numeric_limits<usize>::max();

        for (auto [xx, yy] : black_cells) {
            usize dist =
                (xx > a ? xx - a : a - xx) +
                (yy > b ? yy - b : b - yy);

            min_dist = min(min_dist, dist);
        }

        i64 step_cost = static_cast<i64>(
            static_cast<double>(min_dist) *
            sqrt(static_cast<double>(step + 1))
        );

        sim.stepwise_costs.push_back(step_cost);

        if (step + 1 == t) {
            sim_at_t = sim;
            sim_at_t->cur_target = input.targets[step];
        }
    }

    i64 cost = sim.total_cost(T);

    i64 score = static_cast<i64>(
        round(
            1e6 * pow(static_cast<double>(input.N), 3.0) /
            (1.0 + static_cast<double>(cost))
        )
    );

    if (!sim_at_t) {
        throw out_of_range("Invalid visualization turn");
    }

    return {cost, score, "", move(*sim_at_t)};
}

inline pair<i64, string> compute_score(
    const Input& input,
    const Output& out
) {
    auto [cost, score, err, sim] =
        compute_score_details(input, out, input.N * input.N);

    if (!err.empty()) {
        score = 0;
    }

    return {score, err};
}


} // namespace translated_b
