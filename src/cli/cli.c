#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>

#include "cli.h"

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

randdag_cli_options randdag_cli_parse(int def, int argc, char* argv[]) {
  randdag_cli_options cli_opts = {0, NULL, NULL, NULL};
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
