/* Randdag: C library for the uniform random generation of DAGs

   Copyright (C) 2020  Martin Pépin, Antoine Genitrini and Alfredo Viola

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <malloc.h> /* calloc */

#include "../../includes/common.h"

randdag_t randdag_alloc(int N) {
  randdag_vertex *v = calloc(N, sizeof(randdag_vertex));
  randdag_t g;
  g.N = N;
  g.v = v;
  return g;
}

void randdag_free(randdag_t g) {
  int i;
  for (i = 0; i < g.N; i++) {
    if (g.v[i].out_degree > 0)
      free(g.v[i].out_edges);
  }
  free(g.v);
}

void randdag_to_dot(FILE *fd, const randdag_t g, unsigned int flags) {
  int i;

  fprintf(fd, "digraph G {\n  rankdir = \"TB\"\n");
  if (flags & RD_DOT_ORDERING) {
    fprintf(fd, "  ordering = \"out\"\n");
  }
  fprintf(fd, "  edge [arrowhead=none, penwidth=2]\n");
  if (!(flags & RD_DOT_LABELLED)) {
    fprintf(fd, "  node [shape=circle, label=\"\", color=black, style=filled, "
                "width=.5]\n");
  }

  for (i = 0; i < g.N; i++) {
    int j;
    const randdag_vertex u = g.v[i];

    fprintf(fd, "  n%d", u.id);
    if (flags & RD_DOT_LABELLED)
      fprintf(fd, " [label=\"%d\"]", u.id);
    fprintf(fd, "\n");
    if (u.out_degree > 0) {
      fprintf(fd, "  n%d -> {n%d", u.id, u.out_edges[0].id);
      for (j = 1; j < u.out_degree; j++) {
        fprintf(fd, ", n%d", u.out_edges[j].id);
      }
      fprintf(fd, "}\n");
    }
  }

  fprintf(fd, "}\n");
}
