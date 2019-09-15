#ifndef STAN_MATH_OPENCL_REP_VECTOR_HPP
#define STAN_MATH_OPENCL_REP_VECTOR_HPP
#ifdef STAN_OPENCL
#include <stan/math/opencl/matrix_cl.hpp>
#include <stan/math/opencl/matrix_cl_view.hpp>
#include <stan/math/opencl/kernels/rep_matrix.hpp>
#include <stan/math/prim/scal/err/check_nonnegative.hpp>
#include <stan/math/prim/meta.hpp>

namespace stan {
namespace math {
/**
 * Creates a matrix_cl representing a vector 
 * by replicating the value of the only element
 * in the input 1x1 matrix_cl.
 *
 * @tparam T type of elements in the input matrix
 * @param x the input 1x1 matrix_cl
 * @param m number of rows in the results row_vector
 *
 * @return matrix_cl with replicated value from the input matrix
 *
 * @throw <code>domain_error</code> if the
 * requested dimensions are negative
 * @throw <code>invalid_argument</code> if input
 * element is not a matrix_cl of size 1
 *
 */
template <typename T, typename = enable_if_all_arithmetic<T>>
inline matrix_cl<double> rep_vector(const matrix_cl<T>& x, int m) {
  check_nonnegative("rep_vector (OpenCL)", "elements", m);
  matrix_cl<double> A(m, 1);
  if (A.size() == 0) {
    return A;
  }
  check_mat_size_one("rep_vector (OpenCL)", "x", x);
  try {
    opencl_kernels::rep_matrix(cl::NDRange(A.rows(), A.cols()), A, x,
                             A.rows(), A.cols(), x.rows(), x.cols(), A.view());
  } catch (cl::Error& e) {
    check_opencl_error("rep_vector", e);
  }
  return A;
}

}  // namespace math
}  // namespace stan

#endif
#endif
