#include "utils.h" /* Boring command line parsing stuff. */
#include <gmp.h>
#include <stdio.h>

#include "../includes/doag.h"

const char *usage_string = "USAGE: %s N\nSamples a DOAG of size N.\n";

/* Command line parsing */
static int parse_args(int argc, char **argv, int *n) {
  char *progname;

  if ((progname = next_argument(&argc, &argv)) == NULL)
    return 1;

  if ((*n = parse_int(usage_string, progname, &argc, &argv)) == -1)
    return 1;

  if (argc != 0) {
    fprintf(stderr, "Too many arguments\n");
    fprintf(stderr, usage_string, progname);
    return 1;
  }

  return 0;
}

int main(int argc, char **argv) {
  int n;
  gmp_randstate_t prng;

  /* Get the number of vertices from the command line */
  if (parse_args(argc, argv, &n) != 0)
    return 1;

  /* Don't forget to initialise the PRNG */
  gmp_randinit_default(prng);
  gmp_randseed_ui(prng, 0xdeadbeef);

  /* The interesting part is here. */
  {
    /* Generate a uniform DOAG with n vertices. */
    randdag_t doag = doag_unif_n(prng, n);

    /* Print the graph in dot format to stdout.
     * Since the ordering of the nodes matters in DOAGs, the RD_DOT_ORDERING
     * flag must be passed to tell graphviz to respect our ordering when
     * printing the graph. */
    randdag_to_dot(stdout, doag, RD_DOT_ORDERING);

    /* Free up all the space taken by the doag and GMP's prng. */
    randdag_free(doag);
    gmp_randclear(prng);
  }

  return 0;
}
