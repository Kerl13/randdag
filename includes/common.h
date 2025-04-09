#ifndef _RANDDAG_COMMON_H
#define _RANDDAG_COMMON_H

/** \file common.h
 * Memoisation structure and graph representation common to all types of DAGs.
 */

#include <gmp.h>
#include <stdio.h>

/** Memoisation structure for counting algorithms. */
typedef struct {
  /** The maximum number of vertices of the graphs that this structure can count
   */
  int N;
  /** The maximum number of edges of the graphs that this structure can count */
  int M;
  /** The maximum out-degree of the graphs that this structure can count */
  int bound;
  /* XXX. The integer 0. Leave this undocumented. */
  mpz_t *zero;
  /* XXX. The integer 1. Leave this undocumented. */
  mpz_t *one;
  /** The array in which counting information is stored. Never manipulate this
   * directly. */
  mpz_t ***vals;
} memo_t;

/** Allocate a memoisation structure with enough space for storing counting
 * information for DAGs of max degree bounded by bound, up to N vertices and up
 * to M edges.
 * If a non-positive bound is passed, allocate enough space for graphs of
 * unbounded out-degree.
 *
 * A structure allocated with this function must be freed using the memo_free
 * function. */
memo_t memo_alloc(int N, int M, int bound);

/** Free the memory space occupied by a memo_t allocated by memo_alloc. */
void memo_free(memo_t);

/** Dump the content of a memo_t into a file. */
void memo_dump(FILE *, memo_t);

/** Load the content of a dump (as produced by memo_dump) into a memo_t.
 * It is the caller's responsibility to ensure that there is enough space in
 * the table for the content of the dump. */
void memo_load(memo_t, FILE *);

/** Get a pointer to the coefficient of indices (n, m, k) stored in memo.
 * It is the caller's responsibility to ensure that (n, m, k) is not out of
 * bounds. */
#define memo_get_ptr(memo, n, m, k) (&((memo).vals[(n)-2][(k)-1][m]))

/** The type of graph vertices */
typedef struct _randdag_vertex {
  /** An integer ids for the vertex */
  int id;
  /** The out-degree of the vertex */
  int out_degree;
  /** The array of out-edges of the vertex */
  struct _randdag_vertex *out_edges;
} randdag_vertex;

/** The type of graphs */
typedef struct _randdag_t {
  /** The number of vertices of the graph */
  int N;
  /** An array of size N of vertices */
  randdag_vertex *v;
} randdag_t;

/** Allocate a graph with N uninitialised vertices */
randdag_t randdag_alloc(int N);

/** Free a graph allocated with randdag_alloc */
void randdag_free(randdag_t);

#define RD_DOT_LABELLED 1
#define RD_DOT_ORDERING 2

/** Render a graph to graphviz format.
 * The `flag` argument is a combination of zero or more of the above flags:
 * RD_DOT_ORDERING indicates that the out-edges of the vertices are ordered;
 * RD_DOT_LABELLED indicate that the vertices' ids shall be used as labels. */
void randdag_to_dot(FILE *, const randdag_t, unsigned int flags);

#endif
