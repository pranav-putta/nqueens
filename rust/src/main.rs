use std::io;
use std::io::*;
use std::time::Instant;
use rand::Rng;
use indicatif::{ProgressIterator, ProgressStyle};
use indicatif::ProgressBar;

fn is_valid(queens: &Vec<usize>) -> bool {
    for (col, row) in queens.iter().enumerate() {
        for (col2, row2) in queens.iter().enumerate() {
            if col == col2 {
                continue;
            }
            if (*row == *row2) || ((*row as isize - *row2 as isize).abs() == (col as isize - col2 as isize).abs()) {
                return false;
            }
        }
    }
    return true;
}

fn initialize_queens(n: usize) -> Vec<usize> {
    let mut queens: Vec<usize> = vec![0; n];

    let mut r = 1;
    for i in 0..n {
        queens[i] = r;
        r += 2;
        if r >= n {
            r = 0
        }
    }

    queens
}

fn build_conflicts(queens: &Vec<usize>, n: usize) -> Vec<usize> {
    let mut rows = vec![0; n];
    let mut rdiags = vec![0; 2 * n];
    let mut ldiags = vec![0; 2 * n];

    for (col, row) in queens.iter().enumerate() {
        rows[*row] += 1;
        rdiags[(n - 1) + *row - col] += 1;
        ldiags[(n - 1) + *row - (n - col - 1)] += 1;
    }
    let mut conflicts = vec![0; n];
    for (col, row) in queens.iter().enumerate() {
        conflicts[col] = (rows[*row] - 1) + (rdiags[(n - 1) + *row - col] - 1) + (ldiags[(n - 1) + *row - (n - col - 1)] - 1);
    }

    conflicts
}

fn random_position<F: Fn(usize) -> bool>(list: &Vec<usize>, filter: F) -> usize {
    let mut possible = vec![];
    for i in 0..list.len() {
        if filter(list[i]) {
            possible.push(i);
        }
    }
    let rand_i = rand::thread_rng().gen_range(0..possible.len());
    return possible[rand_i];
}

fn row_conflicts(queens: &Vec<usize>, n: usize, col: usize) -> Vec<usize> {
    let mut row_conflicts: Vec<usize> = vec![0; n];
    for (col_q, row_q) in queens.iter().enumerate() {
        if col_q == col as usize {
            continue;
        }
        row_conflicts[*row_q] += 1;
        let ld = *row_q as isize + (col_q as isize - col as isize).abs();
        let rd = *row_q as isize - (col_q as isize - col as isize).abs();
        if ld >= 0 && (ld as usize) < n {
            row_conflicts[ld as usize] += 1;
        }
        if rd >= 0 && (rd as usize) < n {
            row_conflicts[rd as usize] += 1;
        }
    }

    return row_conflicts;
}

fn update_conflicts(queens: &Vec<usize>, conflicts: &mut Vec<usize>, col: usize, new_row: usize) {
    let mut count = 0;
    let old_row = queens[col];

    for (col2, row2) in queens.iter().enumerate() {
        if col == col2 {
            continue;
        }

        if (old_row == *row2) || ((old_row as isize - *row2 as isize).abs() == (col as isize - col2 as isize).abs()) {
            conflicts[col2] -= 1;
        }
        if (new_row == *row2) || ((new_row as isize - *row2 as isize).abs() == (col as isize - col2 as isize).abs()) {
            conflicts[col2] += 1;
            count += 1;
        }
    }
    conflicts[col] = count;
}

fn iterative_repair(n: usize) -> (Vec<usize>, u64) {
    const MAX_STEPS: u64 = 100000;
    let mut bar = ProgressBar::new(MAX_STEPS);
    bar.set_style(ProgressStyle::default_bar()
        .template("[{elapsed_precise}] {bar:40.cyan/blue} {pos:>7}/{len:7} {msg}"));
    let mut queens = initialize_queens(n);
    let mut conflicts = build_conflicts(&queens, n);
    let mut rng = rand::thread_rng();

    for i in 0..MAX_STEPS {
        let T = f64::exp(-100.0 * i as f64 / n as f64);
        let num_conflicts: usize = conflicts.iter().sum();
        if num_conflicts == 0 {
            bar.finish();
            return (queens, i);
        }
        let mut r = rng.gen::<f64>();
        let col;
        if r < T {
            col = rng.gen_range(0..n);
        } else {
            col = random_position(&conflicts, |x| { x > 0 });
        }

        let row_conflicts = row_conflicts(&queens, n, col);
        let new_row;
        r = rng.gen::<f64>();
        if r < T {
            new_row = rng.gen_range(0..n);
        } else {
            let min_conflict = *row_conflicts.iter().min().unwrap();
            new_row = random_position(&row_conflicts, |x| { x == min_conflict });
        }

        update_conflicts(&queens, &mut conflicts, col, new_row);
        queens[col] = new_row;
        bar.inc(1);
    }
    bar.finish();
    return (queens, MAX_STEPS);
}

fn test() -> Vec<i32> {
    let t = vec![0; 100000000];
    let mut a = vec![0; 100000000];
    for i in 0..t.len() {
        a[i] += t[i];
    }
    return a;
}

fn main() {
    /*
    let mut s = String::new();
    io::stdin().read_line(&mut s).expect("error: unable to read user input");
    let n = s.trim().parse::<i32>().unwrap() as usize;
    let now = Instant::now();
    let n = 10004;
    let (queens, steps) = iterative_repair(n);
    print!("{} {} {}s", n, steps, now.elapsed().as_secs());*/
    test();
}