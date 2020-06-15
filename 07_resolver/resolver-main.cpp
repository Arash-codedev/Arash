#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// resolver interface:
#include <07_resolver/resolver-interface.hpp>
#include <07_resolver/resolver-actuator.hpp>


// Receive the files from injector and prepare results to be fed to the optimizer
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("injector-out.txt"));
    timer.tic();
    resolver_process(context);
    cout << "Resolver was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("resolver-out.txt", context_text);
    return 0;
}
