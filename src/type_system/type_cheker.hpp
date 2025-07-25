#pragma once
#include "type_system/type.hpp"

struct TypeChecker {};
[[nodiscard]] constexpr bool is_compatible_t(const Type& a, const Type& b) {
    PANIC("not implemented");
}
