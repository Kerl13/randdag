#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/doag.h"


static inline int min(int x, int y) { return x < y ? x : y; }


mpz_t* doag_count(memo_t memo, int n, int m, int k) {
  // assert(k >= 1);
  // assert(n == 1 || k < n);
  // assert(n - 1 <= m);
  // assert(m <= n * (n - 1) / 2 - k * (k - 1) / 2);
  if (n <= 2) {
    // assert(k == 1);
    // assert(m == n - 1);
    return memo.one;
  } else {
    // mpz_t* res = &(memo.vals[n - 2][k - 1][m + 1 - n]);
    mpz_t* res = memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      mpz_t factor;
      mpz_init(factor);
      // p = q + s
      for (int p = 1; p <= min(n - k, m + 2 - n); p++) {
        const int s_start = (p == n - k);
        mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
        for (int s = s_start; s <= p - (k == 1); s++) {
          const int q = p - s;
          if (m + (k - 1 + q) * (k - 2 + q) / 2 <= p + (n - 1) * (n - 2) / 2)
            mpz_addmul(*res, *doag_count(memo, n - 1, m - p, k - 1 + q), factor);
          mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
          mpz_divexact_ui(factor, factor, s + 1);
        }
      }
      mpz_clear(factor);
    }
    return res;
  }
}
