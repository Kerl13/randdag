#ifndef _LDAG_H
#define _LDAG_H

#include <stdio.h>
#include <gmp.h>

#include "common.h"

// Counting

mpz_t* ldag_count(memo_t, int n, int m, int k);

#endif
