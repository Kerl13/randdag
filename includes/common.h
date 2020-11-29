#ifndef _RANDDAG_COMMON_H
#define _RANDDAG_COMMON_H

#include <stdio.h>
#include <gmp.h>

/***************
 * Memoization *
 ***************/

typedef struct {
  int N;
  int M;
  int bound;
  mpz_t* one;
  mpz_t*** vals;
} memo_t;

/* Allocate a memoisation structure with enough space for storing counting
 * information for DAGs of max degree bounded by bound, up to N vertices and up
 * to M edges.
 * Set bound to N for DAGs of unbounded degree. */
memo_t memo_alloc(int N, int M, int bound);

/* Free the memory space occupied by a memo_t allocated by memo_alloc. */
void memo_free(memo_t);

/* Dump the content of a memo_t into a file. */
void memo_dump(FILE*, memo_t);

/* Load the content of a dump (as produced by memo_dump) into a memo_t.
 * It is the caller's responsibility to ensure that there is enough space in
 * the table for the content of the dump. */
void memo_load(memo_t, FILE*);

/* Get a pointer to the coefficient of indices (n, m, k) stored in memo.
 * It is the caller's responsibility to ensure that (n, m, k) is not out of
 * bounds. */
#define memo_get_ptr(memo, n, m, k) (&((memo).vals[(n) - 2][(k) - 1][(m) + 1 - (n)]))

/************************
 * Graph representation *
 ************************/

typedef struct _randdag_vertex {
  int id;
  int out_degree;
  struct _randdag_vertex *out_edges;
} randdag_vertex;

typedef struct _randdag_t {
  int N;
  randdag_vertex* v;
} randdag_t;

randdag_t randdag_alloc(int N);
void randdag_free(randdag_t);

#define RD_DOT_LABELLED 1
#define RD_DOT_ORDERING 2

/* Render a graphv to graphviz format.
 * The `flag` argument is a combination of zero or more of the above flags:
 * RD_DOT_ORDERING indicates that the out-edges of the vertices are ordered;
 * RD_DOT_LABELLED indicate that the vertices' ids shall be used as labels. */
void randdag_to_dot(FILE*, const randdag_t, unsigned int flags);

#endif
