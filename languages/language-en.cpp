#include <languages/language-interface.hpp>
#include <00_common/protocol.hpp>
#include <cctype>
#include <libs/easytext.hpp>
#include <00_common/utils.hpp>

// Language language;

NS_LANGUAGE_BEGIN

using easytext::starts_with;



struct TokenInterpretation
{
    string text;
    TokenType type;
};



static void get_next_space_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_string_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_hexnumber_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_decimalnumber_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_id_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_line_comment_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_segment_comment_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_symbol_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static void get_next_continuation_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter);
static TokenType get_symbol_token_type(
    const string & symbol_text,
    Context & context,
    Module & module,
    const Location & location);
static bool is_id(char ch);
static bool is_num09(char ch);
static bool is_symbol(char ch);
static void get_id_token_type_category(const string & token_text, TokenType & token_type, TokenCategory & token_category);



static const vector<TokenInterpretation> keyword_list = {
    { "body", TokenType::kw_body },
    { "break", TokenType::kw_break },
    { "case", TokenType::kw_case },
    { "class", TokenType::kw_class },
    { "const", TokenType::kw_const },
    { "copy", TokenType::kw_copy },
    { "var", TokenType::kw_var },
    { "cref", TokenType::kw_cref },
    { "ref", TokenType::kw_ref },
    { "continue", TokenType::kw_continue },
    { "func", TokenType::kw_func },
    { "for", TokenType::kw_for },
    { "if", TokenType::kw_if },
    { "import", TokenType::kw_import },
    { "return", TokenType::kw_return },
    { "switch", TokenType::kw_switch },
    { "type", TokenType::kw_type },
    { "while", TokenType::kw_while },
};

static const vector<TokenInterpretation> builtin_datatype_list = {
    { "Bool", TokenType::vt_Bool },
    { "Char", TokenType::vt_Char },
    { "Int", TokenType::vt_Int },
    { "Int16", TokenType::vt_Int16 },
    { "Int32", TokenType::vt_Int32 },
    { "Int64", TokenType::vt_Int64 },
    { "Int8", TokenType::vt_Int8 },
    { "String", TokenType::vt_String },
    { "UInt", TokenType::vt_UInt },
    { "UInt16", TokenType::vt_UInt16 },
    { "UInt32", TokenType::vt_UInt32 },
    { "UInt64", TokenType::vt_UInt64 },
    { "UInt8", TokenType::vt_UInt8 },
};

static const string all_symbols = "!#%&()*+,-./:;<=>@[]^|~";

static const vector<TokenInterpretation> symbol_list = {
    { "!", TokenType::sym_not },
    { "#", TokenType::sym_hash_tag },
    { "%", TokenType::sym_percent },
    { "%=", TokenType::sym_perenct_equal },
    { "&", TokenType::sym_ampersand },
    { "&&", TokenType::sym_ampersand_ampersand },
    { "&=", TokenType::sym_ampersand_equal },
    { "(", TokenType::sym_parenthesis_open },
    { ")", TokenType::sym_parenthesis_close },
    { "*", TokenType::sym_asterisk },
    { "*=", TokenType::sym_asterisk_equal },
    { "+", TokenType::sym_plus },
    { "++", TokenType::sym_plus_plus },
    { "+=", TokenType::sym_plus_equal },
    { ",", TokenType::sym_comma },
    { "-", TokenType::sym_minus },
    { "--", TokenType::sym_minus_minus },
    { "-=", TokenType::sym_minus_equal },
    { "->", TokenType::sym_arrow },
    { ".", TokenType::sym_dot },
    { "...", TokenType::sym_3dots },
    { "/", TokenType::sym_slash },
    { "/=", TokenType::sym_slash_equal },
    { ":", TokenType::sym_colon },
    { ":=", TokenType::sym_colon_equal },
    { ";", TokenType::sym_semicolon },
    { "<", TokenType::sym_less },
    { "<<", TokenType::sym_shift_left },
    { "<<=", TokenType::sym_shift_left_equal },
    { "<=", TokenType::sym_less_equal },
    { "<=>", TokenType::sym_spaceship },
    { "=", TokenType::sym_equal },
    { "==", TokenType::sym_equal_equal },
    { ">", TokenType::sym_greater },
    { ">=", TokenType::sym_greater_equal },
    { ">>", TokenType::sym_shift_right },
    { ">>=", TokenType::sym_shift_right_equal },
    { "@", TokenType::sym_at },
    { "[", TokenType::sym_squared_bracket_open },
    { "]", TokenType::sym_squared_bracket_close },
    { "{", TokenType::sym_curly_brace_open },
    { "}", TokenType::sym_curly_brace_close },
    { "^", TokenType::sym_xor },
    { "^=", TokenType::sym_xor_equal },
    { "|", TokenType::sym_pipe },
    { "|=", TokenType::sym_pipe_equal },
    { "||", TokenType::sym_pipe_pipe },
    { "~", TokenType::sym_bit_not },
};

