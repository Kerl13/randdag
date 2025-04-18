#include <stdio.h>

#include "utils.h"
#include <gmp.h>

#include "../includes/doag.h"

/*
 * This file demonstrates how to sample uniform DOAGs with n vertices using
 * libdoag's doag_unif_n function.
 * The algorithm used in this function does not require any pre-processing.
 *
 * Compile with: utils.c -ldoag -lgmp
 *
 * Example: running `doag_n.exe 10 > doag.dot` from the command line will
 * generate a uniform random DOAG with 10 vertices and store it to `doag.dot`
 * (in graphviz' dot format).
 * You can visualise the result by running `dot -Tpdf doag.dot -o doag.pdf`.
 *
 * NB. utils.c contains the boilerplate command line parsing code for our
 * examples so that this file contains only the relevant part of the
 * demonstration on how to use the libraries.
 */

const char *usage_string = "USAGE: %s N\nSamples a DOAG of size N.\n";

int main(int argc, char **argv) {
  int n;
  gmp_randstate_t prng;
  randdag_t doag;

  /* Get the number of vertices from the command line */
  if (parse_one_int(usage_string, argc, argv, &n) != 0)
    return 1;

  /* Initialise the PRNG */
  gmp_randinit_default(prng);
  gmp_randseed_ui(prng, 0xdeadbeef);

  /* Generate a uniform DOAG with n vertices. */
  doag = doag_unif_n(prng, n);

  /* Print the graph in dot format to stdout.
   * Since the ordering of the nodes matters in DOAGs, the RD_DOT_ORDERING
   * flag must be passed to tell graphviz to respect our ordering when
   * printing the graph. */
  randdag_to_dot(stdout, doag, RD_DOT_ORDERING);

  /* Free up all the space taken by the doag and GMP's prng. */
  randdag_free(doag);
  gmp_randclear(prng);

  return 0;
}
