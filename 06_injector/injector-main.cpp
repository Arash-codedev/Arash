#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// injector interface:
#include <01_streamer/streamer-interface.hpp>
#include <06_injector/injector-interface.hpp>


// Receive the files from pertainer and prepare results to be fed to the resolver
int main(int argc, char** argv)
{
    cronometer timer;
    Compiler_Parameters params = parse_arguments(argc,argv);
    Context context;
    context.compiler_params = params;
    timer.tic();
    streamer_process(context); // if removed, the output will be empty
    injector_process(context);
    cout << "Injecting was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    print_notifications(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("injector-out.txt", context_text);
    return 0;
}
