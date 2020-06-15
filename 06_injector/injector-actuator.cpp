#include <light_header>
#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <languages/language-interface.hpp>
#include <01_streamer/streamer-interface.hpp>
#include <02_tokenizer/tokenizer-interface.hpp>
#include <03_blocker/blocker-interface.hpp>
#include <04_abstractor-init/abstractor-init-interface.hpp>
#include <05_pertainer/pertainer-interface.hpp>


static bool module_already_exists(const Context & context, const string & filepath)
{
    for(const Module & module : context.modules)
    {
        if(module.filepath == filepath)
            return true;
    }
    return false;
}

void injector_process_module(Context & context, int module_index)
{
    if(context.modules[module_index].stage == Stage::Pertainer)
        context.modules[module_index].stage = Stage::Injector;
    else
        throw runtime_error("Wrong stage. A34782703487");

// Do not delete this code. Implement later.
    // for(BuiltinModule bi_module : context.modules[module_index].pertained_builtin_modules)
    // {
    //     string path = canonical_path(bi_module);
    //     module.pertained_modules.insert(path);
    // }

    set<string> pertained_modules = context.modules[module_index].pertained_modules; // Keep a copy. The loop migh invalidate the container
    for(const string & file : pertained_modules)
    {
        if(!module_already_exists(context, file))
        {
            if(fs::exists(file))
            {
                string content = filesystem::read_file(file);
                streamer_add_module(context, file, content);
            }
            else
            {
                // If the module file is not found, the pertainer should have already 
                // issued an error. Therefore, issuing an error in this block is just
                // an inaccurate backup for that one.

                const string error_message = "Cannot find an included module file \"" + file + "\"";
                Location location;
                location.file_index = module_index;
                location.line = -1; // The error line is not trivially known
                location.column = -1;
                Notification n;
                n.location = location;
                n.is_fatal = true;
                n.stage = context.modules[module_index].stage;
                n.message_text = error_message;
                n.type = NotificationType::Error;
                context.notifications.push_back(n);
            }
        }
    }
}

/*
    The task of this function is to receive an unprocessed
    module and processes it up to the pertainer stage.
*/
static void injector_prepare_module(Context & context, int module_index)
{
    // Module & module = context.modules[module_index];
    switch(context.modules[module_index].stage)
    {
        case Stage::Unprocessed:
        case Stage::Streamer:
        case Stage::Tokenizer:
        case Stage::BlockBuilder:
        case Stage::AbstractorInit:
            // only these stages are acceptable
            break;
        default:
            throw runtime_error("Unprocessed module is encountered. A38640235098");
    }

    while(!has_any_error(context.modules[module_index])
        && context.modules[module_index].stage != Stage::Pertainer)
    {
        switch(context.modules[module_index].stage)
        {
            case Stage::Unprocessed:
                // Apply Streamer
                throw runtime_error("Unprocessed module is encountered. A84677262405");
                break;
            case Stage::Streamer:
                // Apply tokenizer
                tokenizer_process_module(context, module_index);
                break;
            case Stage::Tokenizer:
                // Apply blocker
                blocker_process_module(context, module_index);
                break;
            case Stage::BlockBuilder:
                // Apply abstractor-init
                abstractor_process_module(context, module_index);
                break;
            case Stage::AbstractorInit:
                // Apply pertainer
                pertainer_process_module(context, module_index);
                break;
            case Stage::Pertainer:
                // Do nothing; everything is now prepared
                break;
            case Stage::Injector:
                throw runtime_error("Code should not reach here. A8734074334");
                break;
            case Stage::Finalized:
                // fine
                break;
            default:
                throw runtime_error("Unhandled module stage. A2860469348");
        }
    }
    if(has_any_error(context.modules[module_index]))
        context.modules[module_index].stage = Stage::Finalized;
}

void injector_process(Context &context)
{
    // warning: the size of context.modules can change
    // in this step. So, a module reference will not be
    // valid.

    // process added modules
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        // Module & module = context.modules[i];
        if(has_any_error(context.modules[i]))
            break;
        if(context.modules[i].stage == Stage::Unprocessed || context.modules[i].stage == Stage::Streamer)
            injector_prepare_module(context, i);
        if(context.modules[i].stage != Stage::Finalized)
            injector_process_module(context, i);

        if(has_any_error(context.modules[i]))
            context.modules[i].stage = Stage::Finalized;
    }
}
