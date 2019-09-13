#ifndef STAN_MATH_PRIM_SCAL_PROB_STUDENT_T_LPDF_HPP
#define STAN_MATH_PRIM_SCAL_PROB_STUDENT_T_LPDF_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/scal/err/check_consistent_sizes.hpp>
#include <stan/math/prim/scal/err/check_finite.hpp>
#include <stan/math/prim/scal/err/check_not_nan.hpp>
#include <stan/math/prim/scal/err/check_positive_finite.hpp>
#include <stan/math/prim/scal/fun/size_zero.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <stan/math/prim/scal/fun/square.hpp>
#include <stan/math/prim/scal/fun/value_of.hpp>
#include <stan/math/prim/scal/fun/lgamma.hpp>
#include <stan/math/prim/scal/fun/digamma.hpp>
#include <cmath>

namespace stan {
namespace math {

/**
 * The log of the Student-t density for the given y, nu, mean, and
 * scale parameter.  The scale parameter must be greater
 * than 0.
 *
 * \f{eqnarray*}{
 y &\sim& t_{\nu} (\mu, \sigma^2) \\
 \log (p (y \, |\, \nu, \mu, \sigma) ) &=& \log \left( \frac{\Gamma((\nu + 1)
 /2)}
 {\Gamma(\nu/2)\sqrt{\nu \pi} \sigma} \left( 1 + \frac{1}{\nu} (\frac{y -
 \mu}{\sigma})^2 \right)^{-(\nu + 1)/2} \right) \\
 &=& \log( \Gamma( (\nu+1)/2 )) - \log (\Gamma (\nu/2) - \frac{1}{2} \log(\nu
 \pi) - \log(\sigma)
 -\frac{\nu + 1}{2} \log (1 + \frac{1}{\nu} (\frac{y - \mu}{\sigma})^2)
 \f}
 *
 * @param y A scalar variable.
 * @param nu Degrees of freedom.
 * @param mu The mean of the Student-t distribution.
 * @param sigma The scale parameter of the Student-t distribution.
 * @return The log of the Student-t density at y.
 * @throw std::domain_error if sigma is not greater than 0.
 * @throw std::domain_error if nu is not greater than 0.
 * @tparam T_y Type of scalar.
 * @tparam T_dof Type of degrees of freedom.
 * @tparam T_loc Type of location.
 * @tparam T_scale Type of scale.
 */
template <bool propto, typename T_y, typename T_dof, typename T_loc,
          typename T_scale>
inline auto student_t_lpdf(const T_y& y, const T_dof& nu, const T_loc& mu,
                           const T_scale& sigma) {
  using T_partials = partials_return_t<T_y, T_dof, T_loc, T_scale>;
  T_partials logp(0.0);

  using std::log;

  static const char* function = "student_t_lpdf";
  check_not_nan(function, "Random variable", y);
  check_positive_finite(function, "Degrees of freedom parameter", nu);
  check_finite(function, "Location parameter", mu);
  check_positive_finite(function, "Scale parameter", sigma);
  check_consistent_sizes(function, "Random variable", y,
                         "Degrees of freedom parameter", nu,
                         "Location parameter", mu, "Scale parameter", sigma);

  const scalar_seq_view<T_y> y_vec(y);
  const scalar_seq_view<T_dof> nu_vec(nu);
  const scalar_seq_view<T_loc> mu_vec(mu);
  const scalar_seq_view<T_scale> sigma_vec(sigma);
  const size_t N = max_size(y, nu, mu, sigma);

  operands_and_partials<T_y, T_dof, T_loc, T_scale> ops_partials(y, nu, mu,
                                                                 sigma);

  if (!include_summand<propto, T_y, T_dof, T_loc, T_scale>::value) {
    return ops_partials.build(T_partials(0.0));
  } else if (size_zero(y, nu, mu, sigma)) {
    return ops_partials.build(T_partials(0.0));
  }

  T_partials square_y_minus_mu_over_sigma__over_nu = 0;
  T_partials log1p_exp = 0;
  T_partials half_nu = 0;
  for (size_t n = 0; n < N; n++) {
    const T_partials y_dbl = value_of(y_vec[n]);
    const T_partials mu_dbl = value_of(mu_vec[n]);
    const T_partials sigma_dbl = value_of(sigma_vec[n]);
    const T_partials nu_dbl = value_of(nu_vec[n]);
    if (include_summand<propto, T_y, T_dof, T_loc, T_scale>::value) {
      half_nu = 0.5 * value_of(nu_dbl);
      square_y_minus_mu_over_sigma__over_nu
          = square((y_dbl - mu_dbl) / sigma_dbl) / nu_dbl;
      log1p_exp = log1p(square_y_minus_mu_over_sigma__over_nu);
    }
    if (include_summand<propto>::value) {
      logp += NEG_LOG_SQRT_PI;
    }
    if (include_summand<propto, T_dof>::value) {
      const T_partials lgamma_half_nu = lgamma(half_nu);
      const T_partials lgamma_half_nu_plus_half = lgamma(half_nu + 0.5);
      logp += lgamma_half_nu_plus_half - lgamma_half_nu - 0.5 * log(nu_dbl);
    }
    if (include_summand<propto, T_scale>::value) {
      logp -= log(sigma_dbl);
    }
    if (include_summand<propto, T_y, T_dof, T_loc, T_scale>::value) {
      logp -= (half_nu + 0.5) * log1p_exp;
    }

    if (!is_constant_all<T_y>::value) {
      ops_partials.edge1_.partials_[n]
          += -(half_nu + 0.5) * 1.0
             / (1.0 + square_y_minus_mu_over_sigma__over_nu)
             * (2.0 * (y_dbl - mu_dbl) / square(sigma_dbl) / nu_dbl);
    }
    if (!is_constant_all<T_dof>::value) {
      const T_partials inv_nu = 1.0 / nu_dbl;
      const T_partials digamma_half_nu = digamma(half_nu);
      const T_partials digamma_half_nu_plus_half = digamma(half_nu + 0.5);

      ops_partials.edge2_.partials_[n]
          += 0.5 * digamma_half_nu_plus_half - 0.5 * digamma_half_nu
             - 0.5 * inv_nu - 0.5 * log1p_exp
             + (half_nu + 0.5)
                   * (1.0 / (1.0 + square_y_minus_mu_over_sigma__over_nu)
                      * square_y_minus_mu_over_sigma__over_nu * inv_nu);
    }
    if (!is_constant_all<T_loc>::value) {
      ops_partials.edge3_.partials_[n]
          -= (half_nu + 0.5) / (1.0 + square_y_minus_mu_over_sigma__over_nu)
             * (2.0 * (mu_dbl - y_dbl) / (sigma_dbl * sigma_dbl * nu_dbl));
    }
    if (!is_constant_all<T_scale>::value) {
      const T_partials inv_sigma = 1.0 / sigma_dbl;
      ops_partials.edge4_.partials_[n]
          += -inv_sigma
             + (nu_dbl + 1.0) / (1.0 + square_y_minus_mu_over_sigma__over_nu)
                   * (square_y_minus_mu_over_sigma__over_nu * inv_sigma);
    }
  }
  return ops_partials.build(logp);
}

template <typename T_y, typename T_dof, typename T_loc, typename T_scale>
inline auto student_t_lpdf(const T_y& y, const T_dof& nu, const T_loc& mu,
                           const T_scale& sigma) {
  return student_t_lpdf<false>(y, nu, mu, sigma);
}

}  // namespace math
}  // namespace stan
#endif
