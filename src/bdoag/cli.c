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

static int sample(char* sample_file, const bdoag_memo memo, int M, int bound) {
  // Setup output file
  FILE* fd = stdout;
  if (strcmp("-", sample_file) != 0)
    fd = fopen(sample_file, "w");
  if (fd == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", sample_file);
    return 1;
  }

  // Prepare RNG
  gmp_randstate_t state;
  gmp_randinit_mt(state);
  unsigned long int seed;
  getrandom(&seed, sizeof(unsigned long int), 0);
  printf("Using random seed 0x%lx\n", seed);
  gmp_randseed_ui(state, seed);

  // Sample
  randdag_t g = bdoag_unif_m(state, memo, M, bound);
  randdag_to_dot(fd, g);
  fflush(fd);

  // Do some cleaning
  if (fd != stdout) fclose(fd);
  randdag_free(g);
  gmp_randclear(state);

  return 0;
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

  // Random sampling

  if (opts.sample_file != NULL) {
    int r = sample(opts.sample_file, memo, M, bound);
    if (r != 0) return r;
  }

  // Some cleaning

  bdoag_memo_free(memo);

  return 0;
}
