#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// pertainer interface:
#include <05_pertainer/pertainer-interface.hpp>
#include <05_pertainer/pertainer-actuator.hpp>


// Receive the files from abstractor-init and prepare results to be fed to the injector
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("abstractor-init-out.txt"));
    timer.tic();
    pertainer_process(context);
    cout << "Pertaining was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("pertainer-out.txt", context_text);
    return 0;
}
