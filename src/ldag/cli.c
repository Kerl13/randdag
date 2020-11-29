#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/ldag.h"

mpz_t zero;

static randdag_t sampler(gmp_randstate_t s, memo_t memo, int m) {
  return ldag_unif_m(s, memo, m);
}

static mpz_t* counter(memo_t memo, int n, int m) {
  if (m <= n * (n - 1) / 2) return ldag_count(memo, n, m, 1);
  else return &zero;
}

int main(int argc, char* argv[]) {
  int exitcode;
  mpz_init(zero);
  exitcode = run_cli(argc, argv, counter, sampler, RD_DOT_LABELLED); 
  mpz_clear(zero);
  return exitcode;
}
