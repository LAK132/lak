#ifndef LAK_OPTIONAL_HPP
#define LAK_OPTIONAL_HPP

#include "type_utils.hpp"

#include <optional>

namespace lak
{
  template<typename T>
  using optional = std::optional<std::conditional_t<
    std::is_lvalue_reference_v<T>,
    std::reference_wrapper<std::remove_reference_t<T>>, // lvalue reference.
    std::remove_reference_t<T>>>; // rvalue reference or not a references.
}

#endif // LAK_OPTIONAL_HPP
