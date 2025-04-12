/* Randdag: C library for the uniform random generation of DAGs

   Copyright (C) 2020  Martin Pépin, Antoine Genitrini and Alfredo Viola

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <assert.h>
#include <gmp.h>
#include <malloc.h>
#include <string.h> /* memcpy */

#include "../../includes/common.h"
#include "../../includes/ldag.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

static void _fisher_yates(gmp_randstate_t state, int *dest, int n) {
  int i, j, tmp;
  for (i = 0; i < n; i++)
    dest[i] = i;
  for (i = 0; i < n; i++) {
    j = (int)gmp_urandomm_ui(state, n - i);
    tmp = dest[i];
    dest[i] = dest[i + j];
    dest[i + j] = tmp;
  }
}

static void _add_src(gmp_randstate_t state, randdag_vertex *dest,
                     randdag_vertex *other, const int nb_src, int nb_other,
                     int s, const int q) {
  int i;
  randdag_vertex *e;
  randdag_vertex *sources = other - 1;
  randdag_vertex tmp;

  dest->out_degree = s + q;
  dest->out_edges = e = calloc(s + q, sizeof(randdag_vertex));

  for (i = 0; i < q; i++) {
    int j = (int)gmp_urandomm_ui(state, nb_src - i);
    tmp = sources[-i];
    sources[-i] = sources[-i - j];
    sources[-i - j] = tmp;
    e[i] = sources[-i];
  }
  e = e + q;

  while (s) {
    if ((int)gmp_urandomm_ui(state, nb_other) < s) {
      e[0] = other[0];
      e++;
      s--;
    }
    other++;
    nb_other--;
  }
}

static void _ldag_unif(gmp_randstate_t state, const memo_t memo,
                       const int *labels, randdag_vertex *v, int n, int m,
                       int k, int bound) {
  mpz_t rank, factor, factor0, tmp;
  int p, i;
  const int C = min(bound, n - k);

  /* These invariants MUST HOLD, otherwise the behaviour of this function is
   * undefined. They correspond to the conditions under which there is at least
   * one DOAG with parameters (n, m, k, bound). */

  /* Negative parameters are absurd. */
  assert(bound >= 0);
  assert(n >= 0);
  /* At most n sources and at least one except for the empty graph. */
  assert((n > 0) <= k && k <= n);
  /* At least (n-k) edge: one for each internal vertex.
   * At most one C out-edges per source + max(bound,n-k-i) for the i-th internal
   * vertex. */
  assert(n - k <= m && m <= C * (C - 1) / 2 + (n - C) * C);

  /* Base case n=0: the empty graph. */
  if (n == 0)
    return;

  /* Base case n=1: only one vertex. */
  v[0].id = labels[0];
  if (n == 1) {
    v[0].out_degree = 0;
    v[0].out_edges = NULL;
    return;
  }

  mpz_init(rank);
  mpz_init_set_ui(factor, 1);
  mpz_init_set_ui(factor0, 1);
  mpz_init(tmp);

  /* Draw a uniform rank. */
  mpz_init_set(tmp, *ldag_count(memo, n, m, k, bound));
  mpz_mul_ui(tmp, tmp, k);
  mpz_divexact_ui(tmp, tmp, n);
  mpz_urandomm(rank, state, tmp);
  mpz_clear(tmp);

  for (p = 0; p <= min(C, m); p++) {
    /* Loop invariants:
     * - factor = binomial(n - k - p + i, i) * binomial(k - 1 + p - i, p - i)
     * - factor0 = factor(i=0) = binomial(k - 1 + p, p) */

    for (i = 0; i <= min(p - (k == 1), m - n + k); i++) {
      const int C2 = min(n - k - (p - i), bound);

      if (m - p <= (C2 * (C2 - 1)) / 2 + C2 * (n - 1 - C2)) {
        mpz_submul(rank, *ldag_count(memo, n - 1, m - p, k - 1 + p - i, bound),
                   factor);
        if (mpz_sgn(rank) < 0) {
          _ldag_unif(state, memo, labels + 1, v + 1, n - 1, m - p,
                     k - 1 + p - i, bound);
          _add_src(state, v, v + k + p - i, k - 1 + p - i, n - k - p + i, i,
                   p - i);
          mpz_clears(rank, factor, factor0, NULL);
          return;
        }
      }

      /* Update factor for the next iteration. */
      mpz_mul_ui(factor, factor, (n - k - p + i + 1) * (p - i));
      mpz_divexact_ui(factor, factor, (i + 1) * (k - 1 + p - i));
    }

    /* Update factor0 for the next iteration. Reset factor to factor0. */
    mpz_mul_ui(factor0, factor0, k + p);
    mpz_divexact_ui(factor0, factor0, p + 1);
    mpz_set(factor, factor0);
  }

  /* Reaching this point means that there is a bug in the algorithm. */
  assert(0);
}

/* --- Recursive method: uniform DAG with n vertices, m edges, k sources -- */

