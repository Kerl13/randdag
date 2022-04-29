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

/* FIXME: REMOVE ME: */
#include <stdio.h>

#include <assert.h>
#include <gmp.h>
#include <malloc.h>

#include "../../includes/common.h"
#include "../../includes/doag.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

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

randdag_t doag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m,
                       int bound) {
  randdag_t g = randdag_alloc(n);
  if (bound <= 0)
    bound = m; /* The maximum out-degree in the unbounded case is m */
  _unif_doag(state, memo, g.v, n, m, 1, bound);
  return g;
}

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

static int bern_inv_p_fact(gmp_randstate_t state, int p) {
  int k;
  for (k = p; k > 1; k--) {
    if (gmp_urandomm_ui(state, k))
      return 0;
  }
  return 1;
}

static int bounded_poisson(gmp_randstate_t state, int n) {
  /* Not optimal */
  int p;
  while (1) {
    p = gmp_urandomm_ui(state, n);
    if (bern_inv_p_fact(state, p))
      return p;
  }
}

static int doag_unif_n_sim(gmp_randstate_t state, int n, int *params,
                           int *row_sizes) {
  int *path;
  int i, j, streak;

  params[0] = bounded_poisson(state, n - 2);
  row_sizes[0] = n - 1;

  path = calloc(n, sizeof(int));
  path[0] = -1;
  path[1] = 0;

  streak = 1;
  for (j = 2; j < n - 1; j++) {
    i = j - 1;
    params[i] = bounded_poisson(state, n - 1 - i);
    row_sizes[i] = n - i - 1;
    while (i >= 0) {
      if ((int)gmp_urandomm_ui(state, row_sizes[i]) < params[i]) {
        params[i] -= 1;
        row_sizes[i] -= 1;
        i -= 1;
      } else {
        row_sizes[i] -= 1;
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
  path[n - 1] = n - 2;

exit:
  if (j == n - 1) {
    for (i = 0; i < n; i++) {
      if (path[i] == i - 1) {
        printf("%c[0m%c[39m%d ", 0x1B, 0x1B, i);
      } else {
        printf("%c[1m%c[31m%d ", 0x1B, 0x1B, i);
      }
    }
    printf("\n");
  } else {
    printf("Reject\n");
  }

  free(path);
  return (j == n - 1);
}

randdag_t doag_unif_n(gmp_randstate_t state, int n) {
  int *params;
  int *row_sizes;
  int ok;

  params = calloc(n - 1, sizeof(int));
  row_sizes = calloc(n - 1, sizeof(int));

  do {
    ok = doag_unif_n_sim(state, n, params, row_sizes);
  } while (!ok);

  free(params);
  free(row_sizes);
  return randdag_alloc(n); /* TODO: construct this graph */
}
