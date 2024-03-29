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

#include <gmp.h>
#include <malloc.h>
#include <stdio.h>

#include "../../includes/common.h"
#include "../../includes/ldag.h"

#include <assert.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

mpz_t __two;
int __two_set = 0;

mpz_t *ldag_count(memo_t memo, int n, int m, int k) {
  /* assert(k >= 1); */
  /* assert(n == 1 || k < n); */
  /* assert(n - 1 <= m); */
  /* assert(m <= n * (n - 1) / 2 - k * (k - 1) / 2); */
  if (n == 1) {
    /* assert(k == 1); */
    /* assert(m == n - 1); */
    return memo.one;
  } else if (n == 2) {
    /* assert(k == 1); */
    /* assert(m == n - 1); */
    /* XXX. Super ugly… */
    if (!__two_set) {
      mpz_init_set_ui(__two, 2);
      __two_set = 1;
    }
    return &__two;
  } else {
    mpz_t *res = memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      int p, s;
      mpz_t factor, factor0;

      /* Invariant: factor = binomial(n - k - q, s) * binomial(k - 1 + q, q) */
      mpz_init(factor);

      /* Invariant: factor0 = factor(s=0) = binomial(k - 1 + p, p) */
      mpz_init_set_ui(factor0, 1);

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
          if (m + kk * (kk - 1) / 2 <= p + (n - 1) * (n - 2) / 2)
            mpz_addmul(*res, *ldag_count(memo, n - 1, m - p, kk), factor);

          mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
          mpz_divexact_ui(factor, factor, (s + 1) * kk);
        }
      }
      mpz_mul_ui(*res, *res, n);
      mpz_divexact_ui(*res, *res, k);

      mpz_clear(factor);
      mpz_clear(factor0);
    }
    return res;
  }
}
