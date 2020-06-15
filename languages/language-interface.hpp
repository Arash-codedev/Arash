#pragma once
#include <00_common/protocol.hpp>
#include <light_header>

#define NS_LANGUAGE_BEGIN namespace language{
#define NS_LANGUAGE_END }

NS_LANGUAGE_BEGIN

bool get_next_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);

bool is_all_space(const string & token);
int space_count(const string & token);
bool get_builtin_module(const string & token, BuiltinModule & builtin_module);
string translate_notification(const Context & context, const Notification & notification);
bool is_main_function_name(const string & func_name);
string artifacts_folder_name();
string object_file_extention();
string ir_file_extention();
string get_sys_decl();
string get_pointer_sign();

NS_LANGUAGE_END
