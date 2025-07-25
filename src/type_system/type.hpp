#pragma once
#include <libassert/assert.hpp>
#include "type_system/symbol_definitions.hpp"
#include "utils/common_utils.hpp"

struct ASTNode;

#define make_enum_member(_v) _##_v,
enum struct TypeKind {
    _undetermined, // should be converted in the type checker or give error
    _struct,
    _union,
    _enum,
    map_macro(make_enum_member, builtin_types)
};

struct TypeQualifier {
  private:
    enum impl { _const, _volatile } value;
  public:
    constexpr operator impl() { return value; }
    TypeQualifier(impl some) : value(some) {}
    TypeQualifier() {}
};

struct Struct {};
struct Enum {};

struct Type {
    str name = "undetermined";
    TypeKind kind = TypeKind::_undetermined;
    TypeQualifier qualifier;
    Opt<ASTNode*> struct_or_enum {}; // or union
    int ptr_count = 0;
};

#define each_builtin_type(builtin_name) if (type_name == #builtin_name) return TypeKind::_##builtin_name;

[[nodiscard]] constexpr TypeKind builtin_type_kind(std::string_view type_name) {
    map_macro(each_builtin_type, builtin_types);
    INTERNAL_PANIC("internal compiler error: provided unknown type name: {}", type_name);
}

#undef make_enum_member
#undef each_builtin_type

