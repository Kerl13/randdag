#ifndef _LDAG_H
#define _LDAG_H

#include "common.h"
#include <gmp.h>
#include <stdio.h>

/** \file ldag.h
 * Functions for counting and sampling labelled dags (LDAGs) with an optional
 * bound on the out-degree.
 *
 * LDAGs are directed graphs on the integer vertices [1; n] with the following
 * extra structural property:
 * 1. [Acyclic] They contain no cycle, that is to say there is no path of edges
 *    from a vertex to itself.
 */

/**
 * Return a pointer to a GMP integer storing the number of LDAGs with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound` (if a negative bound is passed, this function
 *   counts LDAGs with unbounded out-degree).
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing the result of this function, i.e. its `N`, `M`, and
 * `bound` fields must be respectively larger or equal to `n`, `m`, and `bound`.
 */
mpz_t *ldag_count(memo_t, int n, int m, int k, int bound);

/**
 * Return a uniform LDAG with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing the result of this function, i.e. its `N` and `M`
 * fields must be respectively larger or equal to `n` and `m`. */
randdag_t ldag_unif_nm(gmp_randstate_t, memo_t, int n, int m);

/**
 * Return a uniform LDAG with `m` edges.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing the result of this function, i.e. its `N` and `M`
 * fields must be respectively larger or equal to `m+1` and `m`. */
randdag_t ldag_unif_m(gmp_randstate_t, memo_t, int m);

#endif
