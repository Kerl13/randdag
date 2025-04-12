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
/* FIXME: this function wastes a lot of random bits, we should draw the rank
 * only once and do unranking. */
static void _doag_unif(gmp_randstate_t state, const memo_t memo,
                       randdag_vertex *v, int n, int m, int k, int bound) {
  mpz_t rank, factor;
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
  v[0].id = n;
  if (n == 1) {
    v[0].out_degree = 0;
    v[0].out_edges = NULL;
    return;
  }

  /* Draw a uniform rank. */
  mpz_init(rank);
  mpz_init(factor);
  mpz_urandomm(rank, state, *doag_count(memo, n, m, k, bound));

  for (p = 0; p <= min(C, m); p++) {
    mpz_set_ui(factor, 1);
    for (i = 0; i <= min(p - (k == 1), m - n + k); i++) {
      const int C2 = min(n - k - (p - i), bound);
      if (m - p <= (C2 * (C2 - 1)) / 2 + C2 * (n - 1 - C2)) {
        mpz_submul(rank, *doag_count(memo, n - 1, m - p, k - 1 + p - i, bound),
                   factor);
        if (mpz_sgn(rank) < 0) {
          _doag_unif(state, memo, v + 1, n - 1, m - p, k - 1 + p - i, bound);
          _add_src(state, v, v + k + p - i, n - k - p + i, i, p - i);
          mpz_clear(rank);
          mpz_clear(factor);
          return;
        }
      }
      mpz_mul_ui(factor, factor, (n - k - p + i + 1) * (p - i));
      mpz_divexact_ui(factor, factor, i + 1);
    }
  }

  /* Reaching this point means that there is a bug in the algorithm. */
  assert(0);
}

/* --- Recursive method: uniform DOAG with n vertices, m edges, k sources - */

randdag_t doag_unif_nmk(gmp_randstate_t state, const memo_t memo, int n, int m,
                        int k, int bound) {
  randdag_t g = randdag_alloc(n);
  if (bound < 0)
    bound = n;

  /* There should exist at least one DOAG with parameters (n,m, k, bound). */
  if (mpz_sgn(*doag_count(memo, n, m, k, bound)) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DOAG with n=%d, m=%d, k=%d, "
            "bound=%d\n",
            n, m, k, bound);
    assert(0);
  }

  _doag_unif(state, memo, g.v, n, m, k, bound);
  return g;
}

/* --- Recursive method: uniform DOAG with n vertices and m edges --------- */

randdag_t doag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m,
                       int bound) {
  int k;
  mpz_t sum, rank;
  randdag_t g = randdag_alloc(n);

  if (bound < 0)
    bound = n;

  /* 1. Count the DOAGs with n vertices and m edges. */
  mpz_init(sum);
  for (k = 0; k <= n; k++) {
    mpz_add(sum, sum, *doag_count(memo, n, m, k, bound));
  }

  /* 2. Sanity check: there should exist a DOAG with parameters (n, m). */
  if (mpz_sgn(sum) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DOAG with n=%d, m=%d, bound=%d\n",
            n, m, bound);
    assert(0);
  }

  /* 3. Select the number of sources. */
  mpz_init(rank);
  mpz_urandomm(rank, state, sum);
  mpz_clear(sum);
  for (k = 0; k <= n; k++) {
    mpz_sub(rank, rank, *doag_count(memo, n, m, k, bound));
    if (mpz_sgn(rank) < 0) {
      /* Found, do some cleanups and call the main sampler. */
      mpz_clear(rank);
      _doag_unif(state, memo, g.v, n, m, k, bound);
      return g;
    }
  }

  /* Reaching this point means there is a bug in the selection algorithm. */
  assert(0);
}

/* --- Recursive method: uniform DOAG with n vertices and k sources ------- */

randdag_t doag_unif_nk(gmp_randstate_t state, const memo_t memo, int n, int k,
                       int bound) {
  int m, C;
  mpz_t rank, sum;
  randdag_t g = randdag_alloc(n);

  if (bound < 0)
    bound = n;
  C = min(n - k, bound);

  /* 1. Count the DOAGs with n vertices and k sources. */
  mpz_init(sum);
  for (m = n - k; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
    mpz_add(sum, sum, *doag_count(memo, n, m, k, bound));
  }

  /* 2. Sanity check: there should exist a DOAG with parameters (n, k). */
  if (mpz_sgn(sum) <= 0) {
    fprintf(stderr,
            "Invalid parameters, there is no DOAG with n=%d, k=%d, bound=%d\n",
            n, k, bound);
    assert(0);
  }

  /* 3. Select the number of edges. */
  mpz_init(rank);
  mpz_urandomm(rank, state, sum);
  mpz_clear(sum);
  for (m = n - k; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
    mpz_sub(rank, rank, *doag_count(memo, n, m, k, bound));
    if (mpz_sgn(rank) < 0) {
      /* Found, do some cleanups and call the main sampler. */
      mpz_clear(rank);
      _doag_unif(state, memo, g.v, n, m, k, bound);
      return g;
    }
  }

  /* Reaching this point means that there is a bug in the algorithm. */
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
 * Return 0 <= k < n with probability proportional to 1/k!
 * This is Knuth's poisson generator [1] + a rejection procedure when the
 * result is larger than n.
 *
 * [1] https://en.wikipedia.org/wiki/Poisson_distribution#Computational_methods
 */
static int bounded_poisson(gmp_randstate_t state, int n) {
  int k;
  double p;

  /* exp(-1) */
  static const double r =
      0.3678794411714423215955237701614608674458111310317678345078368016;

  while (1) {
    p = 1.0;
    k = 0;
    while (k < n) {
      /* This generates a uniform double in [0; 1] and multiplies it by p.
       * Thank you C89 for not providing the 0x1.0p-53 syntax, thus forcing me
       * to write this big ugly constant <3 */
      p = p * gmp_urandomb_ui(state, 53) * 1.110223024625156540423631668090e-16;
      if (p < r) {
        return k;
      }
      k++;
    }
  }
}

/**
 * Take two adjacent arrays of vertices, perform a uniform permutation of the
 * second array, and perform a uniform shuffling of the first array with the
 * permuted one.
 * The permutation and shuffling algorithms are intertwined so that they can
 * be efficiently implemented in place.
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
       * The value of `r` can be used as our uniform index since, in this
       * branch of the if, it is a uniform integer from [0; n[ constrained to
       * be >= k i.e. a uniform integer from [k; n[. */
      v[n - 1] = v[r];
      v[r] = tmp;
    }
    n--;
  }
}

/** Simulate the generation of a uniform matrix of variations, but computes
 * just enough information to know it should be rejected or if it corresponds
 * to a valid labelled transition matrix of DOAG. */
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

/** Main function: generate a uniform DOAG of size n */
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
