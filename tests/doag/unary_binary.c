#include <inttypes.h>
#include <stdio.h>

#include "../../includes/doag.h"
#include <gmp.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

/* Count all DOAGs of size n with 1 source and out-degree bounded by 2. */
static void count_unary_binary(memo_t memo, mpz_t res, int n) {
  int m;
  const int C = min(n - 1, 2);

  mpz_set_ui(res, 0);
  for (m = n - 1; m <= C * (C - 1) / 2 + C * (n - C); m++) {
    mpz_add(res, res, *doag_count(memo, n, m, 1, 2));
  }
}

static int one_test(memo_t memo, mpz_t x, int n, int64_t expected) {
  count_unary_binary(memo, x, n);
  if (mpz_cmp_si(x, expected) != 0) {
    fprintf(stderr, "[ERROR] doag_count(<memo>, %d, *, 1, 2) returned ", n);
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
                        1,
                        4,
                        23,
                        191,
                        2106,
                        29294,
                        495475,
                        9915483,
                        229898277,
                        6074257926,
                        180460867600,
                        5962588299084,
                        217102236529348,
                        8643663093911458,
                        373804738302509267};

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
