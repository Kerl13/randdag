#ifndef _DOAG_H
#define _DOAG_H

#include <stdio.h>
#include <gmp.h>
#include "common.h"

// Counting

typedef struct doag_memo_s {
  int N;
  int M;
  mpz_t* one;
  mpz_t*** vals;
} doag_memo;

doag_memo doag_memo_alloc(int N, int M);
void doag_memo_free(doag_memo);
void doag_memo_load(doag_memo memo, FILE*);
void doag_memo_dump(FILE*, doag_memo);

inline mpz_t* doag_memo_get_ptr(const doag_memo memo, int n, int m, int k) {
  return &(memo.vals[n - 2][k - 1][m + 1 - n]);
}

mpz_t* doag_count(doag_memo, int n, int m, int k);

// Random sampling

randdag_t doag_unif_nm(gmp_randstate_t, const doag_memo, int n, int m);
randdag_t doag_unif_m(gmp_randstate_t, const doag_memo, int m);

#endif
