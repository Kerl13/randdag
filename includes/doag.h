#ifndef _DOAG_H
#define _DOAG_H

#include "common.h"
#include <gmp.h>
#include <stdio.h>

/** \file doag.h
 * Functions for counting and sampling Directed Ordered Acyclic Graphs (DOAGs),
 * with an optional bound on the out-degree.
 *
 * DOAGs are directed graphs with the following extra structure:
 * 1. [Acyclic] They contain no cycle, that is to say there is no path of edges
 *    from a vertex to itself.
 * 2. [Ordered] The out-edges of each vertex are ordered. In other words, the
 *    ordering of the out-edges in the data-structure is not an implementation
 *    artifact: two graphs with different ordering of the out-edges are
 *    considered different.
 */

/**
 * Return a pointer to a GMP integer storing the number of DOAGs with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound` (if a negative bound is passed, this function
 *   counts DOAGs with unbounded out-degree).
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing the result of this function, i.e. its `N`, `M`, and
 * `bound` fields must be respectively larger or equal to `n`, `m`, and `bound`.
 */
mpz_t *doag_count(memo_t, int n, int m, int k, int bound);

/**
 * Return a uniform DOAG with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a non-positive bound is passed, this function counts DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `m`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t doag_unif_nmk(gmp_randstate_t, const memo_t, int n, int m, int k,
                        int bound);

/**
 * Return a uniform DOAG with:
 * - `n` vertices;
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a non-positive bound is passed, this function counts DOAGs with unbounded
 * out-degree.
 * The `memo` argument is a memoisation structure (\ref memo_t) and it must have
 * enough space for storing all the precomputation, that is:
 * - `memo.N` must be at least `n`;
 * - `memo.M` must be at least `m`;
 * - `memo.bound` must be at least `bound` if `bound >= 0` or `-1` otherwise.
 */
randdag_t doag_unif_nm(gmp_randstate_t, const memo_t, int n, int m, int bound);

/**
 * Return a uniform DOAG with:
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
randdag_t doag_unif_nk(gmp_randstate_t, const memo_t, int n, int k, int bound);

/**
 * Return a uniform DOAG with `n` edges.
 * This function uses a different algorithm from the other random sampling
 * functions and requires no counting information. */
randdag_t doag_unif_n(gmp_randstate_t, int n);

#endif
