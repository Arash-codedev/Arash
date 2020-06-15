#pragma once

#include <00_common/protocol.hpp>



class VarDeclParts
{
public:
    vector<int> qualifiers;
    vector<int> name;
    vector<int> initialization;
    vector<int> type;
};


bool abstractor_is_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

void abstractor_validate_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

StatementAST abstractor_process_stmt_var_decl(
    Context & context,
    Module & module,
    const vector<int> & token_indices);

VarDeclParts separate_var_decle_parts(
	Context & context,
	Module & module,
	const vector<int> & token_indices);
