#ifndef STAN__MATH__REV__CORE__SET_ZERO_ALL_ADJOINTS_HPP
#define STAN__MATH__REV__CORE__SET_ZERO_ALL_ADJOINTS_HPP

#include <stan/math/rev/core/chainable.hpp>
#include <stan/math/rev/core/chainable_alloc.hpp>
#include <stan/math/rev/core/chainablestack.hpp>

namespace stan {
  namespace agrad {

    /**
     * Reset all adjoint values in the stack to zero.
     */
    static void set_zero_all_adjoints() {
      for (size_t i = 0; i < ChainableStack::var_stack_.size(); ++i)
        ChainableStack::var_stack_[i]->set_zero_adjoint();
      for (size_t i = 0; i < ChainableStack::var_nochain_stack_.size(); ++i)
        ChainableStack::var_nochain_stack_[i]->set_zero_adjoint();
    }

  }
}
#endif
