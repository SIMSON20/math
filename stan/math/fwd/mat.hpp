#ifndef STAN_MATH_FWD_MAT_HPP
#define STAN_MATH_FWD_MAT_HPP

#include <stan/math/prim/mat/fun/Eigen.hpp>

#include <stan/math/fwd/core.hpp>
#include <stan/math/fwd/meta.hpp>

#include <stan/math/fwd/vectorize/apply_scalar_unary.hpp>
#include <stan/math/prim/mat.hpp>
#include <stan/math/fwd/arr.hpp>

#include <stan/math/fwd/fun/Eigen_NumTraits.hpp>
#include <stan/math/fwd/fun/columns_dot_product.hpp>
#include <stan/math/fwd/fun/columns_dot_self.hpp>
#include <stan/math/fwd/fun/crossprod.hpp>
#include <stan/math/fwd/fun/determinant.hpp>
#include <stan/math/fwd/fun/divide.hpp>
#include <stan/math/fwd/fun/dot_product.hpp>
#include <stan/math/fwd/fun/dot_self.hpp>
#include <stan/math/fwd/fun/inverse.hpp>
#include <stan/math/fwd/fun/log_determinant.hpp>
#include <stan/math/fwd/fun/log_softmax.hpp>
#include <stan/math/fwd/fun/log_sum_exp.hpp>
#include <stan/math/fwd/fun/mdivide_left.hpp>
#include <stan/math/fwd/fun/mdivide_left_ldlt.hpp>
#include <stan/math/fwd/fun/mdivide_left_tri_low.hpp>
#include <stan/math/fwd/fun/mdivide_right.hpp>
#include <stan/math/fwd/fun/mdivide_right_tri_low.hpp>
#include <stan/math/fwd/fun/multiply.hpp>
#include <stan/math/fwd/fun/multiply_lower_tri_self_transpose.hpp>
#include <stan/math/fwd/fun/qr_Q.hpp>
#include <stan/math/fwd/fun/qr_R.hpp>
#include <stan/math/fwd/fun/quad_form_sym.hpp>
#include <stan/math/fwd/fun/rows_dot_product.hpp>
#include <stan/math/fwd/fun/rows_dot_self.hpp>
#include <stan/math/fwd/fun/softmax.hpp>
#include <stan/math/fwd/fun/squared_distance.hpp>
#include <stan/math/fwd/fun/sum.hpp>
#include <stan/math/fwd/fun/tcrossprod.hpp>
#include <stan/math/fwd/fun/to_fvar.hpp>
#include <stan/math/fwd/fun/trace_quad_form.hpp>
#include <stan/math/fwd/fun/typedefs.hpp>
#include <stan/math/fwd/fun/unit_vector_constrain.hpp>

#include <stan/math/fwd/functor/gradient.hpp>
#include <stan/math/fwd/functor/hessian.hpp>
#include <stan/math/fwd/functor/jacobian.hpp>

#endif
