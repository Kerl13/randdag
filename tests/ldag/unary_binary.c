#include <inttypes.h>
#include <stdio.h>

#include "../../includes/ldag.h"
#include <gmp.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

/* Count all DOAGs of size n with 1 source and out-degree bounded by 2. */
static void count_unary_binary(memo_t memo, mpz_t res, int n) {
  int m;
  const int C = min(n - 1, 2);

  mpz_set_ui(res, 0);
  for (m = n - 1; m <= C * (C - 1) / 2 + C * (n - C); m++) {
    mpz_add(res, res, *ldag_count(memo, n, m, 1, 2));
  }
}

static int one_test(memo_t memo, mpz_t x, int n, int64_t expected) {
  count_unary_binary(memo, x, n);
  if (mpz_cmp_ui(x, expected) != 0) {
    fprintf(stderr, "[ERROR] ldag_count(<memo>, %d, *, 1, 2) returned ", n);
    mpz_out_str(stderr, 10, x);
    fprintf(stderr, " instead of %" PRId64 "\n", expected);
    return 1;
  }
  return 0;
}

static int small_cases() {
  int error, n, N;
  mpz_t x;
  memo_t memo;
  int64_t expected[] = {0,
                        1,
                        2,
                        15,
                        216,
                        5310,
                        201060,
                        10941840,
                        813405600,
                        79456862520,
                        9892857920400,
                        1531852145069400,
                        289108839905424000};

  N = sizeof(expected) / sizeof(int64_t) - 1;
  memo = memo_alloc(N, -1, -1);
  mpz_init(x);

  error = 0;

  for (n = 0; n <= N; n++) {
    error |= one_test(memo, x, n, expected[n]);
  }

  memo_free(memo);
  mpz_clear(x);
  return error;
}

int main() {
  int error;
  error = small_cases();
  fprintf(stderr, "TEST 1-source unary-binary: %s\n", error ? "FAILED" : "OK");
  return error;
}
