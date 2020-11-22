#include <string.h>
#include <gmp.h>
#include <sys/random.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>

#include "cli.h"

static inline int max(int x, int y) { return x < y ? y : x; }

// Generic commands

int generic_sampler(const char* filename, memo_t memo, __sampler_t sampler, int M) {
  // Setup output file
  FILE* fd = stdout;
  if (strcmp("-", filename) != 0)
    fd = fopen(filename, "w");
  if (fd == NULL) {
    fprintf(stderr, "Cannot open file: %s\n", filename);
    return 1;
  }

  // Prepare RNG
  gmp_randstate_t state;
  gmp_randinit_mt(state);
  unsigned long int seed;
  getrandom(&seed, sizeof(unsigned long int), 0);
  printf("Using random seed 0x%lx\n", seed);
  gmp_randseed_ui(state, seed);

  // Sample
  randdag_t g = sampler(state, memo, M);
  randdag_to_dot(fd, g);
  fflush(fd);

  // Do some cleanups
  if (fd != stdout) fclose(fd);
  randdag_free(g);
  gmp_randclear(state);

  return 0;
}

void generic_counter(__counter_t count, memo_t memo, int M) {
  mpz_t x;
  mpz_init(x);

  for (int m = 1; m <= M; m++) {
    mpz_set_ui(x, 0);
    for (int n = 2; n <= m + 1; n++) {
      mpz_add(x, x, *count(memo, n, m));
    }
    printf("%d: ", m);
    mpz_out_str(stdout, 10, x);
    printf("\n");
    fflush(stdout);
  }

  mpz_clear(x);
}

// Command line parsing

typedef struct {
  int count;
  char* sample_file;
  char* dump_file;
  char* load_file;
} cli_options;

static void cli_usage(char progname[]) {
  fprintf(
    stderr,
    "usage: %s [-h] [-c NB] [-s FILE] [-d FILE] [-l FILE]\n\n"
    "optional arguments:\n"
    "  -h, --help             show this help message and exit\n"
    "  -c NB, --count NB      set the number of terms to compute\n"
    "  -s FILE, --sample FILE sample a uniform graph and save it in graphviz format;\n"
    "                         if \"-\" is supplied, stdout will be used\n"
    "  -d FILE, --dump FILE   dump counting information to FILE\n"
    "  -l FILE, --load FILE   load counting information from FILE\n",
    progname
  );
}

static int parse_pos_int(char* s) {
  char* endptr;
  long res = strtol(s, &endptr, 10);
  if (endptr == s)
    return -1;
  else if (res > INT_MAX) {
    fprintf(stderr, "NB is too big (> %d).\n", INT_MAX);
    return -1;
  } else if (res <= 0) {
    fprintf(stderr, "NB most be positive.\n");
    return -1;
  } else
    return (int)res;
}

cli_options cli_parse(int def, int argc, char* argv[]) {
  cli_options cli_opts = {0, NULL, NULL, NULL};
  int help = 0;

  struct option long_options[] = {
    {"count",  required_argument, NULL, 'c'},
    {"sample", required_argument, NULL, 's'},
    {"dump",   required_argument, NULL, 'd'},
    {"load",   required_argument, NULL, 'l'},
    {"help",   no_argument,       &help, 1},
  };
  int option_index = 0;
  int c;

  while (1) {
    c = getopt_long(argc, argv, "c:s:d:l:h", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 'h':
      case 0:
        // Must be --help
        cli_usage(argv[0]);
        exit(0);

      case 'c': {
        if (cli_opts.count > 0) {
          fprintf(stderr, "[--count | -c] cannot be set twice.\n");
          cli_usage(argv[0]);
          exit(1);
        }
        int arg = parse_pos_int(optarg);
        if (arg == -1) {
          cli_usage(argv[0]);
          exit(1);
        }
        cli_opts.count = arg;
        break;
      }

      case 's':
        if (cli_opts.sample_file != NULL) {
          fprintf(stderr, "[--sample | -s] cannot be set twice.\n");
          cli_usage(argv[0]);
          exit(1);
        }
        cli_opts.sample_file = optarg;
        break;

      case 'd':
        if (cli_opts.dump_file != NULL) {
          fprintf(stderr, "[--dump | -d] cannot be set twice.\n");
          cli_usage(argv[0]);
          exit(1);
        }
        cli_opts.dump_file = optarg;
        break;

      case 'l':
        if (cli_opts.load_file != NULL) {
          fprintf(stderr, "[--load | -l] cannot be set twice.\n");
          cli_usage(argv[0]);
          exit(1);
        }
        cli_opts.load_file = optarg;
        break;
    }
  }

  if (cli_opts.count <= 0) cli_opts.count = def;

  if (optind < argc) {
    fprintf(stderr, "Illegal argument: %s\n", argv[optind]);
    cli_usage(argv[0]);
    exit(1);
  }

  return cli_opts;
}

// Generic command line interface

int run_cli(int argc, char* argv[],
            __counter_t counter,
            __sampler_t sampler) {

  cli_options opts = cli_parse(30, argc, argv);
  int M = opts.count;
  memo_t memo;

  // Load a pre-existing dump or allocate a fresh one.
  if (opts.load_file) {
    FILE* fd = fopen(opts.load_file, "r");
    if (fd) {
      // Parse the dump header.
      int N, M2, bound;
      char* line;
      size_t len;
      getline(&line, &len, fd);
      int r = sscanf(line, "%d %d %d\n", &N, &M2, &bound);

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

  // Count.
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

  // Dump the memoisation table if asked to.
  if (opts.sample_file) {
    generic_sampler(opts.sample_file, memo, sampler, M);
  }

  return 0;
}
