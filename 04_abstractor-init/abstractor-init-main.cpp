#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// abstractor interface:
#include <04_abstractor-init/abstractor-init-interface.hpp>
#include <04_abstractor-init/abstractor-init-actuator.hpp>


// Receive the files from blocker and prepare results to be fed to the pertainer
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("blocker-out.txt"));
    timer.tic();
    abstractor_process(context);
    cout << "Abstracting was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("abstractor-init-out.txt", context_text);
    return 0;
}
