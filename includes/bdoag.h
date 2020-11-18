#ifndef _BDOAG_H
#define _BDOAG_H

#include <stdio.h>
#include <gmp.h>
#include "common.h"

// Counting

typedef struct doag_memo_s {
  int N;
  int M;
  int bound;
  mpz_t* one;
  mpz_t*** vals;
} bdoag_memo;

bdoag_memo bdoag_memo_alloc(int N, int M, int bound);
void bdoag_memo_free(bdoag_memo);
void bdoag_memo_load(bdoag_memo memo, FILE*);
void bdoag_memo_dump(FILE*, bdoag_memo);

inline mpz_t* bdoag_memo_get_ptr(const bdoag_memo memo, int n, int m, int k) {
  return &(memo.vals[n - 2][k - 1][m + 1 - n]);
}

mpz_t* bdoag_count(bdoag_memo, int n, int m, int k, int bound);

// Random sampling

randdag_t bdoag_unif_nm(gmp_randstate_t, const bdoag_memo, int n, int m, int bound);
randdag_t bdoag_unif_m(gmp_randstate_t, const bdoag_memo, int m, int bound);

#endif
