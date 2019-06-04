#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <iterator>
#include <chrono>
#include <float.h>

struct coord {
    int i, j;
};

double dist(coord v1, coord v2) {
    return sqrt(pow(v1.i - v2.i, 2) + pow(v1.j - v2.j, 2));
}

double dist(std::vector<int> &state, std::vector<std::vector<double>> &edges) {
    double res = 0;
    for (size_t i = 0; i < state.size() - 1; ++i) {
        res += edges[state[i]][state[i + 1]];
    }
    return res;
}

std::vector<int> get_candidate(std::vector<int> state, std::vector<std::vector<double>> &edges, double cur_dist, double &candidate_dist, std::mt19937 &rng) {
    int left = rng() % (state.size() - 1) + 1;
    int right = rng() % (state.size() - 1) + 1;
    if (right < left) {
        std::swap(left, right);
    }
    candidate_dist = cur_dist - edges[state[left - 1]][state[left]] - edges[state[right - 1]][state[right]];
    std::reverse(state.begin() + left, state.begin() + right);
    candidate_dist += edges[state[left - 1]][state[left]] + edges[state[right - 1]][state[right]];
    return state;
}

double decrease_temp(double initial_temp, int i) {
    return initial_temp / pow(i + 1, 0.66);
    // return initial_temp / i;
}

std::vector<int> get_greedy(std::vector<std::vector<double>> &edges) {
    std::vector<int> state(edges.size() + 1);
    state[0] = state[state.size() - 1] = 0;
    std::vector<int> visited(edges.size(), 0);
    visited[0] = 1;
    for (size_t i = 1; i < edges.size(); ++i) {
        size_t neighboor;
        double value = DBL_MAX;
        for (size_t j = 1; j < edges.size(); ++j) {
            if (visited[j] == 0 && edges[state[i - 1]][j] < value) {
                neighboor = j;
                value = edges[state[i - 1]][j];

            }
        }
        visited[neighboor] = 1;
        state[i] = neighboor;
    }
    return state;
}

std::vector<int> simulated_annealing(std::vector<std::vector<double>> &edges, double initial_temp, double end_temp) {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::vector<int> state = get_greedy(edges);
    // for (size_t i = 0; i < state.size() - 1; ++i) {
    //     state[i] = i;
    // }
    // std::cout << dist(state, edges) << '\n';
    // state[state.size() - 1] = 0;
    std::vector<int> min_state;
    double min_dist = DBL_MAX;
    for (size_t j = 0; j < 1; ++j) {
        // std::shuffle(state.begin() + 1, state.end() - 1, rng);
        double cur_dist = dist(state, edges);
        if (cur_dist < min_dist) {
            min_dist = cur_dist;
            min_state = state;
        }
        double cur_temp = initial_temp;
        int i = 1;
        while (end_temp < cur_temp) {
            double candidate_dist;
            std::vector<int> state_candidate = get_candidate(state, edges, cur_dist, candidate_dist, rng);
            if (candidate_dist < cur_dist) {
                cur_dist = candidate_dist;
                state = state_candidate;
            } else {
                double p = exp((cur_dist - candidate_dist) / cur_temp) * 100000;
                double value = rng() % 100000;
                // std::cout << p << ' ' << value << '\n';
                if (value <= p) {
                    cur_dist = candidate_dist;
                    state = state_candidate;
                }
            }
            if (candidate_dist < min_dist) {
                min_dist = candidate_dist;
                min_state = state_candidate;
            }
            cur_temp = decrease_temp(initial_temp, i);
            ++i;
        }
    }
   // std::cout << min_dist << '\n';
    return min_state;
}

int main() {
    size_t n_villages;
    std::cin >> n_villages;
    std::vector<coord> villages(n_villages);
    for (size_t i = 0; i < villages.size(); ++i) {

        std::cin >> villages[i].i >> villages[i].j;
    }
    std::vector<std::vector<double>> edges(villages.size(), std::vector<double>(villages.size()));
    for (size_t i = 0; i < edges.size(); ++i) {
        for (size_t j = 0; j < edges[i].size(); ++j) {
            if (i == j) {
                edges[i][j] = DBL_MAX;
            } else {
                edges[i][j] = edges[j][i] = dist(villages[i], villages[j]);
            }
        }
    }
    std::vector<int> res;
    if (n_villages > 1000) {
        res = simulated_annealing(edges, 1, 0.0005);
    } else if (n_villages > 500) {
        res = simulated_annealing(edges, 300, 0.0110);
    } else if (n_villages > 200) {
        res = simulated_annealing(edges, 300, 0.01);
    } else if (n_villages > 70) {
        res = simulated_annealing(edges, 300, 0.006);
    } else {
        res = simulated_annealing(edges, 300, 0.01);
    }
    for (size_t i = 0; i < res.size(); ++i) {
        std::cout << res[i] + 1 << ' ';
    }
    std::cout << '\n';
}

