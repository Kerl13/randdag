#include <malloc.h>
#include <stdio.h>
#include <gmp.h>

#include "../../includes/common.h"


static inline int min(int x, int y) { return x < y ? x : y; }


memo_t memo_alloc(int N, int M, int bound) {
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

  memo_t m = {.N = N, .M = M, .vals = vals, .bound = bound};
  m.one = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*m.one, 1);
  return m;
}


void memo_free(memo_t memo) {
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

void memo_dump(FILE* fd, const memo_t memo) {
  const int M = memo.M;
  const int bound = memo.bound;
  fprintf(fd, "%d %d %d\n", memo.N, M, bound);

  for (int n = 2; n <= memo.N; n++) {
    for (int k = 1; k < n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      for (int m = n - 1; m <= max_m; m++) {
        mpz_t* x = memo_get_ptr(memo, n, m, k);
        if (mpz_sgn(*x) > 0) {
          fprintf(fd, "%d %d %d ", n, m, k);
          mpz_out_str(fd, 10, *x);
          fprintf(fd, "\n");
        }
      }
    }
  }
}

void memo_load(memo_t memo, FILE* fd) {
  int n, m, k;

  while (1) {
    if (fscanf(fd, "%d %d %d ", &n, &m, &k) == EOF) break;
    mpz_t* z = memo_get_ptr(memo, n, m, k);
    mpz_inp_str(*z, fd, 10);
    fscanf(fd, "\n");
  }
}
