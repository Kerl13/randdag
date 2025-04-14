#include <inttypes.h>
#include <stdio.h>

#include "../../includes/ldag.h"
#include <gmp.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

/* Count all labelled DAGs of size n with out-degree bounded by 1;
 * equivalently, unordered labelled forests of rooted trees;
 * equivalently, spanning forests of the complete graph of size n;
 * equivalently, spanning trees of the complete graph of size (n+1). */
static void count_unary(memo_t memo, mpz_t res, int n) {
  int m, k;

  mpz_set_ui(res, 0);
  for (k = 0; k <= n; ++k) {
    const int C = min(n - k, 1);
    for (m = n - k; m <= C * (n - C); m++) {
      mpz_add(res, res, *ldag_count(memo, n, m, k, 1));
    }
  }
}

static int one_test(memo_t memo, mpz_t x, int n, int64_t expected) {
  count_unary(memo, x, n);
  if (mpz_cmp_ui(x, expected) != 0) {
    fprintf(stderr, "[ERROR] ldag_count(<memo>, %d, *, *, 1) returned ", n);
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
                        3,
                        16,
                        125,
                        1296,
                        16807,
                        262144,
                        4782969,
                        100000000,
                        2357947691,
                        61917364224,
                        1792160394037,
                        56693912375296,
                        1946195068359375,
                        72057594037927936,
                        2862423051509815793};

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
