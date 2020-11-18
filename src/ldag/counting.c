#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/ldag.h"

#include <assert.h>


static inline int max(int x, int y) { return x < y ? y : x; }
static inline int min(int x, int y) { return x < y ? x : y; }


// Memoization

ldag_memo ldag_memo_alloc(int N, int M) {
  mpz_t*** vals = calloc(N - 1, sizeof(mpz_t**));

  for (int n = 2; n <= N; n++) {
    vals[n - 2] = calloc(n - 1, sizeof(mpz_t*));

    for (int k = 1; k < n; k++) {
      const int max_m = min((n - k) * (n - k - 1) / 2 + k * (n - k), M);
      vals[n - 2][k - 1] = calloc(max_m + 2 - n, sizeof(mpz_t));

      for (int m = n - 1; m <= max_m; m++) {
        mpz_init(vals[n - 2][k - 1][m + 1 - n]);
      }
    }
  }

  ldag_memo m = {.N = N, .M = M, .vals = vals};
  m.one = malloc(sizeof(mpz_t));
  m.two = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*m.one, 1);
  mpz_init_set_ui(*m.two, 2);
  return m;
}

void ldag_memo_free(ldag_memo memo) {
  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int max_m = min((n - k) * (n - k - 1) / 2 + k * (n - k), memo.M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_clear(memo.vals[n - 2][k - 1][m + 1 - n]);
      }
      free(memo.vals[n - 2][k - 1]);
    }
    free(memo.vals[n - 2]);
  }
  free(memo.vals);
  mpz_clear(*memo.one);
  mpz_clear(*memo.two);
  free(memo.one);
}

void ldag_memo_dump(FILE* fd, const ldag_memo memo) {
  fprintf(fd, "%d %d\n", memo.N, memo.M);
  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int max_m = min((n - k) * (n - k - 1) / 2 + k * (n - k), memo.M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_t* x = ldag_memo_get_ptr(memo, n, m, k);
        if (mpz_sgn(*x) > 0) {
          fprintf(fd, "%d %d %d ", n, m, k);
          mpz_out_str(fd, 10, *x);
          fprintf(fd, "\n");
        }
      }
    }
  }
}

void ldag_memo_load(ldag_memo memo, FILE* fd) {
  int n, m, k;

  while (1) {
    if (fscanf(fd, "%d %d %d ", &n, &m, &k) == EOF) break;
    mpz_t* z = ldag_memo_get_ptr(memo, n, m, k);
    mpz_inp_str(*z, fd, 10);
    fscanf(fd, "\n");
  }
}


mpz_t* ldag_count(ldag_memo memo, int n, int m, int k) {
  // assert(k >= 1);
  // assert(n == 1 || k < n);
  // assert(n - 1 <= m);
  // assert(m <= n * (n - 1) / 2 - k * (k - 1) / 2);
  if (n == 1) {
    // assert(k == 1);
    // assert(m == n - 1);
    return memo.one;
  } else if (n == 2) {
    // assert(k == 1);
    // assert(m == n - 1);
    return memo.two;
  } else {
    mpz_t* res = ldag_memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      mpz_t factor, factor0;

      // Invariant: factor = binomial(n - k - q, s) * binomial(k - 1 + q, q)
      mpz_init(factor); 

      // Invariant: factor0 = factor(s=0) = binomial(k - 1 + p, p)
      mpz_init_set_ui(factor0, 1);

      for (int p = 1; p <= min(n - k, m + 2 - n); p++) {
        mpz_mul_ui(factor0, factor0, k - 1 + p);
        mpz_divexact_ui(factor0, factor0, p);

        const int s_start = (p == n - k);
        mpz_set(factor, factor0);
        if (s_start == 1) {
          mpz_mul_ui(factor, factor, p * (n - k - p + 1));
          mpz_divexact_ui(factor, factor, k - 1 + p);
        }

        for (int s = s_start; s <= p - (k == 1); s++) {
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
