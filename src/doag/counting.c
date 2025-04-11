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

#include <assert.h>

#include "../../includes/doag.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define IMPLIES(A, B) (!(A)) || (B)

static mpz_t *_doag_count(memo_t memo, int n, int m, int k, int bound) {
  const int C = min(bound, n - k);

  /* As an optimisation, we try very hard to call this function only for
   * arguments such that DOAG(n,m,k,bound) > 0. Using various
   * combinatorial arguments, we get the following conditions on (n,m,k,bound).
   * They are expressed as asserts for easier debugging. */

  /* Negative parameters are absurd. */
  assert(bound >= 0);
  assert(n >= 0);
  /* At most n sources and at least one except for the empty graph. */
  assert((n > 0) <= k && k <= n);
  /* At least (n-k) edge: one for each internal vertex.
   * At most one C out-edges per source + max(bound,n-k-i) for the i-th internal
   * vertex. */
  assert(n - k <= m && m <= C * (C - 1) / 2 + (n - C) * C);

  /* Small cases */
  if (n <= 1) {
    return memo.one;
  } else {
    int p, i;
    mpz_t factor;
    mpz_t *res = memo_get_ptr(memo, n, m, k);

    /* Memoisation. */
    if (mpz_sgn(*res) != 0)
      return res;

    mpz_init(factor);

    /* For the invariant to hold recursively, we must have:
       1. Condition on the number of sources:
          1 <= k - 1 + p - i <= n - 1, which is equivalent to
          p - (n-k) <= i <= p + (k-2)
       2. Condition on the number of edges:
          n - k - p + i <= m - p <= C2(C2 - 1)/2 + C2(n - 1 - C2)
          with C2 = min(bound, n - k - (p - i))
       In addition, by definition we have 0 <= i <= p.

       It follows that
           max(0, p - (n-k)) = 0 <= i
       and
           i <= min(p, p + k - 2, m - n + k) = min(p - indic(k==1), m-n+k) */
    for (p = 0; p <= min(C, m); p++) {
      mpz_set_ui(factor, 1);
      for (i = 0; i <= min(p - (k == 1), m - n + k); i++) {
        /* Loop invariant: factor = binom(n-k-p+i,i) * p! / (p-i)! */
        const int C2 = min(n - k - (p - i), bound);
        if (m - p <= (C2 * (C2 - 1)) / 2 + C2 * (n - 1 - C2)) {
          mpz_addmul(*res,
                     *_doag_count(memo, n - 1, m - p, k - 1 + p - i, bound),
                     factor);
        }
        mpz_mul_ui(factor, factor, (n - k - p + i + 1) * (p - i));
        mpz_divexact_ui(factor, factor, i + 1);
      }
    }
    mpz_clear(factor);

    assert(mpz_sgn(*res) > 0);
    return res;
  }
}

mpz_t *doag_unsafe_count(memo_t memo, int n, int m, int k, int bound) {
  if (bound < 0)
    bound = n;
  return _doag_count(memo, n, m, k, bound);
}

mpz_t *doag_count(memo_t memo, int n, int m, int k, int bound) {
  int C;

  if (bound < 0)
    bound = n;
  C = min(bound, n - k);

  /* See the comment at the top of _doag_count for the explanations on each of
   * these conditions. */
  if ((n < 0) || ((n > 0) > k) || (k > n) || (n - k > m) ||
      (m > C * (C - 1) / 2 + (n - C) * C)) {
    return memo.zero;
  }

  return _doag_count(memo, n, m, k, bound);
}
