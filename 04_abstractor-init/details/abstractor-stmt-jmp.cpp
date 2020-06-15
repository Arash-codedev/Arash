#include "abstractor-stmt-jmp.hpp"
#include "abstractor-expression.hpp"
#include <00_common/debug.hpp>
#include <00_common/utils.hpp>
#include <00_common/helper.hpp>



bool abstractor_is_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    assert(token_indices.size() > 0);
    const Token & first_token = get_token(module, token_indices[0]); 
    switch(first_token.type)
    {
        case TokenType::kw_return:
        case TokenType::kw_break:
        case TokenType::kw_continue:
            return true;
        default:
            return false;
    }
}

void abstractor_validate_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    (void) module;
    assert(token_indices.size() > 0);
}

StatementAST abstractor_process_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    StatementAST stmt;
    stmt.module_index = module.index;
    stmt.type = StatementType::Jump;
    JumpAST * jmp = new JumpAST();
    jmp->module_index = module.index;
    stmt.data = (void *) jmp;
    assert(token_indices.size() > 0);
    const Token & first_token = get_token(module, token_indices[0]);
    if(first_token.type == TokenType::kw_return)
    {
        jmp->type = JumpType::Return;
        vector<int> after_return_token(token_indices.cbegin() + 1, token_indices.cend());
        if(after_return_token.size() > 0)
        {
            vector<vector<int>> ret_val_list_indices = token_comma_separate(context, module, after_return_token);
            for(const vector<int> ret_val_indices : ret_val_list_indices)
            {
                assert(ret_val_indices.size() > 0);
                
                assert(abstractor_can_be_expression(context, module, ret_val_indices));
                abstractor_validate_expression(context, module, ret_val_indices);
                jmp->return_values.push_back(abstractor_process_expression(context, module, ret_val_indices));
            }
        }
        return stmt;
    }
    not_implemented(module, token_indices, "A2348520340523");
    assert(false); // code should not reach here
    return StatementAST(); // to suppress a warning.
}
