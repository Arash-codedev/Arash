#include "helper.hpp"
#include "utils.hpp"



const vector<ContainerRecognition> container_recognition_set = {
    {ContainerOperator::Parentheses, TokenType::sym_parenthesis_open, TokenType::sym_parenthesis_close},
    {ContainerOperator::SquaredBraket, TokenType::sym_squared_bracket_open, TokenType::sym_squared_bracket_close},
    {ContainerOperator::CurlyBraces, TokenType::sym_curly_brace_open, TokenType::sym_curly_brace_close},
};



// This function verifies that the language has no bug in terms of 
// duplication of containers items or their open/close functions
void verify_container_set(const vector<ContainerRecognition> & crset)
{
    for(int i = 0; i < int(crset.size()); i++)
    {
        assert(crset[i].container_begin != crset[i].container_end);
    }

    for(int i = 0; i < int(crset.size()); i++)
        for(int j = 0; j < int(crset.size()); j++)
        {
            if(i != j)
            {
                assert(crset[i].container_operator != crset[j].container_operator);
                assert(crset[i].container_begin != crset[j].container_begin);
                assert(crset[i].container_end != crset[j].container_end);
                assert(crset[i].container_begin != crset[j].container_end);
                assert(crset[i].container_end != crset[j].container_begin);
            }
        }
}


// example:
//      var all_ret_types = std.vector(token_indices.begin() + 1, token_indices.end()) : vector{Int}
// tokenized as:
//      var|all_ret_types|=|std|.|vector|(|token_indices|.|begin|(|)|+|1|,|token_indices|.|end|(|)|)|:|vector|{|Int|}
// groupified as:
//      var
//      all_ret_types
//      =
//      std.vector
//      (|token_indices|.|begin|(|)|+|1|,|token_indices|.|end|(|)|)
//      :
//      vector
//      {|Int|}
vector<vector<int>> token_container_groupify(
    Context & context,
    Module & module,
    const vector<int> & token_indices)
{
    vector<ContainerSpot> container_stack;
    vector<int> cached_group;

    vector<vector<int>> out;

    verify_container_set(container_recognition_set);
    for(int token_index : token_indices)
    {
        const Token token = get_token(module, token_index);
        cached_group.push_back(token_index);
        for(const ContainerRecognition & cr : container_recognition_set)
        {
            if(cr.container_begin == token.type)
            {
                container_stack.push_back(ContainerSpot(cr.container_operator, token_index));
            }
            else if(cr.container_end == token.type)
            {
                if(container_stack.size() == 0)
                {
                    add_fatal_error(context, module, token, {}, "A container is closed without being opened before");
                    return {};
                }
                else if(container_stack.back().co_operator != cr.container_operator)
                {
                    const Token & stack_top_token = get_token(module, container_stack.back().token_index); 
                    add_fatal_error(context, module, token, { stack_top_token }, "The container closing symbol is different from the container opening symbol.");
                    return {};
                }
                else
                {
                    // container operator matches
                    container_stack.pop_back();
                }
            }
        }
        if(container_stack.size() == 0)
        {
            out.push_back(cached_group);
            cached_group.clear();
        }
    }
    if(container_stack.size() > 0)
    {
        int err_token_index = container_stack.back().token_index; 
        add_fatal_error(context, module, err_token_index, { }, "Container opened but not closed while reaching to the end of the phrase");
        return {};
    }
    else if(cached_group.size() > 0)
    {
        throw runtime_error("Code should not reach here. A84302847384");
    }
    return out;
}


vector<vector<int>> token_comma_separate(
    Context & context, 
    Module & module, 
    const vector<int> & token_indices)
{
    vector<vector<int>> out;
    vector<vector<int>> groups = token_container_groupify(context, module, token_indices);
    vector<int> cache;
    for(const vector<int> & group : groups)
    {
        bool is_comma = false;
        if(group.size() == 1)
        {
            const Token & token = get_token(module, group[0]);
            if(token.type == TokenType::sym_comma)
                is_comma = true;
        }
        if(is_comma)
        {
            out.push_back(cache);
            cache.clear();
        }
        else
        {
            for(int x : group)
                cache.push_back(x);
        }
    }
    out.push_back(cache);
    return out;
}


BuiltinTypes TokenType_to_BuiltinType(TokenType token_type)
{
    switch(token_type)
    {
        case TokenType::vt_Char:
            return BuiltinTypes::Char;
        case TokenType::vt_Int:
            return BuiltinTypes::Int;
        case TokenType::vt_Int8:
            return BuiltinTypes::Int8;
        case TokenType::vt_Int16:
            return BuiltinTypes::Int16;
        case TokenType::vt_Int32:
            return BuiltinTypes::Int32;
        case TokenType::vt_Int64:
            return BuiltinTypes::Int64;
        case TokenType::vt_UInt:
            return BuiltinTypes::UInt;
        case TokenType::vt_UInt8:
            return BuiltinTypes::UInt8;
        case TokenType::vt_UInt16:
            return BuiltinTypes::UInt16;
        case TokenType::vt_UInt32:
            return BuiltinTypes::UInt32;
        case TokenType::vt_UInt64:
            return BuiltinTypes::UInt64;
        case TokenType::vt_Bool:
            return BuiltinTypes::Bool;
        default:
            return BuiltinTypes::Invalid;
    }
}


bool is_decl_qualifier(const Token & token)
{
    switch(token.type)
    {

        case TokenType::kw_const:
        case TokenType::kw_copy:
        case TokenType::kw_ref:
        case TokenType::kw_cref:
        case TokenType::kw_var:
            return true;
        default:
            return false;
    };
}
