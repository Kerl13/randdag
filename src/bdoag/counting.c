#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/bdoag.h"

#include <assert.h>


static inline int max(int x, int y) { return x < y ? y : x; }
static inline int min(int x, int y) { return x < y ? x : y; }


// Memoization

bdoag_memo bdoag_memo_alloc(int N, int M, int bound) {
  mpz_t*** vals = calloc(N - 1, sizeof(mpz_t**));

  for (int n = 2; n <= N; n++) {
    vals[n - 2] = calloc(n - 1, sizeof(mpz_t*));

    for (int k = 1; k < n; k++) {
      const int C = min(bound, n - k);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      vals[n - 2][k - 1] = calloc(max_m + 2 - n, sizeof(mpz_t));

      for (int m = n - 1; m <= max_m; m++) {
        mpz_init(vals[n - 2][k - 1][m + 1 - n]);
      }
    }
  }

  bdoag_memo m = {.N = N, .M = M, .bound = bound, .vals = vals};
  m.one = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*m.one, 1);
  return m;
}

void bdoag_memo_free(bdoag_memo memo) {
  const int bound = memo.bound;
  const int M = memo.M;
  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_clear(memo.vals[n - 2][k - 1][m + 1 - n]);
      }
      free(memo.vals[n - 2][k - 1]);
    }
    free(memo.vals[n - 2]);
  }
  free(memo.vals);
  mpz_clear(*memo.one);
  free(memo.one);
}

void bdoag_memo_dump(FILE* fd, const bdoag_memo memo) {
  const int M = memo.M;
  const int bound = memo.bound;
  fprintf(fd, "%d %d %d\n", memo.N, M, bound);

  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_t* x = bdoag_memo_get_ptr(memo, n, m, k);
        if (mpz_sgn(*x) > 0) {
          fprintf(fd, "%d %d %d ", n, m, k);
          mpz_out_str(fd, 10, *x);
          fprintf(fd, "\n");
        }
      }
    }
  }
}

void doag_memo_load(bdoag_memo memo, FILE* fd) {
  int n, m, k;

  while (1) {
    if (fscanf(fd, "%d %d %d ", &n, &m, &k) == EOF) break;
    mpz_t* z = bdoag_memo_get_ptr(memo, n, m, k);
    mpz_inp_str(*z, fd, 10);
    fscanf(fd, "\n");
  }
}

// Counting

mpz_t* bdoag_count(bdoag_memo memo, int n, int m, int k, int bound) {
  assert(k >= 1);
  assert(n == 1 || k < n);
  assert(n - 1 <= m);

  const int C = min(bound, n - k);
  assert(m <= k * C + C * (C - 1) / 2 + (n - k - C) * bound);

  if (n <= 2) {
    assert(k == 1);
    assert(m == n - 1);
    return memo.one;
  } else {
    // mpz_t* res = &(memo.vals[n - 2][k - 1][m + 1 - n]);
    mpz_t* res = bdoag_memo_get_ptr(memo, n, m, k);
    if (mpz_sgn(*res) == 0) {
      mpz_t factor;
      mpz_init(factor);
      // p = q + s
      const int max_p = min(C, m + 2 - n);
      for (int p = 1; p <= max_p; p++) {
        const int s_start = (p == n - k);
        mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
        for (int s = s_start; s <= p - (k == 1); s++) {
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