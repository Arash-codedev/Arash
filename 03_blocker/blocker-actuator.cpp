#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <00_common/helper.hpp>
#include <languages/language-interface.hpp>



static void trim_uneffective_parts(Context &context, Module & module, vector<TokenLine> & token_lines_trimmed)
{
    (void) context;

    TokenLine trimmed_line;

    for(const TokenLine & itkline : module.token_lines)
    {
        trimmed_line = TokenLine();
        for(int token_index : itkline.token_indices)
        {
            const Token & tk = get_token(module, token_index);
            if(is_effective(tk))
            {
                trimmed_line.token_indices.push_back(token_index);
                if(trimmed_line.indentation < 0)
                    trimmed_line.indentation = itkline.indentation;
            }
        }
        if(trimmed_line.token_indices.size() > 0)
            token_lines_trimmed.push_back(trimmed_line);
    }
}

static vector<TokenLine> trim_multi_lines(const vector<TokenLine> & token_lines_in, Context &context, Module & module)
{
    vector<TokenLine> token_lines_out;
    TokenLine line;
    bool continue_line = false;
    int last_token_index = -1;

    for(const TokenLine & it_line : token_lines_in)
    {
        if(!continue_line)
        {
            line = TokenLine();
            line.indentation = it_line.indentation;
        }
        for(int token_index : it_line.token_indices)
            line.token_indices.push_back(token_index);
        assert(it_line.token_indices.size() > 0);
        const Token last_token = (last_token_index >= 0 ? get_token(module, last_token_index) : Token());
        continue_line = false;
        if(last_token.type == TokenType::continuation)
        {
            continue_line = true;
            // remove the slash from the end of the current line
            line.token_indices.pop_back();
        }
        if(!continue_line && line.token_indices.size() > 0)
        {
            token_lines_out.push_back(line);
            line.token_indices.clear();
        }
    }
    if(line.token_indices.size() > 0)
    {
        token_lines_out.push_back(line);
    }
    if(continue_line)
    {
        const Token last_token = get_token(module, last_token_index);
        add_fatal_error(context, module, last_token, {}, "Line is continueing at the end of the module");
    }
    return token_lines_out;
}

static vector<TokenLine> trim_parenthesis_multi_lines(
    const vector<TokenLine> & token_lines_in,
    Context & context,
    Module & module)
{
    vector<TokenLine> token_lines_out;
    TokenLine cached_line;

    vector<ContainerSpot> container_stack;

    verify_container_set(container_recognition_set);

    for(const TokenLine & it_line : token_lines_in)
    {
        assert(it_line.token_indices.size() > 0);
        if(!container_stack.size())
        {
            // On a new line with no dependency on the previous lines
            if(cached_line.token_indices.size() > 0)
            {
                token_lines_out.push_back(cached_line);
                cached_line.token_indices.clear();
            }
            cached_line = TokenLine();
            cached_line.indentation = it_line.indentation;
        }

        for(int token_index : it_line.token_indices)
        {
            const Token token = get_token(module, token_index);
            cached_line.token_indices.push_back(token_index);
            for(const ContainerRecognition & cr : container_recognition_set)
            {
                if(cr.container_begin == token.type)
                {
                    container_stack.push_back(ContainerSpot(cr.container_operator, token_index));
                }
                else if(cr.container_end == token.type)
                {
                    if(!container_stack.size())
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
                        container_stack.pop_back();
                    }
                }
            }
        }
        assert(it_line.token_indices.size() > 0);
    }
    if(cached_line.token_indices.size() > 0)
    {
        token_lines_out.push_back(cached_line);
    }
    if(container_stack.size() > 0)
    {
        int err_token_index = container_stack.back().token_index; 
        add_fatal_error(context, module, err_token_index, { }, "Container opened but not closed while reaching to the end of the module");
        return {};
    }
    return token_lines_out;
}

// put every line to a block based on its indentation
static void build_generic_block_tree(Context &context, Module & module, const vector<TokenLine> & lines)
{
    // iterate over every line of the module
    for(int i = 0; i < int(lines.size()); i++)
    {
        const TokenLine & line = lines[i];
        assert(line.token_indices.size() > 0);
        int current_indent = line.indentation;
        RawBlockTree * p_parent = &(module.root_raw_block);
        // locate the correct parent
        for(int i = 0; i < current_indent; i++)
        {
            if(p_parent->children.size() == 0)
            {
                add_fatal_error(context, module, get_token(module, line.token_indices[0]), {}, "Line is indented too much.");
                return ;
            }
            p_parent = &(p_parent->children.back());
        }
        RawBlockTree new_child;
        new_child.line = line;
        p_parent->children.push_back(new_child);
    }
}

static void build_block_groups(Module & module, RawBlockTree & block_tree, int depth)
{
    TokenType last = TokenType::unknown;
    bool coherent = false;
    vector<int> group_cache;

    for(int bt_index = 0; bt_index < (int) block_tree.children.size(); bt_index++)
    {
        RawBlockTree & rbt_child = block_tree.children[bt_index];
        coherent = false;
        const TokenLine & token_line = rbt_child.line;
   
        assert(token_line.token_indices.size() > 0);
        const Token & token = module.token_list[token_line.token_indices[0]];

        if(last == TokenType::sym_hash_tag)
            coherent = true;
        else if(last == TokenType::sym_at)
            coherent = true;
        else if(token.type == TokenType::kw_body)
            coherent = true;

        if(!coherent)
        {
            if(group_cache.size() > 0)
            {
                block_tree.block_groups.push_back(group_cache);
                group_cache.clear();
                last = TokenType::unknown;
            }
        }
        group_cache.push_back(bt_index);

        last = token.type;

        build_block_groups(module, rbt_child, depth + 1);
    }

    if(group_cache.size() > 0)
    {
        block_tree.block_groups.push_back(group_cache);
    }
}

static void verify_block_groups(Module & module, const RawBlockTree & block_tree, int depth)
{
    assert(block_tree.block_groups.size() > 0 || block_tree.children.size() == 0);
    int count = 0;
    for(const vector<int> & b_set : block_tree.block_groups)
    {
        for(int b_idx : b_set)
        {
            assert(count == b_idx);
            count++;
        }
    }
    assert(count == (int) block_tree.children.size());

    for(const RawBlockTree & rbt_child : block_tree.children)
        verify_block_groups(module, rbt_child, depth + 1);
}

void blocker_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::Tokenizer)
        module.stage = Stage::BlockBuilder;
    else
        throw runtime_error("Wrong stage. A20499650424");

    vector<TokenLine> trimmed_lines;
    trim_uneffective_parts(context, module, trimmed_lines);
    trimmed_lines = trim_multi_lines(trimmed_lines, context, module);
    trimmed_lines = trim_parenthesis_multi_lines(trimmed_lines, context, module);
    // Now, there is no multiple lines nor multi-line parentheses
    build_generic_block_tree(context, module, trimmed_lines);
    build_block_groups(module, module.root_raw_block, 0);
    verify_block_groups(module, module.root_raw_block, 0);
}

void blocker_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        blocker_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}

void block_to_sourcecode(std::stringstream & stream, const RawBlockTree & block, int level, const Module & module)
{
    if(level > 0 || block.line.token_indices.size())
    {
        for(int i = 0; i < 4 * block.line.indentation; i++)
            stream << " ";
        string sep = "";
        for(int token_index : block.line.token_indices)
        {
            const Token token = get_token(module, token_index);
            stream << sep << token.text;
            sep = " ";
        }
        stream << endl;
    }
    for(const RawBlockTree & child : block.children)
    {
        block_to_sourcecode(stream, child, level + 1, module);
    }
}
