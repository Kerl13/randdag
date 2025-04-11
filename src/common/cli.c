/* Randdag: C library for the uniform random generation of DAGs

   Copyright (C) 2020  Martin Pépin, Antoine Genitrini and Alfredo Viola

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
/* Keep #include <stdio.h> above #include <gmp.h> as suggested by
   https://gmplib.org/manual/I_002fO-of-Integers:

   > When using any of these functions, it is a good idea to include stdio.h
   > before gmp.h, since that will allow gmp.h to define prototypes for these
   > functions. */
#include <gmp.h>        /* mpz_* */
#include <limits.h>     /* INT_MAX */
#include <malloc.h>     /* malloc, realloc */
#include <stdlib.h>     /* strtol, exit */
#include <string.h>     /* strcmp */
#include <sys/random.h> /* getrandom (linux only) */

#include "../../lib/argtable3/argtable3.h"
#include "cli.h"

#define max(x, y) (((x) < (y)) ? (y) : (x))
#define min(x, y) (((x) < (y)) ? (x) : (y))

/* Utility function (getline in not available on non-gnu systems) */

static char *mygetline(size_t *n, FILE *stream) {
  size_t capacity;
  char *buf;
  char c;

  capacity = 32;
  *n = 0;
  buf = malloc(capacity * sizeof(char));

  while (1) {
    if (*n == capacity - 1) {
      capacity = capacity * 2;
      buf = realloc(buf, capacity * sizeof(char));
    }

    c = getc(stream);
    switch (c) {
    case EOF:
      buf[*n] = '\0';
      (*n)++;
      return buf;
    case '\n':
      buf[*n] = '\n';
      buf[*n + 1] = '\0';
      (*n) += 2;
      return buf;
    default:
      buf[*n] = c;
      (*n)++;
    }
  }
}

/* Generic commands */

static int generic_sampler(const char *filename, memo_t memo,
                           __sampler_t sampler, long flags, int M) {
  FILE *ofile;
  gmp_randstate_t state;
  unsigned long int seed;
  randdag_t g;

  /* FIXME: rewrite this to handle two arguments: N and M */
  return EXIT_FAILURE;

  /* Setup output file */
  ofile = (strcmp("-", filename) == 0) ? stdout : fopen(filename, "w");
  if (ofile == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", filename);
    return EXIT_FAILURE;
  }

  /* Prepare RNG */
  gmp_randinit_mt(state);
  getrandom(&seed, sizeof(unsigned long int), 0); /* XXX. Linux only */
  printf("Using random seed 0x%lx\n", seed);
  gmp_randseed_ui(state, seed);

  /* Sample */
  g = sampler(state, memo, M);
  randdag_to_dot(ofile, g, flags);

  /* Do some cleanups */
  if (ofile != stdout)
    fclose(ofile);
  randdag_free(g);
  gmp_randclear(state);

  return EXIT_SUCCESS;
}

static void generic_counter(__counter_t count, memo_t memo, int N, int M,
                            int bound) {
  int n, m, k;
  mpz_t sum;

  mpz_init(sum);

  /* Head line */
  printf("Graphs with n vertices");
  if (M >= 0)
    printf(", at most %d edges", M);
  if (bound >= 0)
    printf(", and out-degree bounded by %d", bound);
  printf(":\n");
  for (n = 0; n <= N; n++) {
    mpz_set_ui(sum, 0);
    for (k = (n > 0); k <= n; k++) {
      const int C = min(n - k, (bound < 0 ? n : bound));
      for (m = n - k; m <= min((C * (C - 1)) / 2 + (n - C) * C, M); m++) {
        mpz_add(sum, sum, *count(memo, n, m, k, bound));
      }
    }

    printf("n=%-6d ", n);
    mpz_out_str(stdout, 10, sum);
    printf("\n");
    fflush(stdout);
  }

  mpz_clear(sum);
}

/* Command line parsing */

typedef struct cli_options {
  int N, M, bound, count;
  const char *sample_file;
  const char *dump_file;
  const char *load_file;
} cli_options;

/* FIXME: these should be local variables. */
struct arg_lit *help, *count;
struct arg_int *arg_N, *arg_M, *arg_B;
struct arg_file *sample, *dump, *load;
struct arg_end *end;

