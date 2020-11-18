#ifndef _LDAG_H
#define _LDAG_H

#include <stdio.h>
#include <gmp.h>

// Counting

typedef struct ldag_memo_s {
  int N;
  int M;
  mpz_t* one;
  mpz_t* two;
  mpz_t*** vals;
} ldag_memo;

ldag_memo ldag_memo_alloc(int N, int M);
void ldag_memo_free(ldag_memo);
void ldag_memo_load(ldag_memo, FILE*);
void ldag_memo_dump(FILE*, const ldag_memo);

inline mpz_t* ldag_memo_get_ptr(const ldag_memo memo, int n, int m, int k) {
  return &(memo.vals[n - 2][k - 1][m + 1 - n]);
}

mpz_t* ldag_count(ldag_memo, int n, int m, int k);

#endif
