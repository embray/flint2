/*
    Copyright (C) 2019 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpz_mod.h"

void fmpz_mod_pow_fmpz(fmpz_t a, const fmpz_t b, const fmpz_t pow,
                                                     const fmpz_mod_ctx_t ctx)
{
    FLINT_ASSERT(fmpz_mod_is_canonical(b, ctx));
    fmpz_powm(a, b, pow, ctx->n);
    FLINT_ASSERT(fmpz_mod_is_canonical(a, ctx));
    return;
}
