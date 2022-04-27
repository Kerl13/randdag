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

#include "../../includes/doag.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

mpz_t *doag_count(memo_t memo, int n, int m, int k) {
  /* assert(k >= 1); */
  /* assert(n == 1 || k < n); */
  /* assert(n - 1 <= m); */
  /* assert(m <= n * (n - 1) / 2 - k * (k - 1) / 2); */
  if (n <= 2) {
    /* assert(k == 1); */
    /* assert(m == n - 1); */
    return memo.one;
  } else {
    int p, s;
    mpz_t *res;

    res = memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      mpz_t factor;
      mpz_init(factor);
      /* p = q + s */
      for (p = 1; p <= min(n - k, m + 2 - n); p++) {
        const int s_start = (p == n - k);
        mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
        for (s = s_start; s <= p - (k == 1); s++) {
          const int q = p - s;
          if (m + (k - 1 + q) * (k - 2 + q) / 2 <= p + (n - 1) * (n - 2) / 2)
            mpz_addmul(*res, *doag_count(memo, n - 1, m - p, k - 1 + q),
                       factor);
          mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
          mpz_divexact_ui(factor, factor, s + 1);
        }
      }
      mpz_clear(factor);
    }

    return res;
  }
}
