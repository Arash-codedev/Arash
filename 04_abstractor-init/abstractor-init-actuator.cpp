#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <00_common/helper.hpp>
#include <languages/language-interface.hpp>
#include "abstractor-init-actuator.hpp"
#include <04_abstractor-init/details/abstractor-stmt-expression.hpp>
#include <04_abstractor-init/details/abstractor-stmt-jmp.hpp>
#include <04_abstractor-init/details/abstractor-stmt-var-decl.hpp>


enum class BlockType
{
    Invalid,
    Import,
    Function,
    Class,
    VarDelcare,
};



static void debug_print(const Module & module, const RawBlockTree & block_tree, int tab)
{
    if(tab == 0)
    {
        cout << "---------------------------------" << endl;
        cout << "Debug block-tree print" << endl;
        cout << "---------------------------------" << endl;
    }
    for(int i = 0; i < tab * 4; i++)
        cout << " ";
    if(block_tree.line.token_indices.size() > 0)
    {
        int token_index = block_tree.line.token_indices[0];
        const Token & token = module.token_list[token_index];
        cout << "* " << token.text << endl;
    }
    else
    {
        cout << "* (empty token)" << endl;
    }
    for(const RawBlockTree & child : block_tree.children)
        debug_print(module, child, tab + 1);
    if(tab == 0)
    {
        cout << "---------------------------------" << endl;
    }
}


static void debug_print(const Module & module, const RawBlockTree & block_tree)
{
    debug_print(module, block_tree, 0);
}


// return the first token index in the token line
// and perform some debug checks
static int get_block_first_token_index(
    const Module & module, 
    const RawBlockTree & block_tree,
    int block_index)
{
    assert(block_index >= 0);
    assert(block_index < int(block_tree.children.size()));
    if(block_tree.children[block_index].line.token_indices.size() == 0)
        throw runtime_error("Token line is empty. A35427930847");

    int first_token_index = block_tree.children[block_index].line.token_indices[0];
    assert(first_token_index >= 0);
    assert(first_token_index < int(module.token_list.size()));

    return first_token_index;
}


static BlockType get_block_type(
    const Module & module, 
    const RawBlockTree & block_tree)
{
    assert(block_tree.line.token_indices.size() > 0);

    int token_index = block_tree.line.token_indices[0];
    const Token & token = module.token_list[token_index];

    switch(token.type)
    {
        case TokenType::sym_hash_tag:
            throw runtime_error("Unexpected block type. A987907674575");
        case TokenType::kw_func:
            return BlockType::Function;
        case TokenType::kw_var:
            return BlockType::VarDelcare;
        case TokenType::kw_import:
            return BlockType::Import;
        default:
            throw runtime_error("Unhandled block token type. A324598723");
    }
}


static int block_group_head(
    const Module & module, 
    const RawBlockTree & block_tree, 
    const vector<int> & block_group)
{
    assert(block_group.size() > 0);
    for(int i : block_group)
    {
        int first_token_index = get_block_first_token_index(module, block_tree, i);
        const Token & token = module.token_list[first_token_index];

        switch(token.type)
        {
            case TokenType::sym_hash_tag:
                continue; // skip the properties
            case TokenType::kw_func:
            case TokenType::kw_var:
            case TokenType::kw_import:
                return i;
            default:
                throw runtime_error("Unhandled block token type. A4843047");
        }
    }
    return -1;
}


static void separate_block_properties(
    Module & module,
    const RawBlockTree & block_tree,
    const vector<int> & block_group,
    vector<int> & attributes_group,
    vector<int> & nonattributes_group)
{
    attributes_group.clear();
    nonattributes_group.clear();
    for(int i : block_group)
    {
        int first_token_index = get_block_first_token_index(module, block_tree, i);
        const Token & token = module.token_list[first_token_index];

        if(token.type == TokenType::sym_hash_tag)
            attributes_group.push_back(i);
        else
            nonattributes_group.push_back(i);
    }
}


