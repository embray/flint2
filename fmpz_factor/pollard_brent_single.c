/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2015 Kushagra Singh

******************************************************************************/

/* This is an implementation of the pollard rho algorithm, with a more efficient
   cycle finding algorithm, as proposed by Richard Brent. Details can be found 
   in the paper http://maths-people.anu.edu.au/~brent/pd/rpb051i.pdf, pseudocode 
   is available on page 182 of the same paper */

#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "ulong_extras.h"
#include "mpn_extras.h"

int
fmpz_factor_pollard_brent_single(fmpz_t p_factor, fmpz_t n_in, fmpz_t yi, 
                                 fmpz_t ai, mp_limb_t max_iters)
{
    mp_ptr a, y, n, ninv, temp;
    mp_limb_t n_size, normbits, ans, size, cy;
    mp_limb_t al, yl, val, valinv;
    __mpz_struct *fac, *mpz_ptr;
    int ret;

    TMP_INIT;

    if (fmpz_is_even(n_in))
    {
       fmpz_set_ui(p_factor, 2);
       return 1;
    }

    n_size = fmpz_size(n_in);

    if (n_size == 1)
    {
        val = fmpz_get_ui(n_in);
        count_leading_zeros(normbits, val);
        val <<= normbits;
        valinv = n_preinvert_limb(val);

        al = fmpz_get_ui(ai);
        yl = fmpz_get_ui(yi);
        al <<= normbits;
        yl <<= normbits;

        ret = n_factor_pollard_brent_single(&ans, val, valinv, al, yl, normbits, max_iters);
        ans >>= normbits;
        fmpz_set_ui(p_factor, ans);
        return ret;
    }

    mpz_ptr = COEFF_TO_PTR(*yi);
    temp = COEFF_TO_PTR(*n_in)->_mp_d;
    count_leading_zeros(normbits, temp[n_size - 1]);

    TMP_START;
    a = TMP_ALLOC(n_size * sizeof(mp_limb_t));
    y = TMP_ALLOC(n_size * sizeof(mp_limb_t));
    n = TMP_ALLOC(n_size * sizeof(mp_limb_t));
    ninv = TMP_ALLOC(n_size * sizeof(mp_limb_t));

    /* copying n_in onto n, and normalizing */

    temp = COEFF_TO_PTR(*n_in)->_mp_d;
    count_leading_zeros(normbits, temp[n_size - 1]);
    if (normbits)
        mpn_lshift(n, temp, n_size, normbits);
    else
        mpn_copyi(n, temp, n_size);

    flint_mpn_preinvn(ninv, n, n_size);

    fac = _fmpz_promote(p_factor);
    mpz_realloc2(fac, n_size * FLINT_BITS);
    fac->_mp_size = n_size;

    mpn_zero(a, n_size);
    mpn_zero(y, n_size);

    if (normbits)
    {        
        if ((!COEFF_IS_MPZ(*yi)))
        {
            y[0] = fmpz_get_ui(yi);
            cy = mpn_lshift(y, y, 1, normbits);
            if (cy)
                y[1] = cy;
        }
        else
        {
            mpz_ptr = COEFF_TO_PTR(*yi);
            temp = mpz_ptr->_mp_d;
            size = mpz_ptr->_mp_size;    
            cy = mpn_lshift(y, temp, size, normbits);

            if (cy)
                y[size] = cy;
        }

        if ((!COEFF_IS_MPZ(*ai)))
        {
            a[0] = fmpz_get_ui(ai);
            cy = mpn_lshift(a, a, 1, normbits);
            if (cy)
                a[1] = cy;
        }
        else
        {
            mpz_ptr = COEFF_TO_PTR(*ai);
            temp = mpz_ptr->_mp_d;
            size = mpz_ptr->_mp_size;
            cy = mpn_lshift(a, temp, size, normbits);
            if (cy)
                a[size] = cy;
        }
    }
    else
    {
        temp = COEFF_TO_PTR(*yi)->_mp_d;
        mpn_copyi(y, temp, n_size);
        temp = COEFF_TO_PTR(*ai)->_mp_d;
        mpn_copyi(a, temp, n_size);
    }

    ret = flint_mpn_factor_pollard_brent_single(fac->_mp_d, n, ninv, a, y, n_size, normbits, max_iters);

    if (ret)
    {
        fac->_mp_size = ret;        /* ret is number of limbs of factor found */
        _fmpz_demote_val(p_factor);    
    }

    TMP_END;
    
    return ret;    
}
