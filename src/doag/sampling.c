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

static void _add_src(gmp_randstate_t state, randdag_vertex *dest,
                     randdag_vertex *other, int nb_other, int s, int q) {
  /* assert(s + q > 0); */
  randdag_vertex *e;
  int i;

  const randdag_vertex *sources = other - 1;
  dest->out_degree = s + q;
  e = calloc(s + q, sizeof(randdag_vertex));
  dest->out_edges = e;

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
                       randdag_vertex *v, int n, int m, int k) {
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
    _unif_doag(state, memo, v + 1, 1, 0, 1);
    v[0].out_degree = 1;
    v[0].out_edges = calloc(1, sizeof(randdag_vertex));
    v[0].out_edges[0] = v[1];
    return;
  }

  mpz_init(r);
  mpz_init(factor);
  mpz_urandomm(r, state, *doag_count(memo, n, m, k));

  /* p = q + s */
  for (p = 1; p <= min(n - k, m + 2 - n); p++) {
    const int s_start = (p == n - k);
    mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
    for (s = s_start; s <= p - (k == 1); s++) {
      const int q = p - s;
      if (m + (k - 1 + q) * (k - 2 + q) / 2 <= p + (n - 1) * (n - 2) / 2) {
        mpz_submul(r, *doag_count(memo, n - 1, m - p, k - 1 + q), factor);
        if (mpz_sgn(r) == -1) {
          _unif_doag(state, memo, v + 1, n - 1, m - p, k - 1 + q);
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

randdag_t doag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m) {
  randdag_t g = randdag_alloc(n);
  _unif_doag(state, memo, g.v, n, m, 1);
  return g;
}

randdag_t doag_unif_m(gmp_randstate_t state, const memo_t memo, int m) {
  int n;
  mpz_t r, tot;
  mpz_inits(r, tot, NULL);

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2)
      mpz_add(tot, tot, *doag_count(memo, n, m, 1));
  }
  mpz_urandomm(r, state, tot);

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2) {
      mpz_sub(r, r, *doag_count(memo, n, m, 1));
      if (mpz_sgn(r) == -1) {
        mpz_clear(r);
        mpz_clear(tot);
        return doag_unif_nm(state, memo, n, m);
      }
    }
  }

  /* Shouldn't reach this point */
  assert(0);
}
