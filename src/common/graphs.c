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

void randdag_to_dot(FILE* fd, const randdag_t g, long flags) {
  fprintf(fd, "digraph G {\n  rankdir = \"TB\"\n");
  if (flags & RD_DOT_ORDERING) {
    fprintf(fd, "  ordering = \"out\"\n");
  }
  fprintf(fd, "  edge [arrowhead=none, penwidth=2]\n");
  if (! (flags & RD_DOT_LABELLED)) {
    fprintf(fd, "  node [shape=circle, label=\"\", color=black, style=filled, width=1.5]\n");
  }

  for (int i = 0; i < g.N; i++) {
    const randdag_vertex u = g.v[i];

    fprintf(fd, "  n%d", u.id);
    if (flags & RD_DOT_LABELLED) fprintf(fd, " [label=\"%d\"]", u.id);
    fprintf(fd, "\n");
    for (int j = 0; j < u.out_degree; j++) {
      fprintf(fd, "  n%d -> n%d\n", u.id, u.out_edges[j].id);
    }
  }

  fprintf(fd, "}\n");
}
