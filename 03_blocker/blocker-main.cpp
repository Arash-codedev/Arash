#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <libs/profiler.hpp>
#include <sstream>
// blocker interface:
#include <03_blocker/blocker-interface.hpp>
#include <03_blocker/blocker-actuator.hpp>


static void block_piped(
    std::stringstream & stream,
    const RawBlockTree & block,
    int level,
    string line_prefix,
    bool is_last_child,
    const Module & module)
{
    if(level > 0)
    {
        stream << line_prefix;

        if(is_last_child)
            stream << "\\-";
        else
            stream << "|-";

        assert(block.line.token_indices.size() > 0);
        for(int i = 0; i< int(block.line.token_indices.size()); i++)
            stream << get_token(module, block.line.token_indices[i]).text << " ";
        stream << ": [`" << block.children.size() << "]" << endl;

        if(is_last_child)
            line_prefix += "    ";
        else
            line_prefix += "|   ";
    }

    bool first_group = true;
    for(const vector<int> & group : block.block_groups)
    {
        // group separator
        if(!first_group)
        {
            stream << line_prefix << "    .----" << endl;
        }
        first_group = false;
        for(int block_index : group)
        {
            const RawBlockTree & child = block.children[block_index];
            bool is_last_inner_block = true;
            for(int j = block_index + 1; j < int(block.children.size()); j++)
            {
                if(block.children[j].children.size())
                    is_last_inner_block = false;
            }
            string index_str = "[*"s + std::to_string(block_index) + "]"s;
            block_piped(stream, child, level + 1, line_prefix + index_str, is_last_inner_block, module);
        }
    }
}

// Receive the files from tokenizer and prepare results to be fed to the abstractor-init
int main()
{
    cronometer timer;
    Context context = context_from_string(filesystem::read_file("tokenizer-out.txt"));
    timer.tic();
    blocker_process(context);
    cout << "Blocking was performed in " << timer.toc() << " seconds." <<endl;
    check_for_errors(context);
    print_notifications(context);
    // create a JSON serialized context
    string context_text = to_string(context);
    filesystem::write_file("blocker-out.txt", context_text);
    // reconstruct the source code from the blocks
    std::stringstream stream_source;
    for(const Module & module : context.modules)
    {
        stream_source << "Module: " << module.filename << endl;
        block_to_sourcecode(stream_source, module.root_raw_block, 0, module);
    }
    filesystem::write_file("blocker-reconstructed-sourcecode.txt", stream_source.str());
    // create a readable overview of the output structure
    std::stringstream stream_piped;
    for(const Module & module : context.modules)
    {
        stream_piped << "Module: " << module.filename << endl;
        block_piped(stream_piped, module.root_raw_block, 0, "", false, module);
    }
    filesystem::write_file("blocker-out-piped.txt", stream_piped.str());
    return 0;
}
