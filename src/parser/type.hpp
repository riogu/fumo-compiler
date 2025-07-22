#pragma once
#include "lexer/token_definitions.hpp"
#include <memory>
#include <optional>

using str = std::string;
template<typename T>
using Opt = std::optional<T>;
template<typename T>
using unique_ptr = std::unique_ptr<T>;

struct Struct{};
struct Enum{};


#define make_enum_member(_v) _##_v,
enum struct TypeKind {
    _undetermined, // should be converted in the type checker or give error
    _struct,
    _union,
    _enum,
    _ptr,
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

struct Type {
    str name;
    TypeKind kind;
    TypeQualifier qualifier;
    Opt<Struct> struct_type; // or union
    Opt<Enum> enum_type;
    int ptr_count = 0;
};

#define each_builtin_type(builtin_name) if (type_name == #builtin_name) return TypeKind::_##builtin_name;

[[nodiscard]] constexpr TypeKind builtin_type_kind(std::string_view type_name) {
    map_macro(each_builtin_type, builtin_types);
    PANIC(std::format("internal compiler error: provided unknown type name: {}", type_name));
}

#undef make_enum_member
#undef each_builtin_type
