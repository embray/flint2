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

    Copyright (C) 2011 William Hart

******************************************************************************/

#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "fmpz.h"
#include "mpn_extras.h"

int mpn_divides(mp_ptr q, mp_srcptr array1, 
      mp_size_t limbs1, mp_srcptr arrayg, mp_size_t limbsg, mp_ptr temp)
{
   mpn_tdiv_qr(q, temp, 0, array1, limbs1, arrayg, limbsg);
   while ((limbsg) && temp[limbsg - 1] == 0) limbsg--;

   return (limbsg == 0);
}
