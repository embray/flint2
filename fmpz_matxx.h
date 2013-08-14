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

    Copyright (C) 2013 Tom Bachmann

******************************************************************************/

#ifndef FMPZ_MATXX_H
#define FMPZ_MATXX_H FMPZ_MATXX_H

#include "fmpz_mat.h"

#include "fmpzxx.h"
#include "fmpz_polyxx.h"

#include "flintxx/ltuple.h"
#include "flintxx/matrix.h"

// TODO input and output
// TODO modular reduction
// TODO addmul
// TODO row reduction
// TODO modular gaussian elimination
// TODO echelon form
// TODO randtest static versions
// TODO nullspace member

namespace flint {
FLINT_DEFINE_BINOP(det_modular)
FLINT_DEFINE_BINOP(det_modular_accelerated)
FLINT_DEFINE_BINOP(mul_multi_mod)
FLINT_DEFINE_BINOP(mat_solve_fflu)
FLINT_DEFINE_BINOP(mat_solve_dixon)
FLINT_DEFINE_BINOP(mat_solve_cramer)
FLINT_DEFINE_BINOP(mat_solve_bound)
FLINT_DEFINE_THREEARY(det_modular_given_divisor)
FLINT_DEFINE_UNOP(det_bareiss)
FLINT_DEFINE_UNOP(det_bound)
FLINT_DEFINE_UNOP(det_cofactor)
FLINT_DEFINE_UNOP(det_divisor)

namespace detail {
template<class Mat>
struct fmpz_matxx_traits : matrices::generic_traits<Mat> { };
} // detail

template<class Operation, class Data>
class fmpz_matxx_expression
    : public expression<derived_wrapper<fmpz_matxx_expression>, Operation, Data>
{
public:
    typedef expression<derived_wrapper< ::flint::fmpz_matxx_expression>,
              Operation, Data> base_t;
    typedef detail::fmpz_matxx_traits<fmpz_matxx_expression> traits_t;

    FLINTXX_DEFINE_BASICS(fmpz_matxx_expression)
    FLINTXX_DEFINE_CTORS(fmpz_matxx_expression)
    FLINTXX_DEFINE_C_REF(fmpz_matxx_expression, fmpz_mat_struct, _mat)

    template<class Expr>
    static evaluated_t create_temporary_rowscols(
            const Expr&, slong rows, slong cols)
    {
        return evaluated_t(rows, cols);
    }
    FLINTXX_DEFINE_MATRIX_METHODS(traits_t)

    // these only make sense with targets
    void set_randbits(frandxx& state, mp_bitcnt_t bits)
        {fmpz_mat_randbits(_mat(), state._data(), bits);}
    void set_randtest(frandxx& state, mp_bitcnt_t bits)
        {fmpz_mat_randtest(_mat(), state._data(), bits);}
    void set_randintrel(frandxx& state, mp_bitcnt_t bits)
        {fmpz_mat_randintrel(_mat(), state._data(), bits);}
    void set_randsimdioph(frandxx& state, mp_bitcnt_t bits, mp_bitcnt_t bits2)
        {fmpz_mat_randsimdioph(_mat(), state._data(), bits, bits2);}
    void set_randntrulike(frandxx& state, mp_bitcnt_t bits, ulong q)
        {fmpz_mat_randntrulike(_mat(), state._data(), bits, q);}
    void set_randntrulike2(frandxx& state, mp_bitcnt_t bits, ulong q)
        {fmpz_mat_randntrulike2(_mat(), state._data(), bits, q);}
    void set_randajtai(frandxx& state, mp_bitcnt_t bits, double alpha)
        {fmpz_mat_randajtai(_mat(), state._data(), bits, alpha);}
    void set_randrank(frandxx& state, slong rank, mp_bitcnt_t bits)
        {fmpz_mat_randrank(_mat(), state._data(), rank, bits);}

    template<class Fmpz>
    typename mp::enable_if<traits::is_fmpzxx<Fmpz> >::type
    set_randdet(frandxx& state, const Fmpz& d)
        {fmpz_mat_randdet(_mat(), state._data(), d.evaluate()._fmpz());}

    template<class Vec>
    int set_randpermdiag(frandxx& state, const Vec& v)
    {
        return fmpz_mat_randpermdiag(_mat(), state._data(), v._array(), v.size());
    }

    void apply_randops(frandxx& state, slong count)
        {fmpz_mat_randops(_mat(), state._data(), count);}

    // these cause evaluation
    slong rank() const {return fmpz_mat_rank(this->evaluate()._mat());}
    bool is_zero() const {return fmpz_mat_is_zero(this->evaluate()._mat());}
    bool is_empty() const {return fmpz_mat_is_empty(this->evaluate()._mat());}
    bool is_square() const {return fmpz_mat_is_square(this->evaluate()._mat());}

    // forwarded lazy ops
    FLINTXX_DEFINE_MEMBER_BINOP(det_modular)
    FLINTXX_DEFINE_MEMBER_BINOP(det_modular_accelerated)
    FLINTXX_DEFINE_MEMBER_BINOP(divexact)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_classical)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_multi_mod)
    FLINTXX_DEFINE_MEMBER_BINOP(pow)

    FLINTXX_DEFINE_MEMBER_BINOP_(solve, mat_solve)
    FLINTXX_DEFINE_MEMBER_BINOP_(solve_bound, mat_solve_bound)
    FLINTXX_DEFINE_MEMBER_BINOP_(solve_cramer, mat_solve_cramer)
    FLINTXX_DEFINE_MEMBER_BINOP_(solve_dixon, mat_solve_dixon)
    FLINTXX_DEFINE_MEMBER_BINOP_(solve_fflu, mat_solve_fflu)

    FLINTXX_DEFINE_MEMBER_3OP(det_modular_given_divisor)

    FLINTXX_DEFINE_MEMBER_UNOP(sqr)
    FLINTXX_DEFINE_MEMBER_UNOP(inv)
    FLINTXX_DEFINE_MEMBER_UNOP(transpose)

    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpz_polyxx, charpoly)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, det)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, det_bareiss)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, det_bound)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, det_cofactor)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, det_divisor)
    FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(fmpzxx, trace)
    //FLINTXX_DEFINE_MEMBER_UNOP_RTYPE(nullspace) // TODO
};

