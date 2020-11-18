#ifndef _DOAG_H
#define _DOAG_H

#include <stdio.h>
#include <gmp.h>

// Graph representation

typedef struct doag_vertex_s {
  int id;
  int out_degree;
  struct doag_vertex_s *out_edges;
} doag_vertex;

typedef struct doag_s {
  int N;
  doag_vertex* v;
} doag;

doag doag_alloc(int N);
void doag_free(doag);
void doag_to_dot(FILE*, doag);

// Counting

typedef struct doag_memo_s {
  int N;
  int M;
  mpz_t* one;
  mpz_t*** vals;
} memo;

memo doag_memo_alloc(int N, int M);
void doag_memo_free(memo);
void doag_memo_load(memo memo, FILE*);
void doag_memo_dump(FILE*, memo);

inline mpz_t* doag_memo_get_ptr(const memo memo, int n, int m, int k) {
  return &(memo.vals[n - 2][k - 1][m + 1 - n]);
}

mpz_t* doag_count(memo, int n, int m, int k);

// Random sampling

doag doag_unif_nm(gmp_randstate_t, const memo, int n, int m);
doag doag_unif_m(gmp_randstate_t, const memo, int m);

#endif