static FunctionParamQualifier token_to_func_param_type(
    const Token & token)
{
    switch(token.type)
    {
        case TokenType::kw_copy:
            return FunctionParamQualifier::Copy;
        case TokenType::kw_const:
            return FunctionParamQualifier::Const;
        case TokenType::kw_cref:
            return FunctionParamQualifier::Cref;
        case TokenType::kw_ref:
            return FunctionParamQualifier::Ref;
        default:
            throw runtime_error("Not implemented. A4302508730");
    }   
}


static bool is_const_func_param(FunctionParamQualifier type)
{
    switch(type)
    {
        case FunctionParamQualifier::Const:
        case FunctionParamQualifier::Cref:
            return true;
        case FunctionParamQualifier::Copy:
        case FunctionParamQualifier::Ref:
            return false;
        default:
            throw runtime_error("Unhandled function param type. A43082750340");
    }
}


static void abstractor_process_type(
    Context & context,
    Module & module,
    TypeDeclAST & param_type,
    const vector<int> & type_token_indices)
{
    (void) context;

    param_type.initialized = true;

    // check pointers
    param_type.pointer_layers = 0;
    for(int i = int(type_token_indices.size()) - 1; i >= 0; i--)
    {
        const Token & token = get_token(module, type_token_indices[i]);
        if(token.text == language::get_pointer_sign())
        {
            param_type.pointer_layers++;
        }
        else
            break;
    }

    // remove the asterisk tokens
    const vector<int> stem_type_token_indices {
        type_token_indices.begin(),
        type_token_indices.end() - param_type.pointer_layers};
    
    if(stem_type_token_indices.size() == 1)
    {
        int token_index = stem_type_token_indices[0];
        const Token & token = get_token(module, token_index);
        BuiltinTypes builtin_type = TokenType_to_BuiltinType(token.type);
        if(builtin_type != BuiltinTypes::Invalid)
        {
            param_type.is_builtin = true;
            param_type.category = TypeCategory::Builtin;
            param_type.builtin_type = builtin_type;
        }
        else
        {
            param_type.is_builtin = false;
            param_type.category = TypeCategory::Class;
            param_type.class_name = token.text;
        }
    }
    else
    {
        // compound type
        throw runtime_error("Not implemented. A870876586");
    }
}


// examples:
//      const x : Int
//      cref  y : String
//      copy  z : Char *
//      ...
static void abstractor_process_function_params_add_input(
    Context & context,
    Module & module,
    const RawBlockTree & param_block,
    FunctionAST & func_ast)
{
    (void) context;
    FuncParamDeclAST func_param;
    func_param.module_index = module.index;
    const vector<int> & token_indices = param_block.line.token_indices;
    assert(token_indices.size() > 0);
    const Token & first_token = module.token_list[token_indices[0]];

    // if variadic function: ...
    if(token_indices.size() == 1 && first_token.type == TokenType::sym_3dots)
    {
        if(func_ast.is_variadic)
            throw runtime_error("The function is already flagged as variadic");
        if(func_ast.output_types.size() > 0)
            throw runtime_error("The function variadic symbol cannot be placed after the output types");

        func_ast.is_variadic = true;
    }
    else // if fixed number of arguments
    {
        if(func_ast.is_variadic)
            throw runtime_error("The function should not take any argument after the variadic symbol. Place the input argument before.");

        func_param.qualifier = token_to_func_param_type(first_token);
        assert(token_indices.size() > 1);
        const Token & second_token = module.token_list[token_indices[1]];
        // todo: second token should be id or -> error
        assert(second_token.category == TokenCategory::id);
        func_param.var_name = second_token.text;
        func_param.is_const = is_const_func_param(func_param.qualifier);
        assert(token_indices.size() >= 4);
        const Token & third_token = module.token_list[token_indices[2]];
        assert(third_token.type == TokenType::sym_colon);
        // todo: second token should be : or -> error
        vector<int> param_type_token_indices = std::vector(token_indices.begin() + 3, token_indices.end());
        assert(func_param.param_type.initialized == false);
        abstractor_process_type(context, module, func_param.param_type, param_type_token_indices);
        func_ast.input_params.push_back(func_param);
    }
}


