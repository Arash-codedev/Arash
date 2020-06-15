#pragma once

#include <00_common/protocol.hpp>


void block_to_sourcecode(std::stringstream & stream, const RawBlockTree & block, int level, const Module & module);
void blocker_process_module(Context & context, int module_index);
