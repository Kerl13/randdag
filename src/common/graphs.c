#include <malloc.h> // calloc

#include "../../includes/common.h"


randdag_t randdag_alloc(int N) {
  randdag_vertex* v = calloc(N, sizeof(randdag_vertex));
  randdag_t g = {.N = N, .v = v};
  return g;
}

void randdag_free(randdag_t g) {
  for (int i = 0; i < g.N; i++) {
    if (g.v[i].out_degree > 0)
      free(g.v[i].out_edges);
  }
  free(g.v);
}

void randdag_to_dot(FILE* fd, const randdag_t g) {
  fprintf(fd, "digraph G {\n  rankdir = \"TB\"\n  ordering = \"out\"\n");
  fprintf(fd, "  node [shape=point, label=\"\"]\n  edge [arrowhead=none]\n");

  for (int i = 0; i < g.N; i++) {
    const randdag_vertex u = g.v[i];

    fprintf(fd, "  n%d\n", u.id);
    for (int j = 0; j < u.out_degree; j++) {
      fprintf(fd, "  n%d -> n%d\n", u.id, u.out_edges[j].id);
    }
  }

  fprintf(fd, "}\n");
}