static void abstractor_process_function_params_add_output(
    Context & context,
    Module & module,
    const RawBlockTree & param_block,
    FunctionAST & func_ast)
{
    const vector<int> & token_indices = param_block.line.token_indices;
    vector<int> all_ret_types = std::vector(token_indices.begin() + 1, token_indices.end());
    vector<vector<int>> each_ret_type_list = token_comma_separate(context, module, all_ret_types);
    for(const vector<int> & each_ret_type : each_ret_type_list)
    {
        TypeDeclAST param_type;
        abstractor_process_type(context, module, param_type, each_ret_type);
        func_ast.output_types.push_back(param_type);
    }
}


static void abstractor_process_function_attributes(
    Context & context,
    Module & module,
    FunctionAST & func_ast)
{
    (void) context;
    (void) module;
    for(const Attribute & attribute: func_ast.attributes)
    {
        if(attribute.attr_name == language::get_sys_decl())
        {
            if(attribute.params.size() > 0)
                throw runtime_error("sys_decle does not take any parameter. A43502987340");
            func_ast.is_sys_decl = true;
            func_ast.is_decl_only = true;
        }
    }
}


static void abstractor_process_function_params(
    Context & context,
    Module & module,
    const RawBlockTree & param_block,
    FunctionAST & func_ast)
{
    for(const vector<int> block_group : param_block.block_groups)
    {
        // At the moment, there is no grouping for function parameters
        assert(block_group.size()==1);

        int block_index = block_group[0];

        int token_index = get_block_first_token_index(module, param_block, block_index);
        const RawBlockTree & child_block = param_block.children[block_index];
        const Token & token = module.token_list[token_index];
        switch(token.type)
        {
            case TokenType::kw_copy:
            case TokenType::kw_const:
            case TokenType::kw_cref:
            case TokenType::kw_ref:
            case TokenType::sym_3dots:
                abstractor_process_function_params_add_input(context, module, param_block.children[block_index], func_ast);
                break;
            case TokenType::sym_arrow:
                abstractor_process_function_params_add_output(context, module, param_block.children[block_index], func_ast);
                break;
            default:
                debug_print(module, child_block);
                throw runtime_error("Not implemented. A34750239");
        }
    }
}


static StatementAST abstractor_process_compound_stmt(
    Context & context,
    Module & module,
    const RawBlockTree & outer_block,
    vector<int> group)
{
    if(group.size() == 1)
    {
        // Here, the child group is made of only one block
        const RawBlockTree & inner0 = outer_block.children[group[0]];
        if(inner0.children.size() == 0)
        {
            // Here, the statement group is made of one line only
            vector<int> token_indices = inner0.line.token_indices;

            if(abstractor_is_stmt_var_decl(context, module, token_indices))
            {
                abstractor_validate_stmt_var_decl(context, module, token_indices);
                return abstractor_process_stmt_var_decl(context, module, token_indices);
            }
            if(abstractor_is_stmt_jmp(context, module, token_indices))
            {
                abstractor_validate_stmt_jmp(context, module, token_indices);
                return abstractor_process_stmt_jmp(context, module, token_indices);
            }
            if(abstractor_can_be_stmt_expression(context, module, token_indices))
            {
                abstractor_validate_stmt_expression(context, module, token_indices);
                return abstractor_process_stmt_expression(context, module, token_indices);
            }
        }
    }

    throw runtime_error("Unkonwn statement. A348750324");
}


static void abstractor_process_function_body(
    Context & context,
    Module & module,
    const RawBlockTree & body_block,
    CompoundStmtAST & compound_stmt)
{
    compound_stmt.module_index = module.index;
    for(vector<int> group : body_block.block_groups)
    {
        StatementAST stmt = abstractor_process_compound_stmt(
            context, module, body_block, group);
        compound_stmt.statements.push_back(stmt);
    }
}


