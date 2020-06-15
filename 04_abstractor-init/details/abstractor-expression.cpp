#include "abstractor-stmt-expression.hpp"
#include <00_common/debug.hpp>
#include <00_common/helper.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>


bool abstractor_can_be_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    assert(token_indices.size() > 0);
    const Token & first_token = get_token(module, token_indices[0]);
    return first_token.category != TokenCategory::keyword;
}


void abstractor_validate_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    (void) module;
    (void) token_indices;
    if(!abstractor_can_be_expression(context, module, token_indices)) 
        throw runtime_error("Code is wrong. A20375027434");
}


// get a group of group of tokens
// all tokens before pivot will go to left-hand-side
// all tokens after pivot will go to right-hand-side
//
static void separate_group_tokens_pivot(
    const vector<vector<int>> & groups,
    vector<int> & lhs,
    vector<int> & rhs,
    int group_pivot_index
    )
{
    for(int i = 0; i < group_pivot_index; i++)
        for(int token_index : groups[i])
            lhs.push_back(token_index);
    for(int i = group_pivot_index + 1; i < int(groups.size()); i++)
        for(int token_index : groups[i])
            rhs.push_back(token_index);
}


// example:
//      (y*y + 3*y)/x + sin(x*x + y*y)
//      x
//      myclass.x
//      myclass.show()
//
ExpressionAST abstractor_process_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    vector<vector<int>> groups = token_container_groupify(context, module, token_indices);
    ExpressionAST expr;
    expr.module_index = module.index;

    // detect: direct assignment
    // example:
    //     a = b + c
    //     z = x
    for(int gi = 0; gi < int(groups.size()); gi++)
    {
        const vector<int> & group = groups[gi];
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.type == TokenType::sym_equal)
            {
                vector<int> lhs;
                vector<int> rhs;
                separate_group_tokens_pivot(groups, lhs, rhs, gi);
                assert(lhs.size() > 0); // todo: to be an error
                assert(rhs.size() > 0); // todo: to be an error
                expr.type = ExpressionType::Assignment;
                expr.operators.push_back(OperatorType::IndependentAssignment);
                ExpressionAST left_expr = abstractor_process_expression(context, module, lhs);
                ExpressionAST right_expr = abstractor_process_expression(context, module, rhs);
                expr.operands.push_back(left_expr);
                expr.operands.push_back(right_expr);
                return expr;
            }
        }
    }

    // detect addition
    // example:
    //     a + b
    //     sin(x)*y + z
    // but not: +x
    for(int gi = 1 /* avoid unary plus */; gi < int(groups.size()); gi++)
    {
        const vector<int> & group = groups[gi];
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.type == TokenType::sym_plus)
            {
                vector<int> lhs;
                vector<int> rhs;
                separate_group_tokens_pivot(groups, lhs, rhs, gi);
                assert(lhs.size() > 0); // todo: to be an error
                assert(rhs.size() > 0); // todo: to be an error
                expr.type = ExpressionType::Addition;
                expr.operators.push_back(OperatorType::Addition);
                ExpressionAST left_expr = abstractor_process_expression(context, module, lhs);
                ExpressionAST right_expr = abstractor_process_expression(context, module, rhs);
                expr.operands.push_back(left_expr);
                expr.operands.push_back(right_expr);
                return expr;
            }
        }
    }

    // detect id
    // example:
    //     x
    //     sin
    //     myObj
    //     Int
    if(groups.size() == 1)
    {
        const vector<int> & group = groups[0];
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.category == TokenCategory::id || token.category == TokenCategory::builtin_type)
            {
                expr.type = ExpressionType::id;
                expr.id_token_index = group[0];
                return expr;
            }
        }
    }

    // detect numbers
    // example:
    //     5
    //     5.6
    //     .7
    if(groups.size() == 1)
    {
        const vector<int> & group = groups[0];
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.category == TokenCategory::literal && token.type == TokenType::literal_number)
            {
                expr.type = ExpressionType::Number;
                expr.literal_token_index = group[0];
                return expr;
            }
        }
    }


    // detect string literals
    // example:
    //     'hello world'
    //     'E'
    if(groups.size() == 1)
    {
        const vector<int> & group = groups[0];
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.category == TokenCategory::literal && token.type == TokenType::literal_string)
            {
                expr.type = ExpressionType::String;
                expr.literal_token_index = group[0];
                return expr;
            }
        }
    }

    // detect function call
    // example:
    //     sin(x*x + theta - 0.1)
    //     init()
    if(groups.size() == 2)
    {
        const vector<int> & first_group = groups[0];
        const vector<int> & second_group = groups[1];
        if(first_group.size() == 1 && second_group.size() >= 2)
        {
            const Token & first_group_token = get_token(module, first_group[0]);
            const Token & second_group_first_token = get_token(module, second_group[0]);
            const Token & second_group_last_token = get_token(module, second_group.back());
            if(
                first_group_token.category == TokenCategory::id &&
                second_group_first_token.type == TokenType::sym_parenthesis_open &&
                second_group_last_token.type == TokenType::sym_parenthesis_close
                )
            {
                vector<int> param_line(second_group.begin() + 1, second_group.end() - 1);
                vector<vector<int>> params = token_comma_separate(context, module, param_line);

                expr.type = ExpressionType::FunctionCall;
                expr.func_name_token_index = first_group[0];
                expr.func_name = get_token(module, first_group[0]).text;
                for(const vector<int> & param : params)
                {
                    ExpressionAST p_param_ast = abstractor_process_expression(context, module, param);
                    expr.func_params.push_back(p_param_ast);
                }
                return expr;
            }
        }
    }

    not_implemented(module, token_indices, "A76976975547");
    assert(false); // code should not reach here.
    return ExpressionAST(); // to supress a warning
}
