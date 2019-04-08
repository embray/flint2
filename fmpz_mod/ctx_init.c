/*
    Copyright (C) 2019 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpz_mod.h"

void fmpz_mod_ctx_init(fmpz_mod_ctx_t ctx, const fmpz_t n)
{
    fmpz_init_set(ctx->n, n);
    fmpz_preinvn_init(ctx->ninv, n);
}