static const char escape_character = '\\';
static const char continuation_token = '\\';


static const vector<std::pair<char, char>> escape_list = {
    {'n', '\n'},
    {'t', '\t'},
    {'r', '\r'},
    {'\\', '\\'},
};

static const vector<std::pair<string, BuiltinModule>> builtin_module_list = {
    {"std", BuiltinModule::std},
    {"fs", BuiltinModule::fs},
    {"os", BuiltinModule::os},
};

bool get_next_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    if(next_column < 0)
        throw runtime_error("Negative column index is given. A304987530");
    if(next_column >= int(text_line.length()))
        return false;

    char current_char = text_line[next_column];
    const string from_now = text_line.substr(next_column);

    if(current_char == ' ')
        get_next_space_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(starts_with(from_now, "//"))
        get_next_line_comment_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(starts_with(from_now, "/*"))
        get_next_segment_comment_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(current_char == '"' || current_char == '\'')
        get_next_string_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(starts_with(from_now, "0x"))
        get_next_hexnumber_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(is_num09(current_char))
        get_next_decimalnumber_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(is_id(current_char))
        get_next_id_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(is_symbol(current_char))
        get_next_symbol_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(next_column == int(text_line.length()) - 1 && current_char == continuation_token)
        get_next_continuation_token(text_line, next_column, new_token, context, module, location, id_counter);
    else if(current_char == '\t')
    { // tab is an illegal character
        module.fatal_error_count++;
        Notification n;
        n.location = location;
        n.is_fatal = true;
        n.stage = module.stage;
        n.message_text = "Tab is an illegal character";
        n.type = NotificationType::Error;
        context.notifications.push_back(n);
    }
    else
    {
        module.fatal_error_count++;
        Notification n;
        n.location = location;
        n.is_fatal = true;
        n.stage = module.stage;
        n.message_text = "Unhandled token.";
        n.type = NotificationType::Error;
        context.notifications.push_back(n);
    }

    return true;
}


static void get_next_string_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    const string from_now = text_line.substr(next_column);
    assert(from_now.size() > 0);
    assert(from_now[0] == '\'' || from_now[0] == '\"');

    int begin_location = next_column;
    char first_char = from_now[0];
    next_column++;
    string encoded_text {""}; // resolve all \n, \r etc
    while(next_column < int(text_line.length()) && text_line[next_column] != first_char)
    {
        if(text_line[next_column] != escape_character)
        {
            encoded_text += text_line[next_column];
        }
        else // if character is escaped
        {
            next_column++;
            if(next_column >= int(text_line.length()))
            {
                Location current_location = location;
                current_location.column = next_column - 1;
                module.fatal_error_count++;
                Notification n;
                n.location = current_location;
                n.is_fatal = true;
                n.stage = module.stage;
                n.message_text = "The line cannot end with the escape character.";
                n.type = NotificationType::Error;
                context.notifications.push_back(n);
            }
            else
            {
                bool escape_item_found = false;
                for(const std::pair<char, char> & escaped_item : escape_list)
                {
                    if(escaped_item.first == text_line[next_column])
                    {
                        escape_item_found = true;
                        encoded_text += escaped_item.second;
                        break;
                    }
                }
                if(!escape_item_found)
                {
                    Location current_location = location;
                    current_location.column = next_column;
                    module.fatal_error_count++;
                    Notification n;
                    n.location = current_location;
                    n.is_fatal = true;
                    n.stage = module.stage;
                    n.message_text = "Unrecognized escaped character";
                    n.type = NotificationType::Error;
                    context.notifications.push_back(n);
                }
            }
        }
        next_column++;
    }
    // if no string close found
    if(next_column >= int(text_line.length()))
    {
        Location current_location = location;
        current_location.column = next_column - 1;
        module.fatal_error_count++;
        Notification n;
        n.location = location;
        n.is_fatal = true;
        n.stage = module.stage;
        n.auxiliary_locations.push_back(current_location);
        n.message_text = "String token is opened but not closed up until end of the line.";
        n.type = NotificationType::Error;
        context.notifications.push_back(n);
    }
    next_column++;

    int last_column = std::min(next_column, int(text_line.length()));
    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::literal;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::literal_string;
    token.location = location;
    token.text = text_line.substr(begin_location, last_column - begin_location);
    token.encoded_text = encoded_text;
    new_token = token;
}


