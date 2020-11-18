#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../../includes/bdoag.h"
#include "../cli/cli.h"

static inline int min(int x, int y) {
  return x < y ? x : y;
}

int main(int argc, char* argv[]) {
  randdag_cli_options opts = randdag_cli_parse(30, argc, argv);
  int bound = 2;

  int M = opts.count;
  bdoag_memo memo = bdoag_memo_alloc(M + 1, M, bound);

  // Counting

  mpz_t res; mpz_init(res);
  for (int m = 1; m <= M; m++) {
    mpz_set_ui(res, 0);
    for (int n = 2; n <= m + 1; n++) {
      const int C = min(bound, n - 1);
      if (m <= (C + 1) * C / 2 +  (n - 1 - C) * bound)
        mpz_add(res, res, *bdoag_count(memo, n, m, 1, bound));
    }
    printf("%d: ", m);
    mpz_out_str(stdout, 10, res);
    printf("\n");
    fflush(stdout);
  }
  mpz_clear(res);

  bdoag_memo_free(memo);

  return 0;
}
