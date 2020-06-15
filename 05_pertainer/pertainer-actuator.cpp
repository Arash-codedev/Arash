#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>


static void pertainer_process_module_block(Context & context, Module & module, const RawBlockTree & block_tree, int depth)
{ 
   if(depth > 0) // ignore the root
    {
        assert(block_tree.line.token_indices.size() > 0);
        const Token & first_token = module.token_list[block_tree.line.token_indices[0]];
        if(first_token.type == TokenType::kw_import)
        {
            if(block_tree.line.token_indices.size() < 2)
            {
                add_fatal_error(context, module, first_token, {}, "No module/path is specified for the import.");
                return ;
            }
            const Token & includee = module.token_list[block_tree.line.token_indices[1]];
            switch(includee.category)
            {
                case TokenCategory::id:
                    {
                        BuiltinModule builtin_module = BuiltinModule::invalid;
                        if(language::get_builtin_module(includee.text, builtin_module))
                        {
                            assert(builtin_module != BuiltinModule::invalid);
                            module.pertained_builtin_modules.insert(builtin_module);
                            break;
                        }
                        else
                        {
                            throw runtime_error("Not implemented (module inclusion). A2838089848");
                        }
                    }
                    break;
                case TokenCategory::literal:
                    if(includee.type != TokenType::literal_string)
                    {
                        add_fatal_error(context, module, first_token, {}, "An import literal item is expected to be a string.");
                        return ;
                    }
                    {
                        string path = canonical_path(includee.encoded_text);
                        module.pertained_modules.insert(path);
                    }
                    break;
                default:
                    add_fatal_error(context, module, first_token, {}, "Invalid import token.");
                    return ;
            }
        }
    }
    for(const RawBlockTree & child_block : block_tree.children)
        pertainer_process_module_block(context, module, child_block, depth + 1);
}

void pertainer_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::AbstractorInit)
        module.stage = Stage::Pertainer;
    else
        throw runtime_error("Wrong stage. A3984720634");

    pertainer_process_module_block(context, module, module.root_raw_block, 0);
}

void pertainer_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        pertainer_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
