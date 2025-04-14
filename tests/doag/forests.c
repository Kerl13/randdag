#include <inttypes.h>
#include <stdio.h>

#include "../../includes/doag.h"
#include <gmp.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

/* Count all DOAGs of size n with out-degree bounded by 1;
 * equivalently, unordered forests of rooted trees endowed with a total order on
 * the leaves;
 * equivalently (non-trivial), set partitions. */
static void count_unary(memo_t memo, mpz_t res, int n) {
  int m, k;

  mpz_set_ui(res, 0);
  for (k = 0; k <= n; ++k) {
    const int C = min(n - k, 1);
    for (m = n - k; m <= C * (n - C); m++) {
      mpz_add(res, res, *doag_count(memo, n, m, k, 1));
    }
  }
}

static int one_test(memo_t memo, mpz_t x, int n, int64_t expected) {
  count_unary(memo, x, n);
  if (mpz_cmp_ui(x, expected) != 0) {
    fprintf(stderr, "[ERROR] doag_count(<memo>, %d, *, *, 1) returned ", n);
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
  int64_t expected[] = {1,
                        1,
                        2,
                        5,
                        15,
                        52,
                        203,
                        877,
                        4140,
                        21147,
                        115975,
                        678570,
                        4213597,
                        27644437,
                        190899322,
                        1382958545,
                        10480142147,
                        82864869804,
                        682076806159,
                        5832742205057,
                        51724158235372,
                        474869816156751,
                        4506715738447323,
                        44152005855084346,
                        445958869294805289,
                        4638590332229999353};

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
  fprintf(stderr, "TEST forests: %s\n", error ? "FAILED" : "OK");
  return error;
}
