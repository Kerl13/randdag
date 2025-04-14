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
 * NB. utils.c contains the boilerplate command line parsing code for our
 * examples so that this file contains only the relevant part of the
 * demonstration on how to use the libraries.
 */

const char *usage_string = "USAGE: %s N M K\nComputes the number of DOAGs with "
                           "N vertices, M edges, and K sources.\n";

int main(int argc, char **argv) {
  int n, m, k;
  memo_t memo;
  mpz_t *res;

  /* Get the size parameters from the command line */
  if (parse_three_ints(usage_string, argc, argv, &n, &m, &k) != 0)
    return 1;

  /* Allocate the memoisation table. */
  memo = memo_alloc(n, m, -1);

  /* Count! */
  res = doag_count(memo, n, m, k, -1);

  /* Print the result to stdout */
  printf("The number of DOAGs with %d vertices, %d edges, and %d sources is: ",
         n, m, k);
  mpz_out_str(stdout, 10, *res);
  printf("\n");

  /* Free up the space occupied by the memoisation table */
  memo_free(memo);

  return 0;
}
