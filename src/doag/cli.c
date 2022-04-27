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

#include "../../includes/doag.h"
#include "../common/cli.h"

mpz_t zero;

/* TODO: make this a cmd line argument. */
int bound = -1;

static randdag_t doag_sampler(gmp_randstate_t state, memo_t memo, int M) {
  return doag_unif_m(state, memo, M, bound);
}

static mpz_t *doag_counter(memo_t memo, int n, int m) {
  const int C = (bound < n) ? bound : n - 1;
  if (m <= (C + 1) * C / 2 + (n - 1 - C) * bound)
    return doag_count(memo, n, m, 1, bound);
  else
    return &zero;
}

int main(int argc, char *argv[]) {
  int exitcode;
  mpz_init(zero);
  exitcode = run_cli(argc, argv, doag_counter, doag_sampler, RD_DOT_ORDERING);
  mpz_clear(zero);
  return exitcode;
}
