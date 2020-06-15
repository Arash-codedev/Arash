#include <01_streamer/streamer-actuator.hpp>
#include <00_common/utils.hpp>
#include <00_common/protocol.hpp>
#include <libs/filesystem.hpp>
#include <light_header>


void streamer_process_ensure_file_exists(const string &file)
{
    if(!fs::exists(file))
    {
        cout << "Module file \"" + file + "\" does not exist." << endl;
        exit(1);
    }
}

void streamer_add_module(Context &context, const string & file, const string & content)
{
    for(const Module &m : context.modules)
        if(m.filename == file)
            return ; // module is already added. So, do nothing.

    Module module;
    module.filename = file;
    module.filepath = canonical_path(file);
    module.index = (int) context.modules.size(); // index to be added
    module.stage = Stage::Streamer;
    module.content = content;
    string line = "";
    char last_c = 0;
    for(char c : content)
    {
        if(c == '\n')
        {
            if(last_c != '\r')
            {
                module.text_lines.push_back(line);
                line = "";
            }
        }
        else if (c == '\r')
        {
            module.text_lines.push_back(line);
            line = "";
        }
        else
            line += c;
        last_c = c;
    }
    if(line.length() > 0)
    {
        module.text_lines.push_back(line);
    }
    context.modules.push_back(std::move(module));
}

void streamer_process(Context &context)
{
    for(const string &file : context.compiler_params.input_files)
    {
        streamer_process_ensure_file_exists(file); // check the file existence at early stage first
    }
    if(context.compiler_params.input_files.size()==0)
    {
        Module dummy;
        add_fatal_error(context, dummy, Token(), {}, "No input file is given");
        (void) dummy;
    }
    for(const string &file : context.compiler_params.input_files)
    {
        streamer_process_ensure_file_exists(file); // check the file existence again
        string content = filesystem::read_file(file);
        streamer_add_module(context, file, content);
    }
}
