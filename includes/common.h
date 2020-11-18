#ifndef _RANDDAG_COMMON_H
#define _RANDDAG_COMMON_H

#include <stdio.h> // FILE
#include <gmp.h> // gmp_randstate_t

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

// Generic command line stuff
// TODO: do not include this in the static libraries

typedef randdag_t (*__sampler_t)(gmp_randstate_t, int M);
typedef mpz_t*    (*__counter_t)(int n, int m);
typedef void      (*__dumper_t)(FILE*);
typedef int       (*__prepare_t)(int, const char*);

int run_cli(int argc, char* argv[],
            __prepare_t, __counter_t, __sampler_t, __dumper_t);

#endif
