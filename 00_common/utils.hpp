#pragma once
#include <light_header>
#include <00_common/protocol.hpp>

bool is_effective(const Token & token);
void add_ordinary_error(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message);
void add_ordinary_error(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message);
void add_fatal_error(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message);
void add_fatal_error(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message);
void add_warning(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message);
void add_warning(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message);
bool has_any_error(const Module & module);
bool has_any_error(const Context & context);
int error_count(const Context & context);
int warning_count(const Context & context);
Token get_token(const Module & module, int token_index);
string canonical_path(string token_path_text);
string canonical_path(BuiltinModule module);
void check_for_errors(const Context & context);
void print_notifications(const Context & context);
