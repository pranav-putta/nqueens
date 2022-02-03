#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

typedef std::vector<int> vec;
typedef std::pair<vec, int> pair;

bool is_valid(vec &queens, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (queens[i] == queens[j] || std::abs(queens[i] - queens[j]) == std::abs(i - j)) {
                return false;
            }
        }
    }
    return true;
}

vec initialize_queens(int N) {
    int r = 1;
    vec queens(N);

    for (int i = 0; i < N; i++) {
        queens[i] = r;
        r += 2;
        if (r >= N) {
            r = 0;
        }
    }
    return queens;
}

void row_conflicts(vec &queens, vec &row_conflicts, int N, int col) {
    for (int i = 0; i < N; i++) {
        row_conflicts[i] = 0;
    }
    for (int i = 0; i < N; i++) {
        if (i == col) {
            continue;
        }
        row_conflicts[queens[i]] += 1;

        int rd = queens[i] + std::abs(i - col);
        int ld = queens[i] - std::abs(i - col);

        if (rd >= 0 && rd < N) {
            row_conflicts[rd] += 1;
        }
        if (ld >= 0 && ld < N) {
            row_conflicts[ld] += 1;
        }
    }
}

vec build_conflicts(vec &queens, int N) {
    vec rows(N, 0);
    vec rdiags(2 * N, 0);
    vec ldiags(2 * N, 0);

    for (int i = 0; i < N; i++) {
        int row = queens[i];
        rows[row] += 1;
        rdiags[(N - 1) + row - i] += 1;
        ldiags[(N - 1) + row - (N - i - 1)] += 1;
    }

    vec conflicts(N, 0);
    for (int i = 0; i < N; i++) {
        int row = queens[i];
        conflicts[i] = (rows[row] - 1) + (rdiags[(N - 1) + row - i] - 1) + (ldiags[(N - 1) + row - (N - i - 1)] - 1);
    }

    return conflicts;
}

int random_position_greater_zero(vec &list, int N, double rand) {
    vec possible;

    for (int i = 0; i < N; i++) {
        if (list[i] > 0) {
            possible.push_back(i);
        }
    }

    int rand_i = (int) (rand * possible.size());
    return possible[rand_i];
}

int random_position_equal_to(vec &list, int value, int N, double rand) {
    vec possible;

    for (int i = 0; i < N; i++) {
        if (list[i] == value) {
            possible.push_back(i);
        }
    }

    int rand_i = (int) (rand * possible.size());
    return possible[rand_i];
}


int update_conflicts(vec &queens, vec &conflicts, int N, int col, int new_row) {
    int old_row = queens[col];
    conflicts[col] = 0;

    int total_conflicts = 0;

    for (int i = 0; i < N; i++) {
        if (i == col) {
            continue;
        }
        int row = queens[i];
        if ((old_row == row) || std::abs(old_row - row) == std::abs(col - i)) {
            conflicts[i] -= 1;
        }

        if ((new_row == row) || std::abs(new_row - row) == std::abs(col - i)) {
            conflicts[i] += 1;
            conflicts[col] += 1;
        }
        total_conflicts += conflicts[i];
    }

    return total_conflicts + conflicts[col];;
}

int sum(vec &list, int N) {
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += list[i];
    }
    return sum;
}

int min(vec &list, int N) {
    int min = INT_MAX;
    for (int i = 0; i < N; i++) {
        if (list[i] < min) {
            min = list[i];
        }
    }
    return min;
}

pair iterative_repair(int N) {
    const int MAX_STEPS = 1000000;

    vec queens = initialize_queens(N);
    vec conflicts = build_conflicts(queens, N);
    vec row_confs(N, 0);

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);
    int total_conflicts = sum(conflicts, N);

    for (int i = 0; i < MAX_STEPS; i++) {
        if (total_conflicts == 0) {
            return pair(queens, i);
        }

        int col = random_position_greater_zero(conflicts, N, dist(e2));
        row_conflicts(queens, row_confs, N, col);
        int min_conflict = min(row_confs, N);
        int new_row = random_position_equal_to(row_confs, min_conflict, N, dist(e2));
        
        total_conflicts = update_conflicts(queens, conflicts, N, col, new_row);
        queens[col] = new_row;
    }

    return pair(queens, MAX_STEPS);
}

int user_input_N() {
    int N;
    std::cout << "N: ";
    std::cin >> N;
    return N;
}

int main() {
    int N = 0;

    while (N != -1) {
        N = user_input_N();
        auto start = std::chrono::high_resolution_clock::now();
        auto result = iterative_repair(N);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);


        std::cout << N << "\t" << result.second << "\t" << "took " << duration.count() << "s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        bool valid = is_valid(result.first, N);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

        char *s = const_cast<char *>(valid ? "valid" : "invalid");
        std::cout << "and " << s << "! in " << duration.count() << "s" << std::endl;
    }
    return 0;
}
