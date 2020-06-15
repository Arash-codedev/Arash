#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>


void resolver_process_module(Context & context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::Injector)
        module.stage = Stage::Resolver;
    else
        throw runtime_error("Wrong stage. A309845203874");

    // resolve ....
}

void resolver_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        resolver_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
