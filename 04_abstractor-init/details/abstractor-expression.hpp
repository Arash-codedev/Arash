#pragma once

#include <00_common/protocol.hpp>


bool abstractor_can_be_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

void abstractor_validate_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

ExpressionAST abstractor_process_expression(
    Context & context,
    Module & module,
    const vector<int> & token_indices);
