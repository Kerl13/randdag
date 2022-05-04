#include <errno.h>  /* errno */
#include <limits.h> /* INT_MAX */
#include <stdio.h>  /* fprintf, stderr */
#include <stdlib.h> /* strtol */

#include "utils.h"

char *next_argument(int *argc, char ***argv) {
  char *res;

  if (*argc <= 0)
    return NULL;

  res = **argv;
  *argc = *argc - 1;
  *argv = *argv + 1;
  return res;
}

int parse_int(const char *usage_string, const char *progname, int *argc,
              char ***argv) {
  long int res;
  char *end;
  char *next_arg;

  /* Consume one command line argument */
  if ((next_arg = next_argument(argc, argv)) == NULL) {
    fprintf(stderr, "Not enough arguments\n");
    fprintf(stderr, usage_string, progname);
    return -1;
  }

  /* Try to parse it */
  errno = 0;
  res = strtol(next_arg, &end, 10);

  /* Handle the possible error cases */
  if (errno != 0 || end == next_arg || *end != '\0' || res <= 0 ||
      res > INT_MAX) {
    fprintf(stderr, "Invalid argument: `%s`.\n", next_arg);
    fprintf(stderr, "It should be an integer and belong to [1; %d]\n", INT_MAX);
    fprintf(stderr, usage_string, progname);
    return -1;
  }

  /* At this point it is safe to cast to int */
  return (int)res;
}

int parse_one_int(const char *usage_string, int argc, char **argv, int *n) {
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
