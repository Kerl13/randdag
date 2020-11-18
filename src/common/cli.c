#include <string.h>
#include <gmp.h>
#include <sys/random.h>

#include "../../includes/common.h"

int generic_sampler(const char* filename, __sampler_t sampler) {
  // Setup output file
  FILE* fd = stdout;
  if (strcmp("-", filename) != 0)
    fd = fopen(filename, "w");
  if (fd == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", filename);
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
  randdag_t g = sampler(state);
  randdag_to_dot(fd, g);
  fflush(fd);

  // Do some cleanups
  if (fd != stdout) fclose(fd);
  randdag_free(g);
  gmp_randclear(state);

  return 0;
}

void generic_counter(__counter_t count, int M) {
  mpz_t x;
  mpz_init(x);

  for (int m = 1; m <= M; m++) {
    mpz_set_ui(x, 0);
    for (int n = 2; n <= m + 1; n++) {
      mpz_add(x, x, *count(n, m));
    }
    printf("%d: ", m);
    mpz_out_str(stdout, 10, x);
    printf("\n");
    fflush(stdout);
  }

  mpz_clear(x);
}
