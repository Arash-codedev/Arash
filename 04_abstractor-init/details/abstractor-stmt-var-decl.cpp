#include "abstractor-stmt-var-decl.hpp"
#include "abstractor-expression.hpp"
#include <00_common/utils.hpp>
#include <00_common/helper.hpp>
#include <00_common/debug.hpp>


bool abstractor_is_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    assert(token_indices.size() > 0);
    const Token & fist_token = get_token(module, token_indices[0]);
    if(fist_token.type == TokenType::kw_var)
        return true;
    if(fist_token.type == TokenType::kw_const)
        return true;
    return false;
}

void abstractor_validate_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    (void) module;
    (void) token_indices;
    if(token_indices.size() < 3)
        not_implemented(module, token_indices, "A2087765653");
    assert(token_indices.size() >= 3);
    return ;
}

StatementAST abstractor_process_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    (void) context;
    assert(token_indices.size() >= 3);
    VarDeclParts parts = separate_var_decle_parts(context, module, token_indices);

    StatementAST vd_stmt;
    vd_stmt.module_index = module.index;
    vd_stmt.type  = StatementType::DeclareVar;
    VarDeclAST * p_vd = new VarDeclAST();
    vd_stmt.data = (void *) p_vd;
    p_vd->module_index = module.index;
    p_vd->is_const = false;
    for(int index : parts.qualifiers)
    {
        const Token & token = get_token(module, index);
        if(token.type == TokenType::kw_const)
            p_vd->is_const = true;
        if(token.type == TokenType::kw_var)
            p_vd->is_const = false;
        else
        {
            cout << "Unexpected token: " << token.text << endl;
            throw runtime_error("Error: unknown qualitier. A0348750324 \n (should be an error)");
        }
    }

    if(parts.name.size() == 0)
        throw runtime_error("Error: unnamed variable. A0847352034 \n (should be an error)");

    p_vd->var_name_tokens = parts.name;
    if(parts.initialization.size() > 0)
        p_vd->initial_val.push_back(abstractor_process_expression(context, module, parts.initialization));
    if(parts.type.size() > 0)
        p_vd->type.push_back(abstractor_process_expression(context, module, parts.type));

    return vd_stmt;
}


VarDeclParts separate_var_decle_parts(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    VarDeclParts parts;
    vector<vector<int>> groups = token_container_groupify(context, module, token_indices);
    int i = 0;

    // decle qualifier stage
    for(;i < (int) groups.size(); i++)
    {
        assert(groups[i].size() == 1); // type qualifiers are not inside () or {} // todo: it should be an error
        int token_index = groups[i][0];
        const Token & token = get_token(module, token_index);
        if(is_decl_qualifier(token))
            parts.qualifiers.push_back(token_index);
        else
            break;
    }
    bool data_init = false;

    // name stage
    for(;i < (int) groups.size(); i++)
    {
        assert(groups[i].size() > 0);
        if(groups[i].size() == 1)
        {
            int token_index = groups[i][0];
            const Token & token = get_token(module, token_index);
            if(token.type == TokenType::sym_equal)
            {
                data_init = true;
                i++; // skip the token
                break;
            }
            if(token.type == TokenType::sym_colon)
            {
                i++; // skip the token
                break;
            }
        }
        for(int x : groups[i])
            parts.name.push_back(x);
    }

    // initialization stage
    if(data_init)
    {
        for(;i < (int) groups.size(); i++)
        {
            assert(groups[i].size() > 0);
            if(groups[i].size() == 1)
            {
                int token_index = groups[i][0];
                const Token & token = get_token(module, token_index);
                if(token.type == TokenType::sym_colon)
                {
                    i++; // skip the token
                    break;
                }
            }
            for(int x : groups[i])
                parts.initialization.push_back(x);
        }
    }

    // type stage
    for(;i < (int) groups.size(); i++)
    {
        for(int x : groups[i])
            parts.type.push_back(x);
    }

    return parts;
}
