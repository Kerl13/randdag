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

#ifndef _RANGDAG_CLI_H
#define _RANGDAG_CLI_H

#include "../../includes/common.h" /* randdag_t */
#include <gmp.h>                   /* gmp_randstate_t */

typedef randdag_t (*__sampler_t)(gmp_randstate_t, memo_t, int N);
typedef mpz_t *(*__counter_t)(memo_t, int n, int m, int k, int bound);

int run_cli(int argc, char *argv[], __counter_t, __sampler_t, long flags);

#endif
