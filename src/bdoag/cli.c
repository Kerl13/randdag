#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/bdoag.h"


mpz_t zero;

/* TODO: make this a cmd line argument. */
int bound = 2;

static randdag_t bdoag_sampler(gmp_randstate_t state, memo_t memo, int M) {
  return bdoag_unif_m(state, memo, M, bound);
}

static mpz_t* bdoag_counter(memo_t memo, int n, int m) {
  const int C = (bound < n) ? bound : n - 1;
  if (m <= (C + 1) * C / 2 +  (n - 1 - C) * bound)
    return bdoag_count(memo, n, m, 1, bound);
  else
    return &zero;
}

int main(int argc, char* argv[]) {
  int exitcode;
  mpz_init(zero);
  exitcode = run_cli(argc, argv, bdoag_counter, bdoag_sampler, RD_DOT_ORDERING);
  mpz_clear(zero);
  return exitcode;
}
