#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../../includes/common.h"
#include "../../includes/bdoag.h"

static inline int min(int x, int y) { return x < y ? x : y; }
static inline int max(int x, int y) { return x < y ? y : x; }

mpz_t zero;
bdoag_memo memo;
int bound = 2; // TODO: make this a cmd line argument.

randdag_t bdoag_sampler(gmp_randstate_t state, int M) {
  return bdoag_unif_m(state, memo, M, bound);
}

mpz_t* bdoag_counter(int n, int m) {
  const int C = min(bound, n - 1);
  if (m <= (C + 1) * C / 2 +  (n - 1 - C) * bound)
    return bdoag_count(memo, n, m, 1, bound);
  else
    return &zero;
}

void bdoag_dumper(FILE* fd) {
  bdoag_memo_dump(fd, memo);
}

int prepare(int M, const char* filename) {
  mpz_init(zero);

  if (filename) {
    FILE* fd = fopen(filename, "r");
    if (fd != NULL) {
      int N, M2;
      fscanf(fd, "%d %d %d\n", &N, &M2, &bound);

      M = max(M, M2);
      N = max(M + 1, N);
      memo = bdoag_memo_alloc(N, M, bound);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", filename);
      return 1;
    }
  } else {
    memo = bdoag_memo_alloc(M + 1, M, bound);
  }

  return M;
}

int main(int argc, char* argv[]) {
  int res = run_cli(
    argc, argv, prepare, bdoag_counter, bdoag_sampler, bdoag_dumper
  );

  bdoag_memo_free(memo);
  mpz_clear(zero);

  return res;
}
