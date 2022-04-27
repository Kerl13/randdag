#ifndef _LDAG_H
#define _LDAG_H

#include <gmp.h>
#include <stdio.h>

#include "common.h"

/* Counting */

mpz_t *ldag_count(memo_t, int n, int m, int k);

/* Random sampling */

randdag_t ldag_unif_m(gmp_randstate_t, memo_t, int m);
randdag_t ldag_unif_nm(gmp_randstate_t, memo_t, int n, int m);

#endif
