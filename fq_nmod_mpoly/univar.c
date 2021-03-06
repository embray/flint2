/*
    Copyright (C) 2019 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "fq_nmod_mpoly.h"

void fq_nmod_mpoly_univar_init(fq_nmod_mpoly_univar_t poly,
                                                 const fq_nmod_mpoly_ctx_t ctx)
{
    poly->coeffs = NULL;
    poly->exps = NULL;
    poly->alloc = 0;
    poly->length = 0;
    poly->var = -WORD(1);
}


void fq_nmod_mpoly_univar_clear(fq_nmod_mpoly_univar_t poly,
                                                 const fq_nmod_mpoly_ctx_t ctx)
{
    slong i;
    for (i = 0; i < poly->alloc; i++)
        fq_nmod_mpoly_clear(poly->coeffs + i, ctx);
    flint_free(poly->coeffs);
    flint_free(poly->exps);
}


void fq_nmod_mpoly_univar_swap(fq_nmod_mpoly_univar_t poly1,
                   fq_nmod_mpoly_univar_t poly2, const fq_nmod_mpoly_ctx_t ctx)
{
   fq_nmod_mpoly_univar_struct t = *poly1;
   *poly1 = *poly2;
   *poly2 = t;
}

void fq_nmod_mpoly_univar_print_pretty(const fq_nmod_mpoly_univar_t poly,
                                const char ** x, const fq_nmod_mpoly_ctx_t ctx)
{
    slong i;
    if (poly->length == 0)
        flint_printf("0");
    for (i = 0; i < poly->length; i++)
    {
        if (i != 0)
            flint_printf("+");
        flint_printf("(");
        fq_nmod_mpoly_print_pretty(poly->coeffs + i,x,ctx);
        if (x==NULL)
            flint_printf(")*x%wd^%wd", poly->var+1,poly->exps[i]);
        else
            flint_printf(")*%s^%wd", x[poly->var],poly->exps[i]);
    }
}


void fq_nmod_mpoly_univar_fit_length(fq_nmod_mpoly_univar_t poly,
                                   slong length, const fq_nmod_mpoly_ctx_t ctx)
{
    slong i;
    slong old_alloc = poly->alloc;
    slong new_alloc = FLINT_MAX(length, 2*poly->alloc);

    if (length > old_alloc)
    {
        if (old_alloc == 0)
        {
            poly->exps = (ulong *) flint_malloc(new_alloc*sizeof(ulong));
            poly->coeffs = (fq_nmod_mpoly_struct *) flint_malloc(
                                       new_alloc*sizeof(fq_nmod_mpoly_struct));
        }
        else
        {
            poly->exps = (ulong *) flint_realloc(poly->exps,
                                                      new_alloc*sizeof(ulong));
            poly->coeffs = (fq_nmod_mpoly_struct *) flint_realloc(poly->coeffs,
                                       new_alloc*sizeof(fq_nmod_mpoly_struct));
        }

        for (i = old_alloc; i < new_alloc; i++)
        {
            fq_nmod_mpoly_init(poly->coeffs + i, ctx);
        }
        poly->alloc = new_alloc;
    }
}


/* if the coefficient doesn't exist, a new one is created */
fq_nmod_mpoly_struct * _fq_nmod_mpoly_univar_get_coeff(fq_nmod_mpoly_univar_t A,
                          ulong pow, slong bits, const fq_nmod_mpoly_ctx_t ctx)
{
    slong i, j;
    fq_nmod_mpoly_struct * xk;

    for (i = 0; i < A->length && A->exps[i] >= pow; i++)
    {
        if (A->exps[i] == pow) 
        {
            return A->coeffs + i;
        }
    }

    fq_nmod_mpoly_univar_fit_length(A, A->length + 1, ctx);

    for (j = A->length; j > i; j--)
    {
        A->exps[j] = A->exps[j - 1];
        fq_nmod_mpoly_swap(A->coeffs + j, A->coeffs + j - 1, ctx);
    }
    
    A->length++;
    A->exps[i] = pow;
    xk = A->coeffs + i;
    xk->length = 0;
    fq_nmod_mpoly_fit_bits(xk, bits, ctx);
    xk->bits = bits;
    return xk;
}


/*
    the coefficients of poly1 should be constructed with the same bitcounts
    as that of poly2
*/
int fq_nmod_mpoly_to_univar(fq_nmod_mpoly_univar_t poly1,
         const fq_nmod_mpoly_t poly2, slong var, const fq_nmod_mpoly_ctx_t ctx)
{
    slong i, shift, off, bits, N;
    ulong k;
    slong poly1_old_length = poly1->length;
    slong len = poly2->length;
    fq_nmod_struct * coeff = poly2->coeffs;
    ulong * exp = poly2->exps;
    ulong * one;
    fq_nmod_mpoly_struct * xk;
    slong xk_len;
    TMP_INIT;

    TMP_START;
    
    bits = poly2->bits;

    N = mpoly_words_per_exp(bits, ctx->minfo);
    one = (ulong*) TMP_ALLOC(N*sizeof(ulong));

    if (bits <= FLINT_BITS)
    {
        mpoly_gen_monomial_offset_shift_sp(one, &off, &shift,
                                                        var, bits, ctx->minfo);
        poly1->length = 0;
        poly1->var = var;
        for (i = 0; i < len; i++)
        {
            ulong mask = (-UWORD(1)) >> (FLINT_BITS - bits);
            k = (exp[N*i + off] >> shift) & mask;
            xk = _fq_nmod_mpoly_univar_get_coeff(poly1, k, bits, ctx);
            xk_len = xk->length;
            fq_nmod_mpoly_fit_length(xk, xk_len + 1, ctx);
            fq_nmod_set(xk->coeffs + xk_len, coeff + i, ctx->fqctx);
            xk->length = xk_len + 1;
            mpoly_monomial_msub(xk->exps + N*xk_len, exp + N*i, k, one, N);
        }
    }
    else
    {
        fmpz_t c;
        fmpz_init(c);

        off = mpoly_gen_monomial_offset_mp(one, var, bits, ctx->minfo);

        poly1->length = 0;
        poly1->var = var;
        for (i = 0; i < len; i++)
        {
            fmpz_set_ui_array(c, exp + N*i + off, bits/FLINT_BITS);
            if (!fmpz_fits_si(c))
            {
                fmpz_clear(c);
                goto failed;
            }
            k = fmpz_get_si(c);
            xk = _fq_nmod_mpoly_univar_get_coeff(poly1, k, bits, ctx);
            xk_len = xk->length;
            fq_nmod_mpoly_fit_length(xk, xk_len + 1, ctx);
            fq_nmod_set(xk->coeffs + xk_len, coeff + i, ctx->fqctx);
            xk->length = xk_len + 1;
            mpoly_monomial_msub_mp(xk->exps + N*xk_len, exp + N*i, k, one, N);
        }

        fmpz_clear(c);
    }

    /* demote remaining coefficients */
    for (i = poly1->length; i < poly1_old_length; i++)
    {
        fq_nmod_mpoly_clear(poly1->coeffs + i, ctx);
        fq_nmod_mpoly_init(poly1->coeffs + i, ctx);
    }    

    TMP_END;
    return 1;

failed:
    fq_nmod_mpoly_univar_clear(poly1, ctx);
    fq_nmod_mpoly_univar_init(poly1, ctx);
    TMP_END;
    return 0;
}
