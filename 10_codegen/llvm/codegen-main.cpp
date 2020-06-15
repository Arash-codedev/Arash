#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// codegen interface:
#include <10_codegen/llvm/codegen-interface.hpp>
#include <10_codegen/llvm/codegen-actuator.hpp>


// Receive the files from optimizer and create the output binary
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("optimizer-out.txt"));
    timer.tic();
    codegen_process(context);
    cout << "Codegen was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("codegen-out.txt", context_text);
    return 0;
}
