#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/doag.h"

mpz_t zero;

static randdag_t doag_sampler(gmp_randstate_t state, memo_t memo, int M) {
  return doag_unif_m(state, memo, M);
}

static mpz_t* doag_counter(memo_t memo, int n, int m) {
  if (m <= n * (n - 1) / 2) return doag_count(memo, n, m, 1);
  else return &zero;
}

int main(int argc, char* argv[]) {
  int exitcode;
  mpz_init(zero);
  exitcode = run_cli(argc, argv, doag_counter, doag_sampler, RD_DOT_ORDERING);
  mpz_clear(zero);
  return exitcode;
}
