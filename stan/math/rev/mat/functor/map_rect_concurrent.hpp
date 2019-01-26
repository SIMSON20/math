#ifndef STAN_MATH_REV_MAT_FUNCTOR_MAP_RECT_CONCURRENT_HPP
#define STAN_MATH_REV_MAT_FUNCTOR_MAP_RECT_CONCURRENT_HPP

#include <stan/math/prim/mat/fun/typedefs.hpp>

#include <stan/math/prim/mat/functor/map_rect_reduce.hpp>
#include <stan/math/prim/mat/functor/map_rect_concurrent.hpp>
#include <stan/math/prim/scal/err/invalid_argument.hpp>
#include <stan/math/rev/mat/functor/map_rect_reduce.hpp>
#include <stan/math/rev/core/chainablestack.hpp>

#include <vector>
#include <thread>
#include <future>

namespace stan {
namespace math {
namespace internal {

template <int call_id, typename F, typename T_shared_param,
          typename T_job_param>
Eigen::Matrix<typename stan::return_type<T_shared_param, T_job_param>::type,
              Eigen::Dynamic, 1>
map_rect_concurrent(
    const Eigen::Matrix<T_shared_param, Eigen::Dynamic, 1>& shared_params,
    const std::vector<Eigen::Matrix<T_job_param, Eigen::Dynamic, 1>>&
        job_params,
    const std::vector<std::vector<double>>& x_r,
    const std::vector<std::vector<int>>& x_i, std::ostream* msgs) {
  typedef map_rect_reduce<F, T_shared_param, T_job_param> ReduceF;
  typedef map_rect_combine<F, T_shared_param, T_job_param> CombineF;

  const int num_jobs = job_params.size();
  const vector_d shared_params_dbl = value_of(shared_params);
  std::vector<std::future<std::vector<matrix_d>>> futures;

  auto execute_chunk = [&](int start, int size) -> std::vector<matrix_d> {
    const int end = start + size;
    ChainableStack::init();
    std::vector<matrix_d> chunk_f_out;
    chunk_f_out.reserve(size);
    for (int i = start; i != end; i++)
      chunk_f_out.push_back(ReduceF()(
          shared_params_dbl, value_of(job_params[i]), x_r[i], x_i[i], msgs));
    return chunk_f_out;
  };

  int num_threads = get_num_threads(num_jobs);
  int num_jobs_per_thread = num_jobs / num_threads;
  futures.emplace_back(
      std::async(std::launch::deferred, execute_chunk, 0, num_jobs_per_thread));

#ifdef STAN_THREADS
  if (num_threads > 1) {
    const int num_big_threads = num_jobs % num_threads;
    const int first_big_thread = num_threads - num_big_threads;
    for (int i = 1, job_start = num_jobs_per_thread, job_size = 0;
         i < num_threads; ++i, job_start += job_size) {
      job_size = i >= first_big_thread ? num_jobs_per_thread + 1
                                       : num_jobs_per_thread;
      futures.emplace_back(
          std::async(std::launch::async, execute_chunk, job_start, job_size));
    }
  }
#endif

  // collect results
  std::vector<int> world_f_out;
  world_f_out.reserve(num_jobs);
  matrix_d world_output(0, 0);

  int offset = 0;
  for (std::size_t i = 0; i < futures.size(); ++i) {
    const std::vector<matrix_d>& chunk_result = futures[i].get();
    if (i == 0)
      world_output.resize(chunk_result[0].rows(),
                          num_jobs * chunk_result[0].cols());

    for (const auto& job_result : chunk_result) {
      const int num_job_outputs = job_result.cols();
      world_f_out.push_back(num_job_outputs);

      if (world_output.cols() < offset + num_job_outputs)
        world_output.conservativeResize(Eigen::NoChange,
                                        2 * (offset + num_job_outputs));

      world_output.block(0, offset, world_output.rows(), num_job_outputs)
          = job_result;

      offset += num_job_outputs;
    }
  }
  CombineF combine(shared_params, job_params);
  return combine(world_output, world_f_out);
}

}  // namespace internal
}  // namespace math
}  // namespace stan

#endif
