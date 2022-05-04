#include "utils.h"
#include <stdio.h>

#include <gmp.h>

#include "../includes/doag.h"

/*
 * This file demonstrates how to use libdoag to compute the number of DOAGs with
 * a given number of vertices, edges, and sources.
 * The counting algorithm uses memoisation. This means that some of the
 * intermediate computations are stored in a table and can be reused in later
 * calls to the counting function.
 *
 * Compile with: utils.c -ldoag -lgmp
 *
 * Example: running `doag_n.exe 10 30 1` from the command line will
 * print:
 * - the number of DOAG with 10 vertices, 30 edges, and 1 source (the source is
 *   part of the 10 vertices);
 * - some other relevant values.
 *
 * NB. utils.c handles boring command line parsing stuff and is only here to
 * allow you to play the executable. You should drop it and replace the
 * `parse_three_ints` call below with something more appropriate in your own
 * code.
 */

const char *usage_string = "USAGE: %s N M K\nComputes the number of DOAGs with "
                           "N vertices, M edges, and K sources.\n";

int main(int argc, char **argv) {
  int n, m, k;

  /* Get the size parameters from the command line */
  if (parse_three_ints(usage_string, argc, argv, &n, &m, &k) != 0)
    return 1;

  /* We check here the condition under which a DOAG with n vertices, m edges,
   * and k sources exists. */
  if (n != 1 && k >= n) {
    printf("For n > 1, there must be less sources than vertices.\n");
  } else if (n - 1 > m) {
    printf("If m < n - 1, there are not enough edges for the graph to be "
           "connected.\n");
  } else {
    const int upper_bound = k * (n - k) + (n - k) * (n - k - 1) / 2;
    if (m > upper_bound) {
      printf(
          "The number of edges of a DOAG with n sources and k sources cannot "
          "exceed k * (n - k) + choose(n - k, 2) (= %d here).\n",
          upper_bound);
    } else {
      /* Here we are sure that there exists a DOAG with the given parameters.
       * Note that it is important to ensure that this is the case, otherwise
       * the behaviour of doag_count is undefined. */
      int M, N;
      mpz_t *res, tmp;

      /* Allocate a big enough memoisation table.
       * FIXME: explain the purpose of this third parameter. */
      memo_t memo = memo_alloc(n, m, 0);

      /* Count! */
      res = doag_count(memo, n, m, k, 0);

      /* Print the result to stdout */
      printf(
          "The number of DOAGs with %d vertices, %d edges, and %d sources is: ",
          n, m, k);
      mpz_out_str(stdout, 10, *res);
      printf("\n");

      /* Print some other interesting values */
      M = m;
      N = n;
      mpz_init(tmp);
      printf("Here is the total number of DOAGs with m vertices, 1 source "
             "only, and any number of vertices, for some "
             "values of m:\n");
      for (m = 1; m <= M && m <= N - 1 && m <= 50; m++) {
        mpz_set_ui(tmp, 0);
        for (n = 2; n <= m + 1; n++) {
          if (m <= n * (n - 1) / 2)
            mpz_add(tmp, tmp, *doag_count(memo, n, m, 1, 0));
        }
        printf("%d: ", m);
        mpz_out_str(stdout, 10, tmp);
        fflush(stdout);
        printf("\n");
      }
      mpz_clear(tmp);

      /* Free up the space occupied by the memoisation table */
      memo_free(memo);
    }
  }

  return 0;
}
