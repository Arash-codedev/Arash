#pragma once

#include <00_common/protocol.hpp>


bool abstractor_is_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

void abstractor_validate_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

StatementAST abstractor_process_stmt_jmp(
    Context & context,
    Module & module,
    const vector<int> & token_indices);