namespace detail {
struct fmpz_mat_data;
} // detail

typedef fmpz_matxx_expression<operations::immediate, detail::fmpz_mat_data> fmpz_matxx;
typedef fmpz_matxx_expression<operations::immediate,
            flint_classes::ref_data<fmpz_matxx, fmpz_mat_struct> > fmpz_matxx_ref;
typedef fmpz_matxx_expression<operations::immediate, flint_classes::srcref_data<
    fmpz_matxx, fmpz_matxx_ref, fmpz_mat_struct> > fmpz_matxx_srcref;

template<>
struct matrix_traits<fmpz_matxx>
{
    template<class M> static slong rows(const M& m)
    {
        return fmpz_mat_nrows(m._mat());
    }
    template<class M> static slong cols(const M& m)
    {
        return fmpz_mat_ncols(m._mat());
    }

    template<class M> static fmpzxx_srcref at(const M& m, slong i, slong j)
    {
        return fmpzxx_srcref::make(fmpz_mat_entry(m._mat(), i, j));
    }
    template<class M> static fmpzxx_ref at(M& m, slong i, slong j)
    {
        return fmpzxx_ref::make(fmpz_mat_entry(m._mat(), i, j));
    }
};

namespace detail {
template<>
struct fmpz_matxx_traits<fmpz_matxx_srcref>
    : matrices::generic_traits_srcref<fmpz_matxx_srcref, fmpzxx_srcref> { };

template<>
struct fmpz_matxx_traits<fmpz_matxx_ref>
    : matrices::generic_traits_ref<fmpz_matxx_ref, fmpzxx_ref, fmpzxx_srcref> { };
template<> struct fmpz_matxx_traits<fmpz_matxx>
    : fmpz_matxx_traits<fmpz_matxx_ref> { };

struct fmpz_mat_data
{
    typedef fmpz_mat_t& data_ref_t;
    typedef const fmpz_mat_t& data_srcref_t;

    fmpz_mat_t inner;

    fmpz_mat_data(slong m, slong n)
    {
        fmpz_mat_init(inner, m, n);
    }

