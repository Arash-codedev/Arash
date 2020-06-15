#include <00_common/serializer.hpp>
#include <libs/profiler.hpp>
#include <libs/filesystem.hpp>
// streamer interface:
#include <01_streamer/streamer-interface.hpp>


// Receive the user files and prepare their content to be fed to the separator
int main(int argc, char** argv)
{
    cronometer timer;
    timer.tic();
    Compiler_Parameters params = parse_arguments(argc,argv);
    Context context;
    context.compiler_params = params;
    streamer_process(context);
    cout<< "Streaming was performed in " << timer.toc() << " seconds." <<endl;
    string context_text = to_string(context);
    filesystem::write_file("streamer-out.txt", context_text);

    return 0;
}
