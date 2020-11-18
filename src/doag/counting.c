#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/doag.h"


static inline int max(int x, int y) { return x < y ? y : x; }
static inline int min(int x, int y) { return x < y ? x : y; }


// Memoization

doag_memo doag_memo_alloc(int N, int M) {
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

  doag_memo m = {.N = N, .M = M, .vals = vals};
  m.one = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*m.one, 1);
  return m;
}

void doag_memo_free(doag_memo memo) {
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
  free(memo.one);
}

void doag_memo_dump(FILE* fd, const doag_memo memo) {
  fprintf(fd, "%d %d\n", memo.N, memo.M);
  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int max_m = min((n - k) * (n - k - 1) / 2 + k * (n - k), memo.M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_t* x = doag_memo_get_ptr(memo, n, m, k);
        if (mpz_sgn(*x) > 0) {
          fprintf(fd, "%d %d %d ", n, m, k);
          mpz_out_str(fd, 10, *x);
          fprintf(fd, "\n");
        }
      }
    }
  }
}

void doag_memo_load(doag_memo memo, FILE* fd) {
  int n, m, k;

  while (1) {
    if (fscanf(fd, "%d %d %d ", &n, &m, &k) == EOF) break;
    mpz_t* z = doag_memo_get_ptr(memo, n, m, k);
    mpz_inp_str(*z, fd, 10);
    fscanf(fd, "\n");
  }
}

// Counting

mpz_t* doag_count(doag_memo memo, int n, int m, int k) {
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
    mpz_t* res = doag_memo_get_ptr(memo, n, m, k);
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