static void get_next_hexnumber_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    const string from_now = text_line.substr(next_column);
    assert(from_now.size() > 0);
    assert(starts_with(from_now, "0x"));
    string hexnumber = from_now.substr(0, 2);
    next_column += 2;
    while(next_column < int(text_line.length()))
    {
        char current_char = text_line[next_column];
        next_column++;
        bool range_09 = is_num09(current_char);
        bool range_af_small = (current_char >= 'a' && current_char <= 'f');
        bool range_AF_capital = (current_char >= 'A' && current_char <= 'F');
        if(range_09 || range_af_small || range_AF_capital)
            hexnumber += current_char;
        else
            break;
    }

    if(hexnumber.length() < 3)
    {
        module.fatal_error_count++;
        Notification n;
        n.location = location;
        n.is_fatal = true;
        n.stage = module.stage;
        n.message_text = "Incomplete hex number.";
        n.type = NotificationType::Error;
        context.notifications.push_back(n);
    }

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::literal;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::literal_number;
    token.location = location;
    token.text = hexnumber;
    token.encoded_text = "";
    new_token = token;
}


static void get_next_decimalnumber_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    assert(next_column < int(text_line.length()));
    assert(is_num09(text_line[next_column]));
    string number {""};
    int decimal_point_count = 0;
    while(next_column < int(text_line.length()))
    {
        char current_char = text_line[next_column];
        next_column++;
        bool range_09 = (current_char >= '0' && current_char <= '9');

        if(current_char == '.')
            decimal_point_count++;

        if(range_09 || (current_char == '.' && decimal_point_count <= 1))
            number += current_char;
        else
            break;
    }

    assert(number.length() > 0);

    if(decimal_point_count > 1)
    {
        module.fatal_error_count++;
        Notification n;
        n.location = location;
        n.is_fatal = true;
        n.stage = module.stage;
        n.message_text = "Multiple floating point sign detected.";
        n.type = NotificationType::Error;
        context.notifications.push_back(n);
    }

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::literal;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::literal_number;
    token.location = location;
    token.text = number;
    token.encoded_text = "";
    new_token = token;
}


static void get_next_id_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    (void) context;
    (void) module;

    assert(next_column < int(text_line.length()));
    assert(is_id(text_line[next_column]));
    string id_text {""};
    while(next_column < int(text_line.length()))
    {
        char current_char = text_line[next_column];
        if(is_id(current_char) || (id_text.length() > 0 && is_num09(current_char)))
            id_text += current_char;
        else
            break;
        next_column++;
    }
    assert(id_text.length() > 0);

    Token token; // make sure new_token is set from scratch
    get_id_token_type_category(id_text, token.type, token.category);
    token.id = id_counter++; /* token id is unique per module */
    token.location = location;
    token.text = id_text;
    token.encoded_text = "";
    new_token = token;
}


static void get_next_space_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    (void) context;
    (void) module;

    assert(next_column < int(text_line.length()));
    assert(text_line[next_column] == ' ');
    int first_column = next_column;

    while(next_column < int(text_line.length()))
    {
        char current_char = text_line[next_column];
        if(current_char != ' ')
            break;
        next_column++;
    }
    assert(next_column > first_column);

    int last_column = std::min(next_column, int(text_line.length()));

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::space;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::spaces;
    token.location = location;
    token.text = text_line.substr(first_column, last_column - first_column);
    token.encoded_text = "";
    new_token = token;
}


static void get_next_line_comment_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    (void) context;
    (void) module;

    assert(next_column < int(text_line.length()));
    const string from_now = text_line.substr(next_column);
    assert(starts_with(from_now, "//"));
    int first_column = next_column;

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::comment;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::comment;
    token.location = location;
    token.text = text_line.substr(first_column, int(text_line.length()) - first_column);
    token.encoded_text = "";
    new_token = token;

    next_column = int(text_line.length());
}


static void get_next_segment_comment_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    (void) context;
    (void) module;

    assert(next_column < int(text_line.length()));
    const string from_now = text_line.substr(next_column);
    assert(starts_with(from_now, "/*"));

    string comment_txt {"/*"};
    next_column += 2;

    char last_char = '*';
    while(next_column < int(text_line.length()))
    {
        char current_char = text_line[next_column];
        next_column++;
        if(last_char == '*' && current_char == '/')
            break;
        comment_txt += current_char;
        last_char = current_char;
    }

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::comment;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::comment;
    token.location = location;
    token.text = comment_txt;
    token.encoded_text = "";
    new_token = token;
}


