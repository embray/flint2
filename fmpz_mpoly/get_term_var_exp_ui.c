/*
    Copyright (C) 2016 William Hart
    Copyright (C) 2018 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fmpz_mpoly.h"

ulong fmpz_mpoly_get_term_var_exp_ui(const fmpz_mpoly_t A, slong i,
                                         slong var, const fmpz_mpoly_ctx_t ctx)
{
    slong offset, shift;
    slong N;

    if ((ulong) i >= (ulong) A->length)
    {
        flint_throw(FLINT_ERROR, "Index out of range in fmpz_mpoly_get_term_var_exp_ui");
    }

    if (A->bits <= FLINT_BITS)
    {
        mpoly_gen_offset_shift_sp(&offset, &shift, var, A->bits, ctx->minfo);

        N = mpoly_words_per_exp_sp(A->bits, ctx->minfo);

        return ((A->exps + N*i)[offset] >> shift)
                     & ((-UWORD(1)) >> (FLINT_BITS - A->bits));
    }
    else
    {
        slong j;
        ulong wpf = A->bits/FLINT_BITS;
        offset = mpoly_gen_offset_mp(var, A->bits, ctx->minfo);

        N = mpoly_words_per_exp_mp(A->bits, ctx->minfo);

        for (j = 1; j < wpf; j++)
        {
            if ((A->exps + N*i)[offset + j] != 0)
                flint_throw(FLINT_ERROR, "Exponent does not fit a ulong.");
        }

        return (A->exps + N*i)[offset + 0];
    }
}