static int cli_parse(int argc, char *argv[], cli_options *opts) {
  int exitcode, nerrors;
  void *argtable[9];

  argtable[0] = help =
      arg_litn("h", "help", 0, 1, "Display this help and exit.");

  /* Size parameters. */
  argtable[1] = arg_N =
      arg_intn("n", "vertices", "<N>", 0, 1,
               "Set the maximum (resp. exact) number of vertices for "
               "counting (resp. sampling). Defaults to 10.");
  argtable[2] = arg_M =
      arg_intn("m", "edges", "<M>", 0, 1,
               "Set the maximum (resp. exact) number of edges for counting "
               "(resp. sampling). Negative means unbounded. Defaults to -1.");
  argtable[3] = arg_B = arg_intn(
      "b", "bound", "<B>", 0, 1,
      "Set an upper bound on the out-degree of the graphs for counting and "
      "sampling. Negative means unbounded. Defaults to -1.");

  /* Counting and sampling. */
  argtable[4] = count = arg_litn(
      "c", "count", 0, 1, "count DAGs with up to N vertices and M edges");
  argtable[5] = sample =
      arg_filen("s", "sample", "<file>", 0, 1,
                "write a uniform DAG with N vertices to <file>");

  /* Memoisation table management. */
  argtable[6] = dump =
      arg_filen("d", "dump", "<file>", 0, 1, "dump counting info to <file>");
  argtable[7] = load =
      arg_filen("l", "load", "<file>", 0, 1, "load counting info from <file>");

  argtable[8] = end = arg_end(10);

  exitcode = EXIT_SUCCESS;
  nerrors = arg_parse(argc, argv, argtable);

  /* Special case for the --help option. */
  if (help->count > 0) {
    printf("usage: %s", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-20s %s\n");
    goto exit;
  }

  /* Error management. */
  if (nerrors > 0) {
    arg_print_errors(stderr, end, argv[0]);
    fprintf(stderr, "try: %s --help for more information.\n", argv[0]);
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  /* Integers sanitisation. */
  opts->N = (arg_N->count > 0) ? arg_N->ival[0] : 10;
  if (opts->N < 0) {
    fprintf(stderr, "[-n|--vertices] expects a non-negative integer.\n");
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  opts->M = (arg_M->count > 0) ? arg_M->ival[0] : (opts->N * (opts->N - 1)) / 2;
  if (opts->M < 0) {
    fprintf(stderr, "[-m|--edges] expects a non-negative integer.\n");
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  opts->bound = (arg_B->count > 0) ? arg_B->ival[0] : -1;

  /* Store the other flags and filenames. */
  opts->count = (count->count > 0);
  opts->sample_file = (sample->count > 0) ? sample->filename[0] : NULL;
  opts->dump_file = (dump->count > 0) ? dump->filename[0] : NULL;
  opts->load_file = (load->count > 0) ? load->filename[0] : NULL;

exit:
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return exitcode;
}

/* Generic command line interface */

int run_cli(int argc, char *argv[], __counter_t counter, __sampler_t sampler,
            long flags) {

  int exitcode;
  cli_options opts = {0};
  memo_t memo;

  if ((exitcode = cli_parse(argc, argv, &opts)) != EXIT_SUCCESS)
    return exitcode;

  /* Load a pre-existing dump or allocate a fresh one. */
  if (opts.load_file) {
    /* FIXME: maybe the logic in this function should be in memo_load? */

    int file_N, file_M, file_bound, r;
    char *line;
    size_t len;

    /* File management. */
    FILE *fd = fopen(opts.load_file, "r");
    if (!fd) {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.load_file);
      return 1;
    }

    /* Parse of the dump file's header. */
    line = mygetline(&len, fd);
    r = sscanf(line, "%d %d %d\n", &file_N, &file_M, &file_bound);
    if (r < 3) {
      file_bound = file_N;
    }

    /* Allocate enough space for our  */
    memo = memo_alloc(max(opts.N, file_N), max(opts.M, file_M),
                      opts.bound < 0 ? max(opts.N, file_N)
                                     : max(opts.bound, file_bound));

    /* Parse the rest of the file. */
    memo_load(memo, fd);

    fclose(fd);
  } else {
    memo = memo_alloc(opts.N, opts.M, opts.bound < 0 ? opts.N : opts.bound);
  }

  /* Count. */
  generic_counter(counter, memo, opts.N, opts.M, opts.bound);

  /* Dump the memoisation table if asked to. */
  if (opts.dump_file) {
    FILE *fd = fopen(opts.dump_file, "w");
    if (fd == NULL) {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.dump_file);
      return 1;
    }
    memo_dump(fd, memo);
    return 0;
  }

  if (opts.sample_file) {
    generic_sampler(opts.sample_file, memo, sampler, flags, opts.N);
  }

  return 0;
}
