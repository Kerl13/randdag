#include <stdio.h>

#include "../../includes/doag.h"
#include <gmp.h>

/* Count all DOAGs with:
 * - n vertices;
 * - m edges;
 * - unbounded out-degree;
 * - and any number of sources. */
static void count_any_k(memo_t memo, mpz_t res, int n, int m) {
  int k;
  mpz_set_ui(res, 0);
  for (k = 0; k <= n; ++k)
    mpz_add(res, res, *doag_count(memo, n, m, k, -1 /* unbounded */));
}

static int one_test(memo_t memo, mpz_t x, int n, int m, int expected) {
  count_any_k(memo, x, n, m);
  if (mpz_cmp_ui(x, expected) != 0) {
    fprintf(stderr, "[ERROR] doag_count(<memo>, %d, %d, *, 0) returned ", n, m);
    mpz_out_str(stderr, 10, x);
    fprintf(stderr, " instead of %d\n", expected);
    return 1;
  }
  return 0;
}

static int small_cases() {
  int error;
  mpz_t x;
  memo_t memo;

  memo = memo_alloc(6, 15, 0);
  mpz_init(x);

  /* Test the counting function on known small values. */
  error = 0;

  error |= one_test(memo, x, /* n= */ 1, /* m= */ 0, /* expected= */ 1);

  error |= one_test(memo, x, /* n= */ 2, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 2, /* m= */ 1, /* expected= */ 1);

  error |= one_test(memo, x, /* n= */ 3, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 1, /* expected= */ 2);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 2, /* expected= */ 3);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 3, /* expected= */ 2);

  error |= one_test(memo, x, /* n= */ 4, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 1, /* expected= */ 3);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 2, /* expected= */ 8);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 3, /* expected= */ 17);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 4, /* expected= */ 27);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 5, /* expected= */ 27);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 6, /* expected= */ 12);

  error |= one_test(memo, x, /* n= */ 5, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 1, /* expected= */ 4);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 2, /* expected= */ 15);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 3, /* expected= */ 48);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 4, /* expected= */ 139);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 5, /* expected= */ 349);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 6, /* expected= */ 718);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 7, /* expected= */ 1136);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 8, /* expected= */ 1272);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 9, /* expected= */ 888);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 10, /* expected= */ 288);

  error |= one_test(memo, x, /* n= */ 6, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 1, /* expected= */ 5);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 2, /* expected= */ 24);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 3, /* expected= */ 100);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 4, /* expected= */ 391);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 5, /* expected= */ 1434);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 6, /* expected= */ 4868);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 7, /* expected= */ 14940);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 8, /* expected= */ 40261);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 9, /* expected= */ 92493);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 10, /* expected= */ 175738);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 11, /* expected= */ 266898);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 12, /* expected= */ 310096);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 13, /* expected= */ 258120);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 14, /* expected= */ 136800);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 15, /* expected= */ 34560);

  memo_free(memo);
  mpz_clear(x);
  return error;
}

int main() {
  int error;
  error = small_cases();
  fprintf(stderr, "TEST small cases: %s\n", error ? "FAILED" : "OK");
  return error;
}
