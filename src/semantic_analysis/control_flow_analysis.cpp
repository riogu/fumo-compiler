#include "semantic_analysis/analyzer.hpp"
#include "utils/common_utils.hpp"

enum struct ControlFlowResult { NoReturn, Returns, MayReturn};
using enum ControlFlowResult;
ControlFlowResult combine_branch_flow(ControlFlowResult one, ControlFlowResult two) {
    if (one == Returns && two == Returns) return Returns;
    if (one == Returns || two == Returns) return MayReturn;
    return NoReturn;
}
ControlFlowResult combine_sequential_flow(ControlFlowResult one, ControlFlowResult two) {
    if (one == Returns   || two == Returns)   return Returns;
    if (one == MayReturn || two == MayReturn) return MayReturn;
    return NoReturn;
}

void Analyzer::analyze_function_control_flow(ASTNode& node) {
    auto& func_decl = get<FunctionDecl>(&node);
    // we need to iterate through all the paths of this function and find out if we always have valid paths
    if (node.type.kind == Type::void_ && !node.type.ptr_count) return; // void functions dont have to return in all control paths
    
    auto* body = func_decl.body.value_or(nullptr);
    if (!body) return; // was a forward declaration of a function in another file (or libc function)

    ControlFlowResult curr_result = NoReturn;

    auto &nodes = get<BlockScope>(body).nodes;
    for (auto node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
        auto next_result = control_flow_analysis(**node_it);
        curr_result = combine_sequential_flow(curr_result, next_result);

        if (curr_result == Returns) { 
            // remove dead code (for valid LLVM IR)
            nodes.erase(std::next(node_it), nodes.end()); 
            return; // this function will always return (OK)
        }
    }
    // if we get here, function MayReturn or is NoReturn. therefore, its invalid
    report_error(node.source_token, "non-void function does not return a value in all control paths.");
}

ControlFlowResult Analyzer::control_flow_analysis(ASTNode& node) {
    match(node) {
        holds(UnaryExpr, &un) {
            if (un.kind == UnaryExpr::return_statement) return ControlFlowResult::Returns;
        }
        holds(IfStmt, &if_stmt) {
            switch (if_stmt.kind) {
                case IfStmt::if_statement:
                case IfStmt::else_if_statement: {
                    ControlFlowResult if_result = NoReturn;
                    auto &nodes = get<BlockScope>(if_stmt.body).nodes;
                    for (auto node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
                        auto next_result = control_flow_analysis(**node_it);

                        if_result = combine_sequential_flow(if_result, next_result);

                        if (if_result == Returns) {
                            nodes.erase(std::next(node_it), nodes.end()); 
                            break;
                        }
                    }
                    ControlFlowResult else_result = NoReturn;
                    if (auto* else_node = if_stmt.else_stmt.value_or(nullptr)) {
                        else_result = control_flow_analysis(*else_node);
                    }
                    return combine_branch_flow(if_result, else_result);
                }
                case IfStmt::else_statement: {
                    ControlFlowResult else_result = NoReturn;
                    auto &nodes = get<BlockScope>(if_stmt.body).nodes;
                    for (auto node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
                        auto next_result = control_flow_analysis(**node_it);

                        else_result = combine_sequential_flow(else_result, next_result);

                        if (else_result == Returns) {
                            nodes.erase(std::next(node_it), nodes.end()); 
                            return Returns;
                        }
                    }
                    return else_result;
                }
            }
        }
        _default {} // do nothing, this function wont return in this current statement
    }
    return ControlFlowResult::NoReturn;
}
