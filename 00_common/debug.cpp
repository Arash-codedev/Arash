#include "debug.hpp"
#include "utils.hpp"



void not_implemented(
    const Module & module,
    const vector<int> & token_indices,
    const string & error_marker)
{
    code_crash(module, token_indices, "Not implemented.", error_marker);
}

void code_crash(
    const Module & module,
    const vector<int> & token_indices,
    const string & error_message,
    const string & error_marker)
{
    bool first = true;
    cout << "Tokens: ";
    for(int index : token_indices)
    {
        const Token & token = get_token(module, index);
        if(!first)
            cout << ", ";
        cout << "(" << token.text<< ")";
        cout << "[" << index << "] ";
        first = false;
    }
    cout << endl;
    throw runtime_error(error_message + " " + error_marker);
}
