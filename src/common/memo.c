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

#include "../../includes/common.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

memo_t memo_alloc(int N, int M, int bound) {
  int n, m, k;
  mpz_t ***vals;
  memo_t memo;

  /* Negative bounds means unbounded. */
  if (bound < 0)
    bound = N;

  if (M < 0)
    M = N * (N - 1) / 2;

  vals = calloc(N - 1, sizeof(mpz_t **));
  for (n = 2; n <= N; n++) {
    vals[n - 2] = calloc(n, sizeof(mpz_t *));

    for (k = 1; k <= n; k++) {
      const int C = min(bound, n - k);
      const int max_m = min((C - 1) * C / 2 + C * (n - C), M);
      vals[n - 2][k - 1] = calloc(max_m + 1, sizeof(mpz_t));

      for (m = 0; m <= max_m; m++) {
        mpz_init(vals[n - 2][k - 1][m]);
      }
    }
  }

  memo.vals = vals;
  memo.N = N;
  memo.M = M;
  memo.bound = bound;
  memo.zero = malloc(sizeof(mpz_t));
  memo.one = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*memo.one, 1);
  mpz_init_set_ui(*memo.zero, 0);

  return memo;
}

void memo_free(memo_t memo) {
  int n, m, k;
  const int bound = memo.bound;
  const int M = memo.M;

  for (n = 2; n <= memo.N; n++) {
    for (k = 1; k <= n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min((C - 1) * C / 2 + C * (n - C), M);
      for (m = 0; m <= max_m; m++) {
        mpz_clear(memo.vals[n - 2][k - 1][m]);
      }
      free(memo.vals[n - 2][k - 1]);
    }
    free(memo.vals[n - 2]);
  }

  free(memo.vals);
  mpz_clear(*memo.zero);
  mpz_clear(*memo.one);
  free(memo.zero);
  free(memo.one);
}

void memo_dump(FILE *fd, const memo_t memo) {
  int n, m, k;
  fprintf(fd, "%d %d %d\n", memo.N, memo.M, memo.bound);

  for (n = 2; n <= memo.N; n++) {
    for (k = 1; k <= n; k++) {
      const int C = min(n - k, memo.bound);
      const int max_m = min((C - 1) * C / 2 + C * (n - C), memo.M);
      for (m = 0; m <= max_m; m++) {
        mpz_t *x = memo_get_ptr(memo, n, m, k);
        if (mpz_sgn(*x) > 0) {
          fprintf(fd, "%d %d %d ", n, m, k);
          mpz_out_str(fd, 10, *x);
          fprintf(fd, "\n");
        }
      }
    }
  }
}

void memo_load(memo_t memo, FILE *fd) {
  int n, m, k;
  mpz_t *z;

  while (1) {
    if (fscanf(fd, "%d %d %d ", &n, &m, &k) == EOF)
      break;
    z = memo_get_ptr(memo, n, m, k);
    mpz_inp_str(*z, fd, 10);
    fscanf(fd, "\n");
  }
}
