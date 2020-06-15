#include "utils.hpp"
#include <languages/language-interface.hpp>


bool is_effective(const Token & token)
{
    if(token.type == TokenType::spaces || token.type == TokenType::comment)
        return false;
    else
        return true;
}

bool has_any_error(Module & module)
{
    return module.fatal_error_count || module.ordinary_error_count;
}

void add_ordinary_error(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message)
{
    module.ordinary_error_count++;
    Notification n;
    n.location = token_location.location;
    for(const Token & aux_token : token_aux_locations)
        n.auxiliary_locations.push_back(aux_token.location);
    n.is_fatal = false;
    n.stage = module.stage;
    n.message_text = error_message;
    n.type = NotificationType::Error;
    context.notifications.push_back(n);
}

void add_ordinary_error(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message)
{
    add_ordinary_error(context, module, get_token(module, token_index), token_aux_locations, error_message);
}

void add_fatal_error(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message)
{
    module.fatal_error_count++;
    Notification n;
    n.location = token_location.location;
    for(const Token & aux_token : token_aux_locations)
        n.auxiliary_locations.push_back(aux_token.location);
    n.is_fatal = true;
    n.stage = module.stage;
    n.message_text = error_message;
    n.type = NotificationType::Error;
    context.notifications.push_back(n);
}

void add_fatal_error(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message)
{
    add_fatal_error(context, module, get_token(module, token_index), token_aux_locations, error_message);
}

void add_warning(Context & context, Module & module, const Token & token_location, const vector<Token> & token_aux_locations, const string & error_message)
{
    module.warning_count++;
    (void) context;
    (void) token_location;
    (void) token_aux_locations;
    (void) error_message;
    throw runtime_error("Not implemented. A238603870");
}

void add_warning(Context & context, Module & module, int token_index, const vector<Token> & token_aux_locations, const string & error_message)
{
    add_warning(context, module, get_token(module, token_index), token_aux_locations, error_message);
}

int error_count(const Context & context)
{
    int count = 0;
    for(const Notification & notification : context.notifications)
    {
        if(notification.type == NotificationType::Error)
            count++;
    }
    return count;
}

int warning_count(const Context & context)
{
    int count = 0;
    for(const Notification & notification : context.notifications)
    {
        if(notification.type == NotificationType::Warning)
            count++;
    }
    return count;
}

Token get_token(const Module & module, int token_index)
{
    assert(token_index >= 0);
    assert(token_index < int(module.token_list.size()));
    return module.token_list[token_index];
}

string canonical_path(string token_path_text)
{
    fs::path result = fs::weakly_canonical(token_path_text);
    if(result == "")
    {
        cout << "Invalid canonical path for token path text: " << token_path_text << endl;
        throw runtime_error("Invalid canonical path. A24385023847");
    }
    return result;
}

string canonical_path(BuiltinModule module)
{
cout<<"warning: function to be modified."<<endl;
    fs::path base = fs::current_path();
    fs::path target;
    switch(module)
    {
        case BuiltinModule::invalid:
            throw runtime_error("Invalid builtin module. A8260238740");
            break;
        case BuiltinModule::std:
            target = "std";
            break;
        case BuiltinModule::fs:
            target = "fs";
            break;
        case BuiltinModule::os:
            target = "os";
            break;
        default:
            throw runtime_error("Not implemented. A28340523746");
    }
    return fs::relative(target, base);
}

void check_for_errors(const Context & context)
{
    int count = error_count(context);
    if(count == 1)
        cout<<"Code has an error."<<endl;
    if(count > 1)
        cout<<"Code has "<<count<<" errors."<<endl;
}

void print_notifications(const Context & context)
{
    for(const Notification & n : context.notifications)
    {
        cout<< language::translate_notification(context, n) << endl;
    }
}
