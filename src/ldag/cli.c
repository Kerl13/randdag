#include <stdlib.h>
#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/common.h"
#include "../../includes/ldag.h"

mpz_t zero;
memo_t memo;

#define ignore(x) ((void)x)

void not_implemented() {
  fprintf(stderr, "Not implemented\n");
  exit(2);
}

randdag_t sampler(gmp_randstate_t s, int M) {
  ignore(s);
  ignore(M);
  not_implemented();
}

mpz_t* counter(int n, int m) {
  if (m <= n * (n - 1) / 2) return ldag_count(memo, n, m, 1);
  else return &zero;
}

void dumper(FILE* fd) {
  ignore(fd);
  not_implemented();
}

int prepare(int M, const char* filename) {
  if (filename) not_implemented();
  memo = memo_alloc(M + 1, M, M + 1);
  return M;
}

int main(int argc, char* argv[]) {
  int res = run_cli(argc, argv, prepare, counter, sampler, dumper); 

  mpz_clear(zero);
  memo_free(memo);

  return res;
}
