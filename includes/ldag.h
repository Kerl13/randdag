#ifndef _LDAG_H
#define _LDAG_H

#include "common.h"
#include <gmp.h>
#include <stdio.h>

/** \file ldag.h
 * Functions for counting and sampling labelled dags with a single sink (LDAGs).
 *
 * LDAGs are directed graphs where the vertices carry distinct integer labels
 * from the interval [1; n] where n is the number of vertices.
 * In addition, we have the following restrictions:
 * 1. [Acyclic] They contain no cycle, that is to say there is no path of edges
 *    from a vertex to itself.
 * 2. They have only one sink (i.e. vertex with no out-edges).
 */

/**
 * Return a pointer to a GMP integer storing the number of LDAGs with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges. */
mpz_t *ldag_count(memo_t, int n, int m, int k);

/**
 * Return a uniform LDAG with:
 * - `n` vertices (including exactly `k` sources);
 * - `m` edges. */
randdag_t ldag_unif_nm(gmp_randstate_t, memo_t, int n, int m);

/**
 * Return a uniform LDAG with `m` edges. */
randdag_t ldag_unif_m(gmp_randstate_t, memo_t, int m);

#endif
