#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> // getrandom
#include <assert.h>
#include <gmp.h>

#include "../../includes/common.h"
#include "../../includes/bdoag.h"
#include "../cli/cli.h"

static inline int min(int x, int y) { return x < y ? x : y; }
static inline int max(int x, int y) { return x < y ? y : x; }

mpz_t zero;
bdoag_memo memo;
int bound = 2; // TODO: make this a cmd line argument.
int M;

// Alias of type __sampler_t of the sampling function for use in the generic
// sampler.
randdag_t bdoag_sampler(gmp_randstate_t state) {
  return bdoag_unif_m(state, memo, M, bound);
}

// Alias of type __counter_t of the counting function for use in the generic
// counter.
mpz_t* bdoag_counter(int n, int m) {
  const int C = min(bound, n - 1);
  if (m <= (C + 1) * C / 2 +  (n - 1 - C) * bound)
    return bdoag_count(memo, n, m, 1, bound);
  else
    return &zero;
}

int main(int argc, char* argv[]) {
  randdag_cli_options opts = randdag_cli_parse(30, argc, argv);
  M = opts.count;

  // Alloc memo and load

  if (opts.load_file) {
    FILE* fd = fopen(opts.load_file, "r");
    if (fd != NULL) {
      int N, M2;
      fscanf(fd, "%d %d %d\n", &N, &M2, &bound);

      M = max(M, M2);
      N = max(M + 1, N);
      memo = bdoag_memo_alloc(N, M, bound);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.load_file);
      return 1;
    }
  } else {
    memo = bdoag_memo_alloc(M + 1, M, bound);
  }

  // Counting
  generic_counter(bdoag_counter, M);

  // Dump

  if (opts.dump_file) {
    FILE* fd = fopen(opts.dump_file, "w");
    if (fd) {
      bdoag_memo_dump(fd, memo);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.dump_file);
      return 1;
    }
  }

  // Random sampling
  if (opts.sample_file != NULL) {
    int r = generic_sampler(opts.sample_file, bdoag_sampler);
    if (r != 0) return r;
  }

  // Some cleaning
  bdoag_memo_free(memo);

  return 0;
}
