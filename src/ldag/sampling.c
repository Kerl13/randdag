#include <malloc.h>
#include <assert.h>
#include <gmp.h>

#include "../../includes/common.h"
#include "../../includes/ldag.h"

static inline int min(int x, int y) { return x < y ? x : y; }


static void _fisher_yates(gmp_randstate_t state, randdag_vertex* v, int n) {
  for (int i = 0; i < n; i++) {
    int j = (int)gmp_urandomm_ui(state, n - i);
    int tmp = v[i].id;
    v[i].id = v[i + j].id;
    v[i + j].id = tmp;
  }
}


static void _unif_combination(gmp_randstate_t state,
                              randdag_vertex* dest,
                              const randdag_vertex* set,
                              int n, int k) {
  while (k > 0) {
    if ((int)gmp_urandomm_ui(state, n) < k) {
      *dest = *set;
      dest++;
      k--;
    }
    set++;
    n--;
  }
}


static void _unif_ldag(gmp_randstate_t state, const memo_t memo, randdag_vertex* v, int n, int m, int k) {
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
    _unif_ldag(state, memo, v + 1, 1, 0, 1);
    v[0].out_degree = 1;
    v[0].out_edges = calloc(1, sizeof(randdag_vertex));
    v[0].out_edges[0] = v[1];
    return;
  }

  mpz_t r, factor, factor0, tmp;
  mpz_init(r);
  mpz_init(factor);
  mpz_init_set_ui(factor0, 1);
  mpz_init(tmp);

  mpz_init_set(tmp, *ldag_count(memo, n, m, k));
  mpz_mul_ui(tmp, tmp, k);
  mpz_divexact_ui(tmp, tmp, n);

  mpz_urandomm(r, state, tmp);

  // p = q + s
  for (int p = 1; p <= min(n - k, m + 2 - n); p++) {
    mpz_mul_ui(factor0, factor0, k - 1 + p);
    mpz_divexact_ui(factor0, factor0, p);

    const int s_start = (p == n - k);
    mpz_set(factor, factor0);
    if (s_start == 1) {
      mpz_mul_ui(factor, factor, p * (n - k - p + 1));
      mpz_divexact_ui(factor, factor, k - 1 + p);
    }

    for (int s = s_start; s <= p - (k == 1); s++) {
      const int q = p - s;
      const int kk = k - 1 + q;
      if (m + kk * (kk - 1) / 2 <= p + (n - 1) * (n - 2) / 2) {
        mpz_submul(r, *ldag_count(memo, n - 1, m - p, kk), factor);
        if (mpz_sgn(r) < 0) {
          _unif_ldag(state, memo, v + 1, n - 1, m - p, k - 1 + q);
          v->out_degree = p;
          v->out_edges = calloc(p, sizeof(randdag_vertex));
          _unif_combination(state, v->out_edges, v + k + q, n - k - q, s);
          _unif_combination(state, v->out_edges, v + 1, k - 1 + q, q);
          mpz_clears(r, tmp, factor, factor0, NULL);
          return;
        }
      }

      mpz_mul_ui(factor, factor, (n - k - q + 1) * q);
      mpz_divexact_ui(factor, factor, (s + 1) * kk);
    }
  }

  assert(0); // Shouldn't reach this point
}

randdag_t ldag_unif_nm(gmp_randstate_t state, const memo_t memo, int n, int m) {
  randdag_t g = randdag_alloc(n);
  _unif_ldag(state, memo, g.v, n, m, 1);
  _fisher_yates(state, g.v, n);
  return g;
}

randdag_t ldag_unif_m(gmp_randstate_t state, const memo_t memo, int m) {
  mpz_t r, tot;
  mpz_inits(r, tot, NULL);

  for (int n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2)
      mpz_add(tot, tot, *ldag_count(memo, n, m, 1));
  }
  mpz_urandomm(r, state, tot);

  for (int n = 2; n <= m + 1; n++) {
    if (m <= n * (n - 1) / 2) {
      mpz_sub(r, r, *ldag_count(memo, n, m, 1));
      if (mpz_sgn(r) == -1) {
        mpz_clear(r);
        mpz_clear(tot);
        return ldag_unif_nm(state, memo, n, m);
      }
    }
  }

  assert(0); // Shouldn't reach this point
}
