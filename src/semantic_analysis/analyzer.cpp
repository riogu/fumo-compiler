#include "semantic_analysis/analyzer.hpp"

void Analyzer::semantic_analysis(vec<ASTNode>& AST) {
    for (auto& node : AST) analyze(node);
}

void Analyzer::analyze(ASTNode& node) {
    match(node) {
        holds(Primary,  &prim)  {}
        holds(Unary,    &un)    {}
        holds(Binary,   &bin)   {}
        holds(Variable, &var)   {}
        holds(Function, &func)  {}
        holds(Scope,    &scope) {}
        _ {}
    }
}
