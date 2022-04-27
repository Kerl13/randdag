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
 * 3. They have only one sink (i.e. vertex with no out-edges).
 */

/**
 * Return a pointer to a GMP integer storing the number of DOAGs with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a non-positive bound is passed, this function counts DOAGs with unbounded
 * out-degree. */
mpz_t *doag_count(memo_t, int n, int m, int k, int bound);

/**
 * Return a uniform DOAG with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges;
 * - out-degree bounded by `bound`.
 * If a non-positive bound is passed, this function counts DOAGs with unbounded
 * out-degree. */
randdag_t doag_unif_nm(gmp_randstate_t, const memo_t, int n, int m, int bound);

/**
 * Return a uniform DOAG with `m` edges and out-degree bounded by `bound`.
 * If a non-positive bound is passed, this function samples DOAGs with unbounded
 * out-degree. */
randdag_t doag_unif_m(gmp_randstate_t, const memo_t, int m, int bound);

#endif
