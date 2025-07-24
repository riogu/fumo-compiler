#include "parser/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>

/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
*/

struct SymbolTable {
    // updates the ASTNodes to represent a fully compliant program
    vec<std::map<str, ASTNode*>> scope_to_nodes;
    i64 depth = 0; // each scope moves forward 1 element in the vector
    // we clear the current depth when entering a new scope
    // information about structs/namespaces is kept separately
    void open_scope();
    void close_scope();
    void insert_node(ASTNode* node);
    [[nodiscard]] ASTNode* find_node(std::string_view var_name);
};

struct Analyzer {
  public:
    void semantic_analysis(Scope& file_scope);
    void analyze(ASTNode& node);

  private:
    SymbolTable sym_table {};
};
