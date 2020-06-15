#pragma once

#include <00_common/protocol.hpp>


bool abstractor_can_be_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

void abstractor_validate_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

StatementAST abstractor_process_stmt_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);
