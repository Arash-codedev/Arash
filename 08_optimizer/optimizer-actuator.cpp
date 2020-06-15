#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>


void optimizer_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::Resolver)
        module.stage = Stage::Optimizer;
    else
        throw runtime_error("Wrong stage. A38470752343");

    // optimize code ...
}

void optimizer_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        optimizer_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
