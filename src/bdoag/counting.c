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

#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/bdoag.h"


#define min(x, y) (((x) < (y)) ? (x) : (y))

mpz_t* bdoag_count(memo_t memo, int n, int m, int k, int bound) {
  const int C = min(bound, n - k);
  /* assert(k >= 1); */
  /* assert(n == 1 || k < n); */
  /* assert(n - 1 <= m); */
  /* assert(m <= k * C + C * (C - 1) / 2 + (n - k - C) * bound); */

  if (n <= 2) {
    /* assert(k == 1); */
    /* assert(m == n - 1); */
    return memo.one;
  } else {
    int p, s;
    mpz_t* res = memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      const int max_p = min(C, m + 2 - n);
      mpz_t factor;
      mpz_init(factor);
      /* p = q + s */
      for (p = 1; p <= max_p; p++) {
        const int s_start = (p == n - k);
        mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
        for (s = s_start; s <= p - (k == 1); s++) {
          const int q = p - s;
          const int C2 = min(n - q - k, bound);
          if (m - p  <= (k - 1 + q) * C2 + C2 * (C2 - 1) / 2 + (n - q - k - C2) * bound)
            mpz_addmul(*res, *bdoag_count(memo, n - 1, m - p, k - 1 + q, bound), factor);
          mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
          mpz_divexact_ui(factor, factor, s + 1);
        }
      }
      mpz_clear(factor);
    }
    return res;
  }
}
