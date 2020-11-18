#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../../includes/common.h"
#include "../../includes/doag.h"
#include "../cli/cli.h"

doag_memo memo;
int M;

// Alias of type __sampler_t of the sampling function for use in the generic
// sampler.
randdag_t doag_sampler(gmp_randstate_t state) {
  return doag_unif_m(state, memo, M);
}

int main(int argc, char* argv[]) {
  randdag_cli_options opts = randdag_cli_parse(30, argc, argv);
  M = opts.count;

  // Alloc memo and load

  if (opts.load_file) {
    FILE* fd = fopen(opts.load_file, "r");
    if (fd != NULL) {
      int N, M2;
      fscanf(fd, "%d %d\n", &N, &M2);

      M = (M2 > M) ? M2 : M;
      N = (N > (M + 1)) ? N : (M + 1);

      memo = doag_memo_alloc(N, M);
      doag_memo_load(memo, fd);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.load_file);
      return 1;
    }
  } else {
    memo = doag_memo_alloc(M + 1, M);
  }

  // Counting

  mpz_t res; mpz_init(res);
  for (int m = 1; m <= M; m++) {
    mpz_set_ui(res, 0);
    for (int n = 2; n <= m + 1; n++) {
      if (m <= n * (n - 1) / 2)
        mpz_add(res, res, *doag_count(memo, n, m, 1));
    }
    printf("%d: ", m);
    mpz_out_str(stdout, 10, res);
    printf("\n");
    fflush(stdout);
  }
  mpz_clear(res);

  // Dump

  if (opts.dump_file) {
    FILE* fd = fopen(opts.dump_file, "w");
    if (fd != NULL) {
      doag_memo_dump(fd, memo);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.dump_file);
      return 1;
    }
  }

  // Random sampling

  if (opts.sample_file != NULL) {
    int r = generic_sampler(opts.sample_file, doag_sampler);
    if (r != 0) return r;
  }

  // Some cleaning

  doag_memo_free(memo);

  return 0;
}
