#ifndef _RANDDAG_COMMON_H
#define _RANDDAG_COMMON_H

#include <stdio.h> // FILE

// Graph representation

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
void randdag_to_dot(FILE*, const randdag_t);

#endif
