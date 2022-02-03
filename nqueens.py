import random


def is_valid(assignment):
    for col_i, row_i in enumerate(assignment):
        for col_j, row_j in enumerate(assignment[col_i + 1:], col_i + 1):
            if is_conflict(row_i, col_i, row_j, col_j):
                return False
    return True


def is_conflict(row1, col1, row2, col2):
    return (row1 == row2) or (abs(row1 - row2) == abs(col1 - col2))


def initialize_queens(N):
    r = 1
    queens = []
    for i in range(N):
        queens.append(r)
        r += 2
        if r >= N:
            r = 0
    return queens


def row_conflicts(queens: list, N, col):
    row_confs = [0 for _ in range(N)]
    for col_q, row_q in enumerate(queens):
        if col_q == col:
            continue
        row_confs[row_q] += 1  # add conflict for matching row

        if 0 <= row_q + abs(col_q - col) < N:
            row_confs[row_q + abs(col_q - col)] += 1
        if 0 <= row_q - abs(col_q - col) < N:
            row_confs[row_q - abs(col_q - col)] += 1
    return row_confs


def build_conflicts(queens: list):
    rows = {}
    rdiags = {}
    ldiags = {}
    N = len(queens)
    for col, row in enumerate(queens):
        rows[row] = rows.get(row, -1) + 1
        rdiags[row - col] = rdiags.get(row - col, -1) + 1
        ldiags[row - (N - col)] = ldiags.get(row - (N - col), -1) + 1
    conflicts = []
    for col, row in enumerate(queens):
        conflicts.append(rows.get(row, 0) + rdiags.get(row - col, 0) + ldiags.get(row - (N - col), 0))
    return conflicts


def update_conflicts(queens: list, conflicts: list, col, new_row):
    conflicts[col] = 0
    old_row = queens[col]

    for col2, row2 in enumerate(queens):
        if col == col2:
            continue
        # remove all conflicts with old_row
        if (old_row == row2) or (abs(old_row - row2) == abs(col - col2)):
            conflicts[col2] -= 1
        # add all conflicts with new_row
        if (new_row == row2) or (abs(new_row - row2) == abs(col - col2)):
            conflicts[col2] += 1
            conflicts[col] += 1


def pick_position(arr, N, cond):
    return random.choice([i for i in range(N) if cond(arr[i])])


def min_conflict(N, max_steps=10 ** 6):
    queens = initialize_queens(N)
    conflicts = build_conflicts(queens)  # O(n)
    for i in range(max_steps):  # O(N)

        if sum(conflicts) == 0:
            return queens, i

        col = pick_position(conflicts, N, lambda x: x > 0)  # O(N)
        row_confs = row_conflicts(queens, N, col)  # O(N)

        min_conf = min(row_confs)  # O(N)
        new_row = pick_position(row_confs, N, lambda x: x == min_conf)  # O(N)
        # update conflicts
        update_conflicts(queens, conflicts, col, new_row)  # O(N)
        queens[col] = new_row
    return queens, max_steps
