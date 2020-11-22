#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../common/cli.h"
#include "../../includes/doag.h"

mpz_t zero;
memo_t memo;

randdag_t doag_sampler(gmp_randstate_t state, int M) {
  return doag_unif_m(state, memo, M);
}

mpz_t* doag_counter(int n, int m) {
  if (m <= n * (n - 1) / 2) return doag_count(memo, n, m, 1);
  else return &zero;
}

void doag_dumper(FILE* fd) {
  memo_dump(fd, memo);
}

int prepare(int M, const char* filename) {
  mpz_init(zero);

  if (filename) {
    FILE* fd = fopen(filename, "r");
    if (fd != NULL) {
      int N, M2;
      fscanf(fd, "%d %d\n", &N, &M2);

      M = (M2 > M) ? M2 : M;
      N = (N > (M + 1)) ? N : (M + 1);

      memo = memo_alloc(N, M, N);
      memo_load(memo, fd);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", filename);
      exit(1);
    }
  } else {
    memo = memo_alloc(M + 1, M, M + 1);
  }

  return M;
}

int main(int argc, char* argv[]) {
  int res = run_cli(argc, argv, prepare, doag_counter, doag_sampler, doag_dumper);

  mpz_clear(zero);
  memo_free(memo);

  return res;
}
