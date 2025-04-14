#include <stdio.h>

#include "../../includes/ldag.h"
#include <gmp.h>

/* Count all labelled DAGs with:
 * - n vertices;
 * - m edges;
 * - unbounded out-degree;
 * - and any number of sources. */
static void count_any_k(memo_t memo, mpz_t res, int n, int m) {
  int k;
  mpz_set_ui(res, 0);
  for (k = 0; k <= n; ++k)
    mpz_add(res, res, *ldag_count(memo, n, m, k, -1 /* unbounded */));
}

static int one_test(memo_t memo, mpz_t x, int n, int m, int expected) {
  count_any_k(memo, x, n, m);

  if (mpz_cmp_ui(x, expected) != 0) {
    fprintf(stderr, "[ERROR] ldag_count(<memo>, %d, %d, *, -1) returned ", n,
            m);
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

  memo = memo_alloc(6, 15, -1);
  mpz_init(x);

  /* Test the counting function on known small values. */
  error = 0;

  error |= one_test(memo, x, /* n= */ 1, /* m= */ 0, /* expected= */ 1);

  error |= one_test(memo, x, /* n= */ 2, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 2, /* m= */ 1, /* expected= */ 2);

  error |= one_test(memo, x, /* n= */ 3, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 1, /* expected= */ 6);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 2, /* expected= */ 12);
  error |= one_test(memo, x, /* n= */ 3, /* m= */ 3, /* expected= */ 6);

  error |= one_test(memo, x, /* n= */ 4, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 1, /* expected= */ 12);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 2, /* expected= */ 60);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 3, /* expected= */ 152);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 4, /* expected= */ 186);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 5, /* expected= */ 108);
  error |= one_test(memo, x, /* n= */ 4, /* m= */ 6, /* expected= */ 24);

  error |= one_test(memo, x, /* n= */ 5, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 1, /* expected= */ 20);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 2, /* expected= */ 180);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 3, /* expected= */ 940);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 4, /* expected= */ 3050);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 5, /* expected= */ 6180);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 6, /* expected= */ 7960);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 7, /* expected= */ 6540);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 8, /* expected= */ 3330);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 9, /* expected= */ 960);
  error |= one_test(memo, x, /* n= */ 5, /* m= */ 10, /* expected= */ 120);

  error |= one_test(memo, x, /* n= */ 6, /* m= */ 0, /* expected= */ 1);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 1, /* expected= */ 30);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 2, /* expected= */ 420);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 3, /* expected= */ 3600);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 4, /* expected= */ 20790);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 5, /* expected= */ 83952);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 6, /* expected= */ 240480);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 7, /* expected= */ 496680);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 8, /* expected= */ 750810);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 9, /* expected= */ 838130);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 10, /* expected= */ 691020);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 11, /* expected= */ 416160);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 12, /* expected= */ 178230);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 13, /* expected= */ 51480);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 14, /* expected= */ 9000);
  error |= one_test(memo, x, /* n= */ 6, /* m= */ 15, /* expected= */ 720);

  memo_free(memo);
  mpz_clear(x);
  return error;
}

int main() {
  int error;
  error = small_cases();
  fprintf(stderr, "TEST (n,m)-small cases: %s\n", error ? "FAILED" : "OK");
  return error;
}
