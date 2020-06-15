#pragma once

#include <00_common/protocol.hpp>


void streamer_process(Context &context);
Compiler_Parameters parse_arguments(int argc, char** argv);
void streamer_add_module(Context &context, const string & file, const string & content);
