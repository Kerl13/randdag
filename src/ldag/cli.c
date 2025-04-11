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

#include <gmp.h>

#include "../../includes/ldag.h"
#include "../common/cli.h"

static randdag_t sampler(gmp_randstate_t s, memo_t memo, int m) {
  return ldag_unif_m(s, memo, m);
}

int main(int argc, char *argv[]) {
  return run_cli(argc, argv, ldag_count, sampler, RD_DOT_LABELLED);
}
