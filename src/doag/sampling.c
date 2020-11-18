#include <malloc.h>
#include <assert.h>
#include <gmp.h>

#include "../../includes/doag.h"

static inline int min(int x, int y) { return x < y ? x : y; }

static void _add_src(gmp_randstate_t state,
                     doag_vertex* dest,
                     doag_vertex* other, int nb_other,
                     int s, int q) {
  // assert(s + q > 0);

  const doag_vertex* sources = other - 1;

  dest->out_degree = s + q;
  doag_vertex* e = calloc(s + q, sizeof(doag_vertex));
  dest->out_edges = e;

  for (int i = 0; i < dest->out_degree; i++) {
    if ((int)gmp_urandomm_ui(state, s + q) < q) {
      *e = *sources;
      sources--;
      q--;
    } else {
      int j = (int)gmp_urandomm_ui(state, nb_other) ;
      doag_vertex tmp = other[0];
      other[0] = other[j];
      other[j] = tmp;
      *e = other[0];
      other++;
      nb_other--;
      s--;
    }
    e++;
  }
}

static void _unif_doag(gmp_randstate_t state, const memo memo, doag_vertex* v, int n, int m, int k) {
  v[0].id = n;

  // Base case: only one vertex: the sink.
  if (n == 1) {
    v[0].out_degree = 0;
    v[0].out_edges = NULL;
    return;
  }

  if (n == 2) {
    // assert(m == 1);
    // assert(k == 1);
    _unif_doag(state, memo, v + 1, 1, 0, 1);
    v[0].out_degree = 1;
    v[0].out_edges = calloc(1, sizeof(doag_vertex));
    v[0].out_edges[0] = v[1];
    return;
  }

  mpz_t r, factor;
  mpz_init(r);
  mpz_init(factor);

  mpz_urandomm(r, state, *doag_count(memo, n, m, k));

  fflush(stdout);

  // p = q + s
  for (int p = 1; p <= min(n - k, m + 2 - n); p++) {
    const int s_start = (p == n - k);
    mpz_set_ui(factor, s_start ? ((n - k - p + s_start) * p) : 1);
    for (int s = s_start; s <= p - (k == 1); s++) {
      const int q = p - s;
      if (m + (k - 1 + q) * (k - 2 + q) / 2 <= p + (n - 1) * (n - 2) / 2) {
        mpz_submul(r, *doag_count(memo, n - 1, m - p, k - 1 + q), factor);
        if (mpz_sgn(r) == -1) {
          _unif_doag(state, memo, v + 1, n - 1, m - p, k - 1 + q);
          _add_src(state, v, v + k + q, n - k - q, s, q);
          mpz_clear(r);
          mpz_init(factor);
          return;
        }
      }
      mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
      mpz_divexact_ui(factor, factor, s + 1);
    }
  }

  assert(0); // Shouldn't reach this point
}

doag doag_unif_nm(gmp_randstate_t state, const memo memo, int n, int m) {
  doag g = doag_alloc(n);
  _unif_doag(state, memo, g.v, n, m, 1);
  return g;
}

doag doag_unif_m(gmp_randstate_t state, const memo memo, int m) {
  mpz_t r, tot;
  mpz_inits(r, tot, NULL);

  for (int n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2)
      mpz_add(tot, tot, *doag_count(memo, n, m, 1));
  }
  mpz_urandomm(r, state, tot);

  for (int n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2) {
      mpz_sub(r, r, *doag_count(memo, n, m, 1));
      if (mpz_sgn(r) == -1) {
        mpz_clear(r);
        mpz_clear(tot);
        return doag_unif_nm(state, memo, n, m);
      }
    }
  }

  assert(0); // Shouldn't reach this point
}
