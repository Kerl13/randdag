#ifndef _RANGDAG_CLI_H
#define _RANGDAG_CLI_H

#include <gmp.h> // gmp_randstate_t
#include <stdio.h> // FILE
#include "../../includes/common.h" // randdag_t

typedef randdag_t (*__sampler_t) (gmp_randstate_t, int M);
typedef mpz_t*    (*__counter_t) (int n, int m);
typedef void      (*__dumper_t)  (FILE*);
typedef int       (*__prepare_t) (int, const char*);

int run_cli(int argc, char* argv[],
            __prepare_t, __counter_t, __sampler_t, __dumper_t);

#endif
