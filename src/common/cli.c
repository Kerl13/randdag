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

#include <malloc.h>     /* malloc, realloc */
#include <string.h>     /* strcmp */
#include <gmp.h>        /* mpz_* */
#include <sys/random.h> /* getrandom (linux only) */
#include <stdlib.h>     /* strtol, exit */
#include <limits.h>     /* INT_MAX */

#include "../../lib/argtable3/argtable3.h"
#include "cli.h"

#define max(x, y) (((x) < (y)) ? (y) : (x))

/* Utility function (getline in not available on non-gnu systems) */

static char* mygetline(size_t* n, FILE* stream) {
  size_t capacity;
  char* buf;
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

static int generic_sampler(const char* filename, memo_t memo, __sampler_t sampler, long flags, int M) {
  FILE *ofile;
  gmp_randstate_t state;
  unsigned long int seed;
  randdag_t g;
  int exitcode;

  exitcode = EXIT_SUCCESS;

  /* Setup output file */
  ofile = (strcmp("-", filename) == 0) ? stdout : fopen(filename, "w");
  if (ofile == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", filename);
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  /* Prepare RNG */
  gmp_randinit_mt(state);
  getrandom(&seed, sizeof(unsigned long int), 0); /* XXX. Linux only */
  printf("Using random seed 0x%lx\n", seed);
  gmp_randseed_ui(state, seed);

  /* Sample */
  g = sampler(state, memo, M);
  randdag_to_dot(ofile, g, flags);

exit:
  /* Do some cleanups */
  if (ofile != stdout) fclose(ofile);
  randdag_free(g);
  gmp_randclear(state);

  return exitcode;
}

static void generic_counter(__counter_t count, memo_t memo, int M) {
  int n, m;
  mpz_t x;
  mpz_init(x);

  for (m = 1; m <= M; m++) {
    mpz_set_ui(x, 0);
    for (n = 2; n <= m + 1; n++) {
      mpz_add(x, x, *count(memo, n, m));
    }
    printf("%d: ", m);
    mpz_out_str(stdout, 10, x);
    printf("\n");
    fflush(stdout);
  }

  mpz_clear(x);
}

/* Command line parsing */

typedef struct cli_options {
  int count;
  const char* sample_file;
  const char* dump_file;
  const char* load_file;
} cli_options;

struct arg_lit *help;
struct arg_int *count;
struct arg_file *sample, *dump, *load;
struct arg_end *end;

static int cli_parse(int argc, char* argv[], cli_options *opts) {
  int exitcode, nerrors;
  void* argtable[6];

  argtable[0] = help = arg_litn("h", "help", 0, 1, "display this help and exit");
  argtable[1] = count = arg_intn("c", "count", "<M>", 0, 1, "count DAGs with up to M edges, defaults to 30");
  argtable[2] = sample = arg_filen("s", "sample", "<file>", 0, 1, "write a uniform DAG with M edges to <file>");
  argtable[3] = dump = arg_filen("d", "dump", "<file>", 0, 1, "dump counting info to <file>");
  argtable[4] = load = arg_filen("l", "load", "<file>", 0, 1, "load counting info from <file>");
  argtable[5] = end = arg_end(10);

  exitcode = EXIT_SUCCESS;
  nerrors = arg_parse(argc, argv, argtable);

  if (help->count > 0) {
    printf("usage: %s", argv[0]);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-20s %s\n");
    goto exit;
  }

  if (nerrors > 0) {
    arg_print_errors(stderr, end, argv[0]);
    fprintf(stderr, "try: %s --help for more information.\n", argv[0]);
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  opts->count = (count->count > 0) ? count->ival[0] : 30;
  if (opts->count <= 0) {
    fprintf(stderr, "--count expects a positive integer.\n");
    exitcode = EXIT_FAILURE;
    goto exit;
  }

  opts->sample_file = (sample->count > 0) ? sample->filename[0] : NULL;
  opts->dump_file = (dump->count > 0) ? dump->filename[0] : NULL;
  opts->load_file = (load->count > 0) ? load->filename[0] : NULL;

exit:
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return exitcode;
}

/* Generic command line interface */

int run_cli(int argc, char* argv[],
            __counter_t counter, __sampler_t sampler, long flags) {

  cli_options opts = {0, 0, 0, 0};
  int M;
  memo_t memo;

  cli_parse(argc, argv, &opts);
  M = opts.count;

  /* Load a pre-existing dump or allocate a fresh one. */
  if (opts.load_file) {
    FILE* fd = fopen(opts.load_file, "r");
    if (fd) {
      /* Parse the dump header. */
      int N, M2, bound, r;
      char* line;
      size_t len;
      line = mygetline(&len, fd);
      r = sscanf(line, "%d %d %d\n", &N, &M2, &bound);

      M2 = max(M, M2);
      N = max(M + 1, N);
      if (r < 3) { bound = N; }

      memo = memo_alloc(N, M2, bound);
    } else {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.load_file);
      return 1;
    }
  } else {
    memo = memo_alloc(M + 1, M, M + 1);
  }

  /* Count. */
  generic_counter(counter, memo, M);

  if (opts.dump_file) {
    FILE* fd = fopen(opts.dump_file, "w");
    if (fd == NULL) {
      fprintf(stderr, "Cannot open file \"%s\"\n", opts.dump_file);
      return 1;
    }
    memo_dump(fd, memo);
    return 0;
  }

  /* Dump the memoisation table if asked to. */
  if (opts.sample_file) {
    generic_sampler(opts.sample_file, memo, sampler, flags, M);
  }

  return 0;
}
