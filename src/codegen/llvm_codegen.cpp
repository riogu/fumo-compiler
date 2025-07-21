#include "codegen/llvm_codegen.hpp"
#include "parser/ast_node.hpp"

void Codegen::begin_codegen(vec<ASTNode>& AST) {
    for (auto& node : AST) {
        match(node) {
            holds(Primary) codegen<Primary>(&node);
            holds(Binary) codegen<Binary>(&node);
            holds(Function) codegen<Binary>(&node);
            _ {}
        }
    }
}
