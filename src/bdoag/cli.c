#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/bdoag.h"

static inline int min(int x, int y) { return x < y ? x : y; }

mpz_t zero;
int bound = 2; // TODO: make this a cmd line argument.

randdag_t bdoag_sampler(gmp_randstate_t state, memo_t memo, int M) {
  return bdoag_unif_m(state, memo, M, bound);
}

mpz_t* bdoag_counter(memo_t memo, int n, int m) {
  const int C = min(bound, n - 1);
  if (m <= (C + 1) * C / 2 +  (n - 1 - C) * bound)
    return bdoag_count(memo, n, m, 1, bound);
  else
    return &zero;
}

int main(int argc, char* argv[]) {
  mpz_init(zero);
  long dot_flags = RD_DOT_ORDERING;
  int res = run_cli(argc, argv, bdoag_counter, bdoag_sampler, dot_flags);
  mpz_clear(zero);
  return res;
}
