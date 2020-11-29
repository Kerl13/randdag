#ifndef _RANGDAG_CLI_H
#define _RANGDAG_CLI_H

#include <gmp.h> /* gmp_randstate_t */
#include "../../includes/common.h" /* randdag_t */

typedef randdag_t (*__sampler_t) (gmp_randstate_t, memo_t, int M);
typedef mpz_t*    (*__counter_t) (memo_t, int n, int m);

int run_cli(int argc, char* argv[], __counter_t, __sampler_t, long flags);

#endif
