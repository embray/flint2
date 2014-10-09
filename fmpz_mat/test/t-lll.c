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

    Copyright (C) 2014 Abhinav Baid

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz_mat.h"
#include "fmpq.h"
#include "fmpq_mat.h"
#include "fmpq_vec.h"

int
main(void)
{
    int i;
    FLINT_TEST_INIT(state);


    flint_printf("lll....");
    fflush(stdout);

    /* check output basis is LLL reduced (randajtai used) */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        int result;
        fmpz_mat_t A;
        fmpq_lll_t fl;

        slong m;

        m = n_randint(state, 10);

        fmpz_mat_init(A, m, m);
        fmpq_lll_context_init(fl, 0.75, 0.5);

        fmpz_mat_randajtai(A, state, 0.5);

        fmpz_mat_lll(A, fl);

        result = fmpz_mat_is_reduced(A, 0.75, 0.5);

        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("A:\n");
            fmpz_mat_print_pretty(A);
            abort();
        }

        fmpz_mat_clear(A);
        fmpq_lll_context_clear(fl);
    }

    FLINT_TEST_CLEANUP(state);

    flint_printf("PASS\n");
    return EXIT_SUCCESS;
}