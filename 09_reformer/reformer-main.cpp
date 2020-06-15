#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// reformer interface:
#include <09_reformer/reformer-interface.hpp>
#include <09_reformer/reformer-actuator.hpp>


// Receive the files from optimizer and prepare results to be fed to the code generator
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("optimizer-out.txt"));
    timer.tic();
    reformer_process(context);
    cout << "reformer was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("reformer-out.txt", context_text);
    return 0;
}