static void abstractor_process_function(
    Context & context,
    Module & module,
    const RawBlockTree & parent_block_tree,
    const vector<int> & block_group)
{
    FunctionAST func_ast;
    func_ast.module_index = module.index;

    vector<int> attributes_group;
    vector<int> nonattributes_group;
    separate_block_properties(module, parent_block_tree, block_group, attributes_group, nonattributes_group);

    // ------------------
    // process attributes
    // vector<Attribute> attributes;
    for(int attribute_block_index : attributes_group)
    {
        const RawBlockTree & attribute_block = parent_block_tree.children[attribute_block_index];
        if(attribute_block.children.size() > 0)
            throw runtime_error("A attribute line cannot have block children");
        const vector<int> attribute_tokens(
                attribute_block.line.token_indices.begin() + 1, // remove the # sign
                attribute_block.line.token_indices.end()
            );
        const vector<vector<int>> each_attribute_indices = token_comma_separate(context, module, attribute_tokens);
        for(const vector<int> & attribute_item : each_attribute_indices)
        {
            assert(attribute_item.size() > 0);
            if(attribute_item.size() == 1)
            {
                const Token & token = get_token(module, attribute_item[0]);
                Attribute attribute;
                attribute.module_index = module.index;
                attribute.token_index = attribute_item[0];
                attribute.attr_name = token.text;
                func_ast.attributes.push_back(attribute);
            }
            else
            {
                throw runtime_error("Not implemented. A384702374");
            }
        }
    }
    abstractor_process_function_attributes(context, module, func_ast);
 
    // by now, attributes are finalized

    assert(nonattributes_group.size() != 0);

    // ------------------
    // function head (name and parameters and return type)
    const RawBlockTree & param_block_tree = parent_block_tree.children[nonattributes_group[0]];
    if(param_block_tree.line.token_indices.size() < 2)
        throw runtime_error("Function name is not specified. A3845204387");
    const Token & function_name_token = get_token(module, param_block_tree.line.token_indices[1]);
    string function_name = function_name_token.text;
    cout << "function detected: " << function_name << endl;

    func_ast.func_name = function_name;
    if(language::is_main_function_name(func_ast.func_name))
        func_ast.is_entry = true;
    abstractor_process_function_params(context, module, param_block_tree, func_ast);


    // ------------------
    // function body
    if(!func_ast.is_decl_only)
    {
        // todo: if block is not func+body -> error
        if(nonattributes_group.size() != 2)
        {
            // todo: generate error
            assert(nonattributes_group.size() == 2);
            return ;
        }

        int expected_body_index = nonattributes_group[1];

        int first_body_token_index = get_block_first_token_index(module, parent_block_tree, expected_body_index);
        const Token & first_body_token = module.token_list[first_body_token_index];
        if(first_body_token.type != TokenType::kw_body)
        {
            // todo: generate error
            assert(first_body_token.type == TokenType::kw_body);
            return ;
        }

        const RawBlockTree & body_block_tree = parent_block_tree.children[nonattributes_group[1]];

        abstractor_process_function_body(context, module, body_block_tree, func_ast.compound_stmt);
    }

    // append the function
    if(func_ast.is_decl_only)
        module.ast.decl_only_functions.push_back(std::move(func_ast));
    else
        module.ast.functions.push_back(std::move(func_ast));
}


void abstractor_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::BlockBuilder)
        module.stage = Stage::AbstractorInit;
    else
        throw runtime_error("Wrong stage. A098362358");

    module.ast.module_index = module.index;

    for(const vector<int> & block_group : module.root_raw_block.block_groups)
    {
        assert(block_group.size() > 0);
        int block_head = block_group_head(module, module.root_raw_block, block_group);
        assert(block_head >=0);
        const RawBlockTree & processing_block_tree = module.root_raw_block.children[block_head];
        switch(get_block_type(module, processing_block_tree))
        {
            case BlockType::Invalid:
                debug_print(module, processing_block_tree);
                throw runtime_error("Code should not reach here. A86389459");
            case BlockType::Import:
                // ignore
                break;
            case BlockType::Function:
                abstractor_process_function(context, module, module.root_raw_block, block_group);
                break;
            case BlockType::Class:
                throw runtime_error("Not implemented. A384975203487");
                break;
            default:
                debug_print(module, processing_block_tree);
                throw runtime_error("Unhandled block type. A473204875");
        }
    }
}


void abstractor_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        abstractor_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
