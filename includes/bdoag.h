#ifndef _BDOAG_H
#define _BDOAG_H

#include <stdio.h>
#include <gmp.h>
#include "common.h"

// Counting

mpz_t* bdoag_count(memo_t, int n, int m, int k, int bound);

// Random sampling

randdag_t bdoag_unif_nm(gmp_randstate_t, const memo_t, int n, int m, int bound);
randdag_t bdoag_unif_m(gmp_randstate_t, const memo_t, int m, int bound);

#endif