    fmpz_mat_data(const fmpz_mat_data& o)
    {
        fmpz_mat_init_set(inner, o.inner);
    }

    fmpz_mat_data(fmpz_matxx_srcref o)
    {
        fmpz_mat_init_set(inner, o._data().inner);
    }

    ~fmpz_mat_data() {fmpz_mat_clear(inner);}
};
} // detail

#define FMPZ_MATXX_COND_S FLINTXX_COND_S(fmpz_matxx)
#define FMPZ_MATXX_COND_T FLINTXX_COND_T(fmpz_matxx)

namespace traits {
template<class T> struct is_fmpz_matxx
    : flint_classes::is_Base<fmpz_matxx, T> { };
} // traits
namespace mp {
template<class T1, class T2 = void, class T3 = void, class T4 = void>
struct all_fmpz_matxx : mp::and_<all_fmpz_matxx<T1>, all_fmpz_matxx<T2, T3, T4> > { };
template<class T>
struct all_fmpz_matxx<T, void, void, void> : traits::is_fmpz_matxx<T> { };

template<class Out, class T1, class T2 = void, class T3 = void, class T4 = void>
struct enable_all_fmpz_matxx
    : mp::enable_if<all_fmpz_matxx<T1, T2, T3, T4>, Out> { };
} // mp

namespace matrices {
template<>
struct outsize<operations::mul_multi_mod_op>
    : outsize<operations::times> { };

template<> struct outsize<operations::mat_solve_cramer_op>
    : outsize<operations::mat_solve_op> { };
template<> struct outsize<operations::mat_solve_dixon_op>
    : outsize<operations::mat_solve_op> { };
} // matrices

// temporary instantiation stuff
FLINTXX_DEFINE_TEMPORARY_RULES(fmpz_matxx)

