#include <malloc.h> // calloc

#include "../../includes/doag.h"


doag doag_alloc(int N) {
  doag_vertex* v = calloc(N, sizeof(doag_vertex));
  doag g = {.N = N, .v = v};
  return g;
}

void doag_free(doag g) {
  for (int i = 0; i < g.N; i++) {
    if (g.v[i].out_degree > 0)
      free(g.v[i].out_edges);
  }
  free(g.v);
}

void doag_to_dot(FILE* fd, doag g) {
  fprintf(fd, "digraph G {\n  rankdir = \"TB\"\n  ordering = \"out\"\n");
  fprintf(fd, "  node [shape=point, label=\"\"]\n  edge [arrowhead=none]\n");

  for (int i = 0; i < g.N; i++) {
    const doag_vertex u = g.v[i];

    fprintf(fd, "  n%d\n", u.id);
    for (int j = 0; j < u.out_degree; j++) {
      fprintf(fd, "  n%d -> n%d\n", u.id, u.out_edges[j].id);
    }
  }

  fprintf(fd, "}\n");
}
