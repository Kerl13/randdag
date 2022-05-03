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

#include "../../includes/common.h"
#include "../../includes/doag.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

/* TODO: REMOVE ME */
#include <stdio.h>

/* --- Recursive method: main function ------------------------------------ */

/** Auxiliary function: generate on vertex */
static void _add_src(gmp_randstate_t state, randdag_vertex *dest,
                     randdag_vertex *other, int nb_other, int s, int q) {
  int i;
  randdag_vertex *e;
  const randdag_vertex *sources = other - 1;
  /* assert(s + q > 0); */

  dest->out_degree = s + q;
  dest->out_edges = e = calloc(s + q, sizeof(randdag_vertex));

  for (i = 0; i < dest->out_degree; i++) {
    if ((int)gmp_urandomm_ui(state, s + q) < q) {
      *e = *sources;
      sources--;
      q--;
    } else {
      int j = (int)gmp_urandomm_ui(state, nb_other);
      randdag_vertex tmp = other[0];
      other[0] = other[j];
      other[j] = tmp;
      *e = other[0];
      other++;
      nb_other--;
      s--;
    }
    e++;
  }
}

/* Core of the recursive method: uniform DOAG of parameters n, m, k */
static void _unif_doag(gmp_randstate_t state, const memo_t memo,
                       randdag_vertex *v, int n, int m, int k, int bound) {
  mpz_t r, factor;
  int p, s;

  v[0].id = n;

  /* Base case: only one vertex: the sink. */
  if (n == 1) {
    v[0].out_degree = 0;
    v[0].out_edges = NULL;
    return;
  }

  if (n == 2) {
    /* assert(m == 1); */
    /* assert(k == 1); */
    _unif_doag(state, memo, v + 1, 1, 0, 1, bound);
    v[0].out_degree = 1;
    v[0].out_edges = calloc(1, sizeof(randdag_vertex));
    v[0].out_edges[0] = v[1];
    return;
  }

  mpz_init(r);
  mpz_init(factor);
  mpz_urandomm(r, state, *doag_count(memo, n, m, k, bound));

  /* p = q + s */
  for (p = 1; p <= min(n - k, m + 2 - n); p++) {
    const int s_start = (p == n - k);
    mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
    for (s = s_start; s <= p - (k == 1); s++) {
      const int q = p - s;
      const int C2 = min(n - q - k, bound);
      if (m - p <=
          (k - 1 + q) * C2 + C2 * (C2 - 1) / 2 + (n - q - k - C2) * bound) {
        mpz_submul(r, *doag_count(memo, n - 1, m - p, k - 1 + q, bound),
                   factor);
        if (mpz_sgn(r) == -1) {
          _unif_doag(state, memo, v + 1, n - 1, m - p, k - 1 + q, bound);
          _add_src(state, v, v + k + q, n - k - q, s, q);
          mpz_clear(r);
          mpz_clear(factor);
          return;
        }
      }
      mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
      mpz_divexact_ui(factor, factor, s + 1);
    }
  }

  /* Shouldn't reach this point */
  assert(0);
}

/* --- Recursive method: uniform DOAG with n vertex and m edges ----------- */

randdag_t doag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m,
                       int bound) {
  randdag_t g = randdag_alloc(n);
  if (bound <= 0)
    bound = m; /* The maximum out-degree in the unbounded case is m */
  _unif_doag(state, memo, g.v, n, m, 1, bound);
  return g;
}

/* --- Recursive method: uniform DOAG with m edges ------------------------ */

randdag_t doag_unif_m(gmp_randstate_t state, const memo_t memo, int m,
                      int bound) {
  int n;
  mpz_t r, tot;
  mpz_inits(r, tot, NULL);

  if (bound <= 0)
    bound = m; /* The maximum out-degree in the unbounded case is m */

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2)
      mpz_add(tot, tot, *doag_count(memo, n, m, 1, bound));
  }
  mpz_urandomm(r, state, tot);

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2) {
      mpz_sub(r, r, *doag_count(memo, n, m, 1, bound));
      if (mpz_sgn(r) == -1) {
        mpz_clear(r);
        mpz_clear(tot);
        return doag_unif_nm(state, memo, n, m, bound);
      }
    }
  }

  /* Shouldn't reach this point */
  assert(0);
}

/* --- Fast rejection method: uniform DOAG with n vertices ---------------- */

/** Bernoulli random variable of parameter 1/p!.
 * Return 1 with probability 1/p! and 0 with probability 1 - 1/p! */
static int bern_inv_p_fact(gmp_randstate_t state, int p) {
  int k;
  for (k = p; k > 1; k--) {
    if (gmp_urandomm_ui(state, k))
      return 0;
  }
  return 1;
}

/** Poisson random variable of parameter 1 constrained to be < n.
 * Return 0 <= k < n with probability proportional to 1/k! */
static int bounded_poisson(gmp_randstate_t state, int n) {
  /* XXX: Not optimal? */
  int p;
  while (1) {
    p = gmp_urandomm_ui(state, n);
    if (bern_inv_p_fact(state, p))
      return p;
  }
}

/** Simulate the generation of a uniform matrix of variations, but computes just
 * enough information to know it should be rejected or if it corresponds to a
 * valid labelled transition matrix of DOAG. */
static int doag_unif_n_sim(gmp_randstate_t state, randdag_t *g, int n,
                           int *nb_zeros, int *nb_unknown, int *path) {
  int i, j, streak;

  /* Source of the graph */
  nb_zeros[0] = bounded_poisson(state, n - 2);
  g->v[0].out_degree = n - 1 - nb_zeros[0];
  nb_unknown[0] = n - 2;

  path[0] = -1;
  path[1] = 0;

  streak = 1;
  for (j = 2; j < n - 1; j++) {
    i = j - 1;
    nb_zeros[i] = bounded_poisson(state, n - 1 - i);
    nb_unknown[i] = n - i - 1;
    g->v[i].out_degree = n - 1 - i - nb_zeros[i];

    while (i >= 0) {
      if ((int)gmp_urandomm_ui(state, nb_unknown[i]) < nb_zeros[i]) {
        nb_zeros[i] -= 1;
        nb_unknown[i] -= 1;
        i -= 1;
      } else {
        nb_unknown[i] -= 1;
        break;
      }
    }

    /* Reject if the path is not weakly increasing */
    if (path[j - 1] > i)
      goto exit;

    /* Reject if the current streak is not well-ordered */
    if (path[j - 1] < i) {
      if (j - streak > 1) {
        if (!bern_inv_p_fact(state, j - streak))
          goto exit;
      }
      streak = j;
    }

    path[j] = i;
  }

  g->v[n - 2].out_degree = 1;
  nb_zeros[n - 2] = 0;
  nb_unknown[n - 2] = 0;
  path[n - 1] = n - 2;

  nb_zeros[n - 1] = 0;
  nb_unknown[n - 1] = 0;

exit:
  return (j == n - 1);
}

/**
 * Take two adjacent arrays of vertices, perform a uniform permutation of the
 * second array, and perform a uniform shuffling of the first array with the
 * permuted one.
 * The permutation and shuffling algorithms are intertwined so that they can be
 * efficiently implemented in place.
 * - `n` is the cumulated sizes of both arrays.
 * - `k` is the size of the first array. */
static void permut_shuffle(gmp_randstate_t state, randdag_vertex *v, int n,
                           int k) {
  randdag_vertex tmp;
  int r;

  while (n > 0) {
    tmp = v[n - 1];
    r = gmp_urandomm_ui(state, n);
    if (r < k) {
      /* Take the top element of the left array and put it at the end. */
      v[n - 1] = v[k - 1];
      v[k - 1] = tmp;
      k--;
    } else {
      /* Take a uniform element of the right array and put it at the top.
       * The value of `r` can be used as our uniform index since, in this branch
       * of the if, it is a uniform integer from [0; n[ constrained to be >= k
       * i.e. a uniform integer from [k; n[. */
      v[n - 1] = v[r];
      v[r] = tmp;
    }
    n--;
  }
}

/** Complete the generation of a valid partial matrix returned by
 * doag_unif_n_sim. */
static void doag_unif_n_populate(gmp_randstate_t state, randdag_t *g, int n,
                                 int *nb_zeros, int *path) {
  int i, j, p, nb_src;
  randdag_vertex *cur;

  /* The sink */
  g->v[n - 1].out_edges = NULL;

  for (i = n - 2; i >= 0; i--) {
    /* Allocate the row */
    p = nb_zeros[i];
    assert(g->v[i].out_degree > 0);
    cur = calloc(g->v[i].out_degree, sizeof(randdag_vertex));
    g->v[i].out_edges = cur;

    if (i == n - 2)
      assert(p == 0);

    /* Search where row i starts. */
    j = i + 1;
    while (path[j] < i) {
      j++;
    }

    /* Count the number of sources uncovered at the beginning of this row and
     * add pointers to them at the beginning of the current vertex.
     * Their positions will be updated later. */
    nb_src = 0;
    while (path[j] == i) {
      assert(j > i);
      *cur = g->v[j];
      assert(cur->id > i);
      assert(cur->id == j);
      cur++;
      j++;
      nb_src++;
    }

    while (j < n) {
      assert(j > i);
      if (!p || (int)gmp_urandomm_ui(state, n - j) > p) {
        /* Either there is no more zero to insert or the Bern(nb zeroes / nb
         * remaining spots returned false: add a pointer. */
        *cur = g->v[j];
        assert(cur->id > i);
        assert(cur->id == j);
        cur++;
      } else {
        /* Insert a zero in the row, aka skip a vertex. */
        p--;
      }
      j++;
    }
    assert(j == n);

    permut_shuffle(state, g->v[i].out_edges, g->v[i].out_degree, nb_src);
  }
}

randdag_t doag_unif_n(gmp_randstate_t state, int n) {
  int i;
  int *nb_zeros;
  int *nb_unknown;
  int *path;
  randdag_t g = randdag_alloc(n);

  nb_zeros = calloc(n, sizeof(int));
  nb_unknown = calloc(n, sizeof(int));
  path = calloc(n, sizeof(int));

  /* Prepare the graph */
  for (i = 0; i < n; i++) {
    g.v[i].id = i;
  }

  /* Repeat doag_unif_n_sim util it finds a valid transition matrix. */
  while (!doag_unif_n_sim(state, &g, n, nb_zeros, nb_unknown, path)) {
  }
  free(nb_unknown);

  doag_unif_n_populate(state, &g, n, nb_zeros, path);
  free(nb_zeros);
  free(path);

  return g;
}
