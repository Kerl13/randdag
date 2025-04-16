#include <stdio.h>

#include "utils.h"
#include <gmp.h>

#include "../includes/doag.h"

/*
 * This file demonstrates how to sample uniform DOAGs with n vertices, m edges,
 * bounded out-degree and exactly one source using libdoag's doag_unif_nmk
 * function.
 *
 * Compile with: utils.c -ldoag -lgmp
 *
 * Example: running `random_doag_nm1.exe 100 150 2 > doag.dot` from the command
 * line will generate a uniform unary-binary random DOAG with 100 vertices, 150
 * edges, out-degree bounded by 2 and store it to `doag.dot` (in graphviz' dot
 * format). The result can be visualised the result by running `dot -Tpdf
 * doag.dot -o doag.pdf`.
 *
 * NB. utils.c contains the boilerplate command line parsing code for our
 * examples so that this file contains only the relevant part of the
 * demonstration on how to use the libraries.
 */

const char *usage_string =
    "USAGE: %s N M NB\nSamples NB DOAGs of size (N, M) "
    "with one source and out degree bounded by 2.\n"
    "The NB graphs are stored in files named doag_0.dot, doag_1.dot, etc.\n";

int main(int argc, char *argv[]) {
  int n, m, nb;
  gmp_randstate_t prng;
  randdag_t doag;
  memo_t memo;

  /* Get the parameters from the command line */
  if (parse_three_ints(usage_string, argc, argv, &n, &m, &nb) != 0)
    return 1;

  /* Initialise the memoisation table. */
  memo = memo_alloc(n, m, 2);

  /* Initialise the PRNG */
  gmp_randinit_default(prng);
  gmp_randseed_ui(prng, 0xdeadbeef);

  for (; nb > 0; nb--) {
    char filename[32]; /* Large enough to be sure there is no overflow. */
    FILE *fd;

    /* Generate a uniform DOAG with n vertices. */
    doag = doag_unif_nmk(prng, memo, n, m, 1, 2);

    /* Open the output file. */
    sprintf(filename, "doag_%d.dot", nb);
    if (!(fd = fopen(filename, "w"))) {
      perror("fopen");
      fprintf(stderr, "Error while opening file: %s:\n", filename);
      return 2;
    }

    /* Print the graph in dot format to fd.
     * Since the ordering of the nodes matters in DOAGs, the RD_DOT_ORDERING
     * flag must be passed to tell graphviz to respect our ordering when
     * printing the graph. */
    randdag_to_dot(fd, doag, RD_DOT_ORDERING);

    fclose(fd);
  }

  /* Do some cleanups. */
  randdag_free(doag);
  gmp_randclear(prng);

  return 0;
}
