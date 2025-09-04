#include "semantic_analysis/analyzer.hpp"

// TODO: we need to assign storage for local variables,
// and add the concept of the Activation Record (AR)
void assign_storage(ASTNode& node) {
    // we need the concept of a address (storage location) for each variable
    // - concept of a data area
    // - AR (activation record)
    //   the AR models the control information of a procedure call

    // i also need a call stack that keeps all my function calls
    //
    // find out what LLVM wants for creating functions
}

int func(int x) {
    if (x) return x * func(x - 1);
    else
        return 1;
}

struct ActivationRecord {};