randdag_t ldag_unif_nmk(gmp_randstate_t state, const memo_t memo, int n, int m,
                        int k, int bound) {
  int *labels;
  randdag_t g = randdag_alloc(n);

  if (bound < 0)
    bound = n;

  /* There should exist at least one DAG with parameters (n,m, k, bound). */
  if (mpz_sgn(*ldag_count(memo, n, m, k, bound)) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DAG with n=%d, m=%d, k=%d, "
            "bound=%d\n",
            n, m, k, bound);
    assert(0);
  }

  labels = calloc(n, sizeof(int));
  _fisher_yates(state, labels, n);
  _ldag_unif(state, memo, labels, g.v, n, m, k, bound);
  return g;
}

/* --- Recursive method: uniform DAG with n vertices and m edges ---------- */

randdag_t ldag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m,
                       int bound) {
  int k;
  mpz_t sum, rank;

  if (bound < 0)
    bound = n;

  /* 1. Count the DAGs with n vertices and m edges. */
  mpz_init(sum);
  for (k = 0; k <= n; k++) {
    mpz_add(sum, sum, *ldag_count(memo, n, m, k, bound));
  }

  /* 2. Sanity check: there should exist a DAG with parameters (n, m). */
  if (mpz_sgn(sum) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DAG with n=%d, m=%d, bound=%d\n",
            n, m, bound);
    assert(0);
  }

  /* 3. Select the number of sources. */
  mpz_init(rank);
  mpz_urandomm(rank, state, sum);
  mpz_clear(sum);

  for (k = 0; k <= n; k++) {
    mpz_sub(rank, rank, *ldag_count(memo, n, m, k, bound));
    if (mpz_sgn(rank) < 0) {
      /* Found, do some cleanups and call the _nmk sampler. */
      mpz_clear(rank);
      /* Note: we call ldag_unif_nmk here rather than _ldag_unif in order to
       * avoid duplicating the label management code. The drawback of this
       * approach is that we do a redundant check in ldag_unif_nmk. */
      return ldag_unif_nmk(state, memo, n, m, k, bound);
    }
  }

  /* Reaching this point means there is a bug in the selection algorithm. */
  assert(0);
}

randdag_t ldag_unif_nk(gmp_randstate_t state, const memo_t memo, int n, int k,
                       int bound) {
  int m, C;
  mpz_t sum, rank;

  if (bound < 0)
    bound = n;
  C = min(n - k, bound);

  /* 1. Count the DAGs with n vertices and k sources. */
  mpz_init(sum);
  for (m = 0; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
    mpz_add(sum, sum, *ldag_count(memo, n, m, k, bound));
  }

  /* 2. Sanity check: there should exist a DAG with parameters (n, k, bound). */
  if (mpz_sgn(sum) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DAG with n=%d, k=%d, bound=%d\n",
            n, k, bound);
    assert(0);
  }

  /* 3. Select the number of edges. */
  mpz_init(rank);
  mpz_urandomm(rank, state, sum);
  mpz_clear(sum);

  for (m = 0; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
    mpz_sub(rank, rank, *ldag_count(memo, n, m, k, bound));
    if (mpz_sgn(rank) < 0) {
      /* Found, do some cleanups and call the _nmk sampler. */
      mpz_clear(rank);
      /* Note: we call ldag_unif_nmk here rather than _ldag_unif in order to
       * avoid duplicating the label management code. The drawback of this
       * approach is that we do a redundant check in ldag_unif_nmk. */
      return ldag_unif_nmk(state, memo, n, m, k, bound);
    }
  }

  /* Reaching this point means there is a bug in the selection algorithm. */
  assert(0);
}

randdag_t ldag_unif_n(gmp_randstate_t state, const memo_t memo, int n,
                      int bound) {
  int m, k;
  mpz_t sum, rank;

  if (bound < 0)
    bound = n;

  /* 1. Count the DAGs with n vertices. */
  mpz_init(sum);
  for (k = 0; k <= n; k++) {
    const int C = min(n - k, bound);
    for (m = 0; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
      mpz_add(sum, sum, *ldag_count(memo, n, m, k, bound));
    }
  }

  /* 2. Sanity check: there should exist a DAG with parameters (n, bound). */
  if (mpz_sgn(sum) <= 0) {
    fprintf(stderr, "Invalid parameters, there is no DAG with n=%d, bound=%d\n",
            n, bound);
    assert(0);
  }

  /* 3. Select the number of edges. */
  mpz_init(rank);
  mpz_urandomm(rank, state, sum);
  mpz_clear(sum);

  for (k = 0; k <= n; k++) {
    const int C = min(n - k, bound);
    for (m = 0; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
      mpz_sub(rank, rank, *ldag_count(memo, n, m, k, bound));
      if (mpz_sgn(rank) < 0) {
        /* Found, do some cleanups and call the _nmk sampler. */
        mpz_clear(rank);
        /* Note: we call ldag_unif_nmk here rather than _ldag_unif in order to
         * avoid duplicating the label management code. The drawback of this
         * approach is that we do a redundant check in ldag_unif_nmk. */
        return ldag_unif_nmk(state, memo, n, m, k, bound);
      }
    }
  }

  /* Reaching this point means there is a bug in the selection algorithm. */
  assert(0);
}
