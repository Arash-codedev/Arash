#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>


void reformer_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::Optimizer)
        module.stage = Stage::Reformer;
    else
        throw runtime_error("Wrong stage. A445638743");

    // optimize code ...
}

void reformer_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        reformer_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