namespace rules {
FLINT_DEFINE_DOIT_COND2(assignment, FMPZ_MATXX_COND_T, FMPZ_MATXX_COND_S,
        fmpz_mat_set(to._mat(), from._mat()))

FLINTXX_DEFINE_SWAP(fmpz_matxx, fmpz_mat_swap(e1._mat(), e2._mat()))

FLINTXX_DEFINE_EQUALS(fmpz_matxx, fmpz_mat_equal(e1._mat(), e2._mat()))

FLINT_DEFINE_BINARY_EXPR_COND2(times, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_mul(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZXX_COND_S,
        fmpz_mat_scalar_mul_fmpz(to._mat(), e1._mat(), e2._fmpz()))
FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpz_matxx,
        FMPZ_MATXX_COND_S, traits::is_unsigned_integer,
        fmpz_mat_scalar_mul_ui(to._mat(), e1._mat(), e2))
FLINT_DEFINE_CBINARY_EXPR_COND2(times, fmpz_matxx,
        FMPZ_MATXX_COND_S, traits::is_signed_integer,
        fmpz_mat_scalar_mul_si(to._mat(), e1._mat(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(divexact_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZXX_COND_S,
        fmpz_mat_scalar_divexact_fmpz(to._mat(), e1._mat(), e2._fmpz()))
FLINT_DEFINE_BINARY_EXPR_COND2(divexact_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, traits::is_unsigned_integer,
        fmpz_mat_scalar_divexact_ui(to._mat(), e1._mat(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(divexact_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, traits::is_signed_integer,
        fmpz_mat_scalar_divexact_si(to._mat(), e1._mat(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(plus, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_add(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(minus, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_sub(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(negate, fmpz_matxx, FMPZ_MATXX_COND_S,
        fmpz_mat_neg(to._mat(), from._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(transpose_op, fmpz_matxx, FMPZ_MATXX_COND_S,
        fmpz_mat_transpose(to._mat(), from._mat()))
FLINT_DEFINE_UNARY_EXPR_COND(trace_op, fmpzxx, FMPZ_MATXX_COND_S,
        fmpz_mat_trace(to._fmpz(), from._mat()))

#define FMPZ_MATXX_DEFINE_DET(name) \
FLINT_DEFINE_UNARY_EXPR_COND(name##_op, fmpzxx, FMPZ_MATXX_COND_S, \
        fmpz_mat_##name(to._fmpz(), from._mat()))
FMPZ_MATXX_DEFINE_DET(det)
FMPZ_MATXX_DEFINE_DET(det_cofactor)
FMPZ_MATXX_DEFINE_DET(det_bareiss)
FMPZ_MATXX_DEFINE_DET(det_divisor)
FMPZ_MATXX_DEFINE_DET(det_bound)

FLINT_DEFINE_BINARY_EXPR_COND2(det_modular_op, fmpzxx,
        FMPZ_MATXX_COND_S, tools::is_bool,
        fmpz_mat_det_modular(to._fmpz(), e1._mat(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(det_modular_accelerated_op, fmpzxx,
        FMPZ_MATXX_COND_S, tools::is_bool,
        fmpz_mat_det_modular_accelerated(to._fmpz(), e1._mat(), e2))
FLINT_DEFINE_THREEARY_EXPR_COND3(det_modular_given_divisor_op, fmpzxx,
        FMPZ_MATXX_COND_S, FMPZXX_COND_S, tools::is_bool,
        fmpz_mat_det_modular_given_divisor(to._fmpz(), e1._mat(), e2._fmpz(), e3))

FLINT_DEFINE_THREEARY_EXPR_COND3(mat_at_op, fmpzxx,
        FMPZ_MATXX_COND_S, traits::fits_into_slong, traits::fits_into_slong,
        fmpz_set(to._fmpz(), fmpz_mat_entry(e1._mat(), e2, e3)))

FLINT_DEFINE_BINARY_EXPR_COND2(mul_classical_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_mul(to._mat(), e1._mat(), e2._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_multi_mod_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_mul(to._mat(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(sqr_op, fmpz_matxx, FMPZ_MATXX_COND_S,
        fmpz_mat_sqr(to._mat(), from._mat()))
FLINT_DEFINE_BINARY_EXPR_COND2(pow_op, fmpz_matxx,
        FMPZ_MATXX_COND_S, traits::is_unsigned_integer,
        fmpz_mat_pow(to._mat(), e1._mat(), e2))

namespace rdetail {
typedef make_ltuple<mp::make_tuple<bool, fmpz_matxx, fmpzxx>::type >::type
    fmpz_mat_inv_rt;

typedef make_ltuple<mp::make_tuple<fmpzxx, fmpzxx>::type >::type
    fmpz_mat_solve_bound_rt;

typedef make_ltuple<mp::make_tuple<slong, fmpz_matxx>::type >::type
    fmpz_mat_nullspace_rt;
} // rdetail

FLINT_DEFINE_UNARY_EXPR_COND(inv_op, rdetail::fmpz_mat_inv_rt,
        FMPZ_MATXX_COND_S,
        to.template get<0>() = fmpz_mat_inv(to.template get<1>()._mat(),
            to.template get<2>()._fmpz(), from._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(charpoly_op, fmpz_polyxx, FMPZ_MATXX_COND_S,
        fmpz_mat_charpoly(to._poly(), from._mat()))

#define FMPZ_MATXX_DEFINE_SOLVE(name) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, rdetail::fmpz_mat_inv_rt, \
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S, \
        to.template get<0>() = fmpz_##name(to.template get<1>()._mat(), \
            to.template get<2>()._fmpz(), e1._mat(), e2._mat()))
FMPZ_MATXX_DEFINE_SOLVE(mat_solve)
FMPZ_MATXX_DEFINE_SOLVE(mat_solve_dixon)
FMPZ_MATXX_DEFINE_SOLVE(mat_solve_cramer)
FMPZ_MATXX_DEFINE_SOLVE(mat_solve_fflu)

FLINT_DEFINE_BINARY_EXPR_COND2(mat_solve_bound_op,
        rdetail::fmpz_mat_solve_bound_rt,
        FMPZ_MATXX_COND_S, FMPZ_MATXX_COND_S,
        fmpz_mat_solve_bound(to.template get<0>()._fmpz(),
            to.template get<1>()._fmpz(), e1._mat(), e2._mat()))

FLINT_DEFINE_UNARY_EXPR_COND(nullspace_op, rdetail::fmpz_mat_nullspace_rt,
        FMPZ_MATXX_COND_S, to.template get<0>() = fmpz_mat_nullspace(
            to.template get<1>()._mat(), from._mat()))

} // rules
} // flint

#endif
