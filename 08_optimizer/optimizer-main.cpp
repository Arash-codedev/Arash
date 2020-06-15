#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// optimizer interface:
#include <08_optimizer/optimizer-interface.hpp>
#include <08_optimizer/optimizer-actuator.hpp>


// Receive the files from resolver and prepare results to be fed to the reformer
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("resolver-out.txt"));
    timer.tic();
    optimizer_process(context);
    cout << "Optimizer was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("optimizer-out.txt", context_text);
    return 0;
}
