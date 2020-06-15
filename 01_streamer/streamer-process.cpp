#include <01_streamer/streamer-interface.hpp>
#include <libs/easytext.hpp>

using easytext::starts_with;


Compiler_Parameters parse_arguments(int argc, char** argv)
{
    vector<string> args;
    for(int i = 1; i < argc; ++i)
        args.push_back(argv[i]);

    vector<vector<string>> arg_chunks;
    vector<string> arg_chunk;
    bool next_is_dependent = false;
    for(const auto& arg:args)
    {
        arg_chunk.push_back(arg);
        if(starts_with(arg, "-")) // should be smarter
            next_is_dependent = true;
        else
            next_is_dependent = false;
        if(!next_is_dependent)
        {
            arg_chunks.push_back(arg_chunk);
            arg_chunk.clear();
        }
    }
    if(arg_chunk.size() > 0)
        arg_chunks.push_back(arg_chunk);
    // process(arg_chunks);
    Compiler_Parameters params;
    params.output_file = "aaa.txt";
    for(const vector<string> &arg:arg_chunks)
    {
        if(arg.size() == 2)
        {
            if(arg[0] == "-o")
            {
                params.output_file = arg[1];
            }
            else
            {
                cout<<"Unkown argument switch \""<<arg[0]<<"\""<<endl;
                exit(1);
            }
        }
        else if(arg.size() == 1)
        {
           params.input_files.push_back(arg[0]);
        }
        else
        {
            cout<<"Unkown error. A7304659472"<<endl;
            exit(1);
        }
    }
    return params;
}
