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

static void _unif_ldag(gmp_randstate_t state, const memo_t memo,
                       const int *labels, randdag_vertex *v, int n, int m,
                       int k) {
  int p, s;
  mpz_t r, factor, factor0, tmp;

  v[0].id = labels[0];

  /* Base case: only one vertex: the sink. */
  if (n == 1) {
    v[0].out_degree = 0;
    v[0].out_edges = NULL;
    return;
  }

  if (n == 2) {
    /* assert(m == 1); */
    /* assert(k == 1); */
    _unif_ldag(state, memo, labels + 1, v + 1, 1, 0, 1);
    v[0].out_degree = 1;
    v[0].out_edges = calloc(1, sizeof(randdag_vertex));
    v[0].out_edges[0] = v[1];
    return;
  }

  mpz_init(r);
  mpz_init(factor);
  mpz_init_set_ui(factor0, 1);
  mpz_init(tmp);

  mpz_init_set(tmp, *ldag_count(memo, n, m, k, -1));
  mpz_mul_ui(tmp, tmp, k);
  mpz_divexact_ui(tmp, tmp, n);

  mpz_urandomm(r, state, tmp);

  /* p = q + s */
  for (p = 1; p <= min(n - k, m + 2 - n); p++) {
    const int s_start = (p == n - k);

    mpz_mul_ui(factor0, factor0, k - 1 + p);
    mpz_divexact_ui(factor0, factor0, p);
    mpz_set(factor, factor0);

    if (s_start == 1) {
      mpz_mul_ui(factor, factor, p * (n - k - p + 1));
      mpz_divexact_ui(factor, factor, k - 1 + p);
    }

    for (s = s_start; s <= p - (k == 1); s++) {
      const int q = p - s;
      const int kk = k - 1 + q;
      if (m + kk * (kk - 1) / 2 <= p + (n - 1) * (n - 2) / 2) {
        mpz_submul(r, *ldag_count(memo, n - 1, m - p, kk, -1), factor);
        if (mpz_sgn(r) < 0) {
          _unif_ldag(state, memo, labels + 1, v + 1, n - 1, m - p, kk);
          _add_src(state, v, v + kk + 1, kk, n - 1 - kk, s, q);
          mpz_clears(r, tmp, factor, factor0, NULL);
          return;
        }
      }

      mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
      mpz_divexact_ui(factor, factor, (s + 1) * kk);
    }
  }

  /* Shouldn't reach this point */
  assert(0);
}

randdag_t ldag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m) {
  randdag_t g = randdag_alloc(n);
  int *labels = calloc(n, sizeof(int));
  _fisher_yates(state, labels, n);
  _unif_ldag(state, memo, labels, g.v, n, m, 1);
  return g;
}

randdag_t ldag_unif_m(gmp_randstate_t state, const memo_t memo, int m) {
  int n;
  mpz_t r, tot;
  mpz_inits(r, tot, NULL);

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2)
      mpz_add(tot, tot, *ldag_count(memo, n, m, 1, -1));
  }
  mpz_urandomm(r, state, tot);

  for (n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2) {
      mpz_sub(r, r, *ldag_count(memo, n, m, 1, -1));
      if (mpz_sgn(r) == -1) {
        mpz_clear(r);
        mpz_clear(tot);
        return ldag_unif_nm(state, memo, n, m);
      }
    }
  }

  /* Shouldn't reach this point */
  assert(0);
}
