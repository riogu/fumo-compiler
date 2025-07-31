#pragma once
#include <libassert/assert.hpp>
#include <unordered_set>
#include "base_definitions/symbols.hpp"
#include "utils/common_utils.hpp"
struct ASTNode;
#define make_enum_member(_v) _v##_,


struct Type {
    ASTNode* identifier;
    enum Kind {
        Undetermined, // should be converted in the type checker or give error
        Nothing,      // bottom type, used for if statements and other typeless constructs
        // Any,       // generic Any type (unused atm)
        // Function,  // add this later if you want function objects/pointers
        struct_,
        enum_,
        map_macro(make_enum_member, builtin_types)
    } kind = Undetermined;
    enum TypeQualifier { const_, volatile_, extern_, static_ };
    std::unordered_set<TypeQualifier> qualifiers;
    // Opt<ASTNode*> declaration {}; NOTE: is present in identifier node
    int ptr_count = 0;
};

#define each_builtin_type(builtin_name) if (type_name == #builtin_name) return Type::builtin_name##_;
[[nodiscard]] constexpr Type::Kind builtin_type_kind(std::string_view type_name) {
    map_macro(each_builtin_type, builtin_types);
    INTERNAL_PANIC("internal compiler error: provided unknown type name: {}", type_name);
}


#undef make_enum_member
#undef each_builtin_type

