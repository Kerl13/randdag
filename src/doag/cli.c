#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/doag.h"

mpz_t zero;

randdag_t doag_sampler(gmp_randstate_t state, memo_t memo, int M) {
  return doag_unif_m(state, memo, M);
}

mpz_t* doag_counter(memo_t memo, int n, int m) {
  if (m <= n * (n - 1) / 2) return doag_count(memo, n, m, 1);
  else return &zero;
}

int main(int argc, char* argv[]) {
  mpz_init(zero);
  int res = run_cli(argc, argv, doag_counter, doag_sampler);
  mpz_clear(zero);
  return res;
}
