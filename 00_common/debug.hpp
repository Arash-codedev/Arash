#pragma once
#include <light_header>
#include <00_common/protocol.hpp>



void not_implemented(
    const Module & module,
    const vector<int> & token_indices,
    const string & error_marker);

void code_crash(
    const Module & module,
    const vector<int> & token_indices,
    const string & error_message,
    const string & error_marker);
