#include <00_common/serializer.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
#include <00_common/utils.hpp>
// tokenizer interface:
#include <02_tokenizer/tokenizer-interface.hpp>
#include <02_tokenizer/tokenizer-actuator.hpp>


// Receive the files from streamer and prepare results to be fed to the blocker
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("streamer-out.txt"));
    timer.tic();
    tokenizer_process(context);
    cout << "Separating was performed in " << timer.toc() << " seconds." <<endl;
    string context_text = to_string(context);
    filesystem::write_file("tokenizer-out.txt", context_text);
    std::stringstream stream_piped;
    for(const Module & module : context.modules)
    {
        stream_piped << "Module: " << module.filename << endl;
        for(const TokenLine & token_line : module.token_lines)
        {
            stream_piped << token_line.indentation << "i |";
            string tokenizer_pipe = "";
            for(int token_index : token_line.token_indices)
            {
                const Token & token = get_token(module, token_index);
                stream_piped << tokenizer_pipe;
                stream_piped << token.text;
                tokenizer_pipe = "|";
            }
            stream_piped << endl;
        }
    }
    filesystem::write_file("tokenizer-out-piped.txt", stream_piped.str());
    return 0;
}
