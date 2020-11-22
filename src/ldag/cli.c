#include <stdlib.h>
#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/common.h"
#include "../../includes/ldag.h"

mpz_t zero;

#define ignore(x) ((void)x)

void not_implemented() {
  fprintf(stderr, "Not implemented\n");
  exit(2);
}

randdag_t sampler(gmp_randstate_t s, memo_t memo, int M) {
  ignore(s);
  ignore(M);
  ignore(memo);
  not_implemented();
}

mpz_t* counter(memo_t memo, int n, int m) {
  if (m <= n * (n - 1) / 2) return ldag_count(memo, n, m, 1);
  else return &zero;
}

int main(int argc, char* argv[]) {
  mpz_init(zero);
  int res = run_cli(argc, argv, counter, sampler); 
  mpz_clear(zero);
  return res;
}
