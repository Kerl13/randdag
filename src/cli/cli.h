#ifndef _RANDDAG_CLI_UTILS_H
#define _RANDDAG_CLI_UTILS_H

typedef struct {
  int count;
  char* sample_file;
  char* dump_file;
  char* load_file;
} randdag_cli_options;

randdag_cli_options randdag_cli_parse(int def, int argc, char* argv[]);

#endif
