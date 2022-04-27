#ifndef _DOAG_H
#define _DOAG_H

#include "common.h"
#include <gmp.h>
#include <stdio.h>

/* Counting */

mpz_t *doag_count(memo_t, int n, int m, int k);

/* Random sampling */

randdag_t doag_unif_nm(gmp_randstate_t, const memo_t, int n, int m);
randdag_t doag_unif_m(gmp_randstate_t, const memo_t, int m);

#endif
