#ifndef _LDAG_H
#define _LDAG_H

#include "common.h"
#include <gmp.h>
#include <stdio.h>

/** \file ldag.h
 * Functions for counting and sampling labelled dags (LDAGs) with an optional
 * bound on the out-degree.
 *
 * Labelled DAGS are directed graphs on the integer vertices [1; n] with the
 * following extra structural property:
 * 1. [Acyclicity] They contain no cycle, that is to say there is no path of
 *    edges from a vertex to itself.
 */

/**
 * Return a pointer to a GMP integer storing the number of labelled DAGs with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound` (if a negative bound is passed, this function
 *   counts labelled DAGs with unbounded out-degree).
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing the result of this function, i.e. its `N`, `M`, and
 * `bound` fields must be respectively larger or equal to `n`, `m`, and `bound`.
 */
mpz_t *ldag_count(memo_t, int n, int m, int k, int bound);

/**
 * Return a uniform labelled DAG with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a negative bound is passed, this function samples DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `C * (C - 1) / 2 + C * (n - C)` where
 *   `C = min(bound, n - k)`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t ldag_unif_nmk(gmp_randstate_t, memo_t, int n, int m, int k,
                        int bound);

/**
 * Return a uniform labelled DAG with:
 * - `n` vertices;
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a negative bound is passed, this function samples DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `C * (C - 1) / 2 + C * (n - C)` where
 *   `C = min(bound, n - k)`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t ldag_unif_nm(gmp_randstate_t, memo_t, int n, int m, int bound);

/**
 * Return a uniform labelled DAG with:
 * - `n` vertices,
 * - including `k` sources;
 * - out-degree bounded by `bound`.
 * If a negative bound is passed, this function samples DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `C * (C - 1) / 2 + C * (n - C)` where
 *   `C = min(bound, n - k)`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t ldag_unif_nk(gmp_randstate_t, memo_t, int n, int k, int bound);

/**
 * Return a uniform labelled DAG with:
 * - `n` vertices;
 * - out-degree bounded by `bound`.
 * If a negative bound is passed, this function samples DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `C * (C - 1) / 2 + C * (n - C)` where
 *   `C = min(bound, n - 1)`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t ldag_unif_n(gmp_randstate_t, memo_t, int m, int bound);

#endif
