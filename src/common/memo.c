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

#define min(x, y) (((x) < (y)) ? (x) : (y))

memo_t memo_alloc(int N, int M, int bound) {
  int n, m, k;
  mpz_t ***vals;
  memo_t memo;

  /* Non-positive bound means unbounded. */
  if (bound <= 0)
    bound = N;

  vals = calloc(N - 1, sizeof(mpz_t **));
  for (n = 2; n <= N; n++) {
    vals[n - 2] = calloc(n - 1, sizeof(mpz_t *));

    for (k = 1; k < n; k++) {
      const int C = min(bound, n - k);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      vals[n - 2][k - 1] = calloc(max_m + 2 - n, sizeof(mpz_t));

      for (m = n - 1; m <= max_m; m++) {
        mpz_init(vals[n - 2][k - 1][m + 1 - n]);
      }
    }
  }

  memo.vals = vals;
  memo.N = N;
  memo.M = M;
  memo.bound = bound;
  memo.one = malloc(sizeof(mpz_t));
  mpz_init_set_ui(*memo.one, 1);

  return memo;
}

void memo_free(memo_t memo) {
  int n, m, k;
  const int bound = memo.bound;
  const int M = memo.M;

  for (n = 2; n <= memo.N; n++) {
    for (k = 1; k < n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      for (m = n - 1; m <= max_m; m++) {
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

void memo_dump(FILE *fd, const memo_t memo) {
  int n, m, k;
  const int M = memo.M;
  const int bound = memo.bound;
  fprintf(fd, "%d %d %d\n", memo.N, M, bound);

  for (n = 2; n <= memo.N; n++) {
    for (k = 1; k < n; k++) {
      const int C = min(n - k, bound);
      const int max_m = min(k * C + (C - 1) * C / 2 + bound * (n - k - C), M);
      for (m = n - 1; m <= max_m; m++) {
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