static void get_next_symbol_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    assert(next_column < int(text_line.length()));
    assert(is_symbol(text_line[next_column]));

    string symbol_text {""};
    while(next_column < int(text_line.length()) && is_symbol(text_line[next_column]))
    {
        symbol_text += text_line[next_column];
        next_column++;
    }

    assert(symbol_text.length() > 0);

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::symbol;
    token.id = id_counter++; /* token id is unique per module */
    token.type = get_symbol_token_type(symbol_text, context, module, location);
    token.location = location;
    token.text = symbol_text;
    token.encoded_text = "";
    new_token = token;
}


static void get_next_continuation_token(
    const string & text_line,
    int & next_column,
    Token & new_token,
    Context & context,
    Module & module,
    const Location & location,
    int & id_counter)
{
    (void) context;
    (void) module;
    (void) location;

    assert(next_column == int(text_line.length()) - 1);
    assert(text_line[next_column] == continuation_token);

    next_column++;

    Token token; // make sure new_token is set from scratch
    token.category = TokenCategory::continuation;
    token.id = id_counter++; /* token id is unique per module */
    token.type = TokenType::continuation;
    token.location = location;
    token.text = continuation_token;
    token.encoded_text = "";
    new_token = token;
}


static TokenType get_symbol_token_type(
    const string & symbol_text,
    Context & context,
    Module & module,
    const Location & location)
{
    for(const TokenInterpretation & x : symbol_list)
        if(x.text == symbol_text)
            return x.type;

    // symbol not found
    module.fatal_error_count++;
    Notification n;
    n.location = location;
    n.is_fatal = true;
    n.stage = module.stage;
    n.message_text = "Unknown symbol \'" + symbol_text + "\' (length: " + std::to_string(symbol_text.length()) + ").";
    n.type = NotificationType::Error;
    context.notifications.push_back(n);

    return TokenType::invalid;
}


static bool is_id(char ch)
{
    if(ch >= 'a' && ch <= 'z')
        return true;
    if(ch >= 'A' && ch <= 'Z')
        return true;
    if(ch == '_')
        return true;
    return false;
}


static bool is_num09(char ch)
{
    if(ch >= '0' && ch <= '9')
        return true;
    return false;
}


static bool is_symbol(char ch)
{
    for(const char k : all_symbols)
        if(k == ch)
            return true;
    return false;
}


static void get_id_token_type_category(const string & token_text, TokenType & token_type, TokenCategory & token_category)
{
    for(const TokenInterpretation & x : keyword_list)
        if(x.text == token_text)
        {
            token_type = x.type;
            token_category = TokenCategory::keyword;
            return ;
        }

    for(const TokenInterpretation & x : builtin_datatype_list)
        if(x.text == token_text)
        {
            token_type = x.type;
            token_category = TokenCategory::builtin_type;
            return ;
        }

    token_type = TokenType::id;
    token_category = TokenCategory::id;
}


int space_count(const string & token)
{
    int count = 0;
    for(char c : token)
    {
        if(c == ' ')
            count++;
        else
            return -1;
    }
    return count;
}


bool get_builtin_module(const string & token, BuiltinModule & builtin_module)
{
    for(const std::pair<string, BuiltinModule> & builtin_module_item : builtin_module_list)
    {
        if(builtin_module_item.first == token)
        {
            builtin_module = builtin_module_item.second;
            return true;
        }
    }
    return false;
}

static string translate_location_path(const Context & context, const Location & location)
{
    std::stringstream ss;
    string filepath;
    if(location.file_index >= 0)
        filepath = context.modules[location.file_index].filepath;
    ss << filepath;
    if(location.line >= 0)
        ss << ":" << (location.line + 1); // the location is zero-based
    if(location.column >= 0)
        ss << ":" << (location.column + 1); // the location is zero-based
    return ss.str();
}


string translate_notification(const Context & context, const Notification & notification)
{
    std::stringstream ss;
    ss << notification.message_text << endl;
    ss << translate_location_path(context, notification.location) << endl;
    if(notification.auxiliary_locations.size() > 0)
        ss << "See also:" << endl;
    for(const Location & aux_location : notification.auxiliary_locations)
        ss << translate_location_path(context, aux_location) << endl;
    return ss.str();
}


bool is_main_function_name(const string & func_name)
{
    return func_name == "main";
}


string artifacts_folder_name()
{
    return "artifacts";
}


string object_file_extention()
{
    return ".o";
}


string ir_file_extention()
{
    return ".ir";    
}


string get_sys_decl()
{
    return "sys_decl";
}


string get_pointer_sign()
{
    return "*";
}

NS_LANGUAGE_END
