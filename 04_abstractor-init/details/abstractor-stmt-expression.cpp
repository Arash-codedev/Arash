#include "abstractor-stmt-expression.hpp"
#include "abstractor-expression.hpp"
#include <00_common/debug.hpp>



bool abstractor_can_be_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    return abstractor_can_be_expression(context, module, token_indices);
}

void abstractor_validate_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    (void) module;
    (void) token_indices;
    if(!abstractor_can_be_stmt_expression(context, module, token_indices)) 
        throw runtime_error("Code is wrong. A2348032450234");
}

StatementAST abstractor_process_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    assert(token_indices.size() > 0);
    assert(abstractor_can_be_expression(context, module, token_indices));

    StatementAST stmt;
    stmt.module_index = module.index;
    stmt.type = StatementType::Expression;
    ExpressionAST * exp_ast = new ExpressionAST(abstractor_process_expression(context, module, token_indices)); // convert a stack object to a heap object
    stmt.data = (void *) exp_ast;

    return stmt;
}
