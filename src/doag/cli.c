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
#include <stdlib.h>

#include "../../includes/doag.h"
#include "../common/cli.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))

static randdag_t sampler(gmp_randstate_t state, memo_t memo, int n, int m,
                         int bound) {
  if (m >= 0) {
    /* We want control over the number of edges. */
    return doag_unif_nm(state, memo, n, m, bound);
  } else if (bound >= 0) {
    /* No control over the number of edge BUT we have a bound on the out-degree
     * => we must do some work. */
    int k;
    mpz_t rank, sum;

    /* 1. Count the DOAGs with n vertices and bounded out-degree. */
    mpz_init(sum);
    for (k = 0; k <= n; k++) {
      const int C = min(n - k, bound);
      for (m = n - k; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
        mpz_add(sum, sum, *doag_count(memo, n, m, k, bound));
      }
    }

    /* 2. Select the value of m and k. */
    mpz_init(rank);
    mpz_urandomm(rank, state, sum);
    mpz_clear(sum);
    for (k = 0; k <= n; k++) {
      const int C = min(n - k, bound);
      for (m = n - k; m <= (C * (C - 1)) / 2 + C * (n - C); m++) {
        mpz_sub(rank, rank, *doag_count(memo, n, m, k, bound));
        if (mpz_sgn(rank) < 0) {
          /* Found, do some cleanups and call the main sampler. */
          mpz_clear(rank);
          return doag_unif_nmk(state, memo, n, m, k, bound);
        }
      }
    }

    /* Reaching this point means that there is a bug in the algorithm. */
    exit(2);
  } else {
    /* We can call the fast rejection sampler. */
    return doag_unif_n(state, n);
  }
}

int main(int argc, char *argv[]) {
  return run_cli(argc, argv, doag_count, sampler, RD_DOT_ORDERING);
}
