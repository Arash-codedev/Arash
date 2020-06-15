#include <light_header>
#include <00_common/protocol.hpp>
#include <languages/language-interface.hpp>
#include <00_common/utils.hpp>



static void check_illegal_characters(Context &context, int module_index);
static void evaluate_indentations(Context &context, int module_index);
static void separate_text_line(const string & text_line, TokenLine & token_line, int line_number, int module_index, vector<Token> & token_list, Context & context);



// separate tokens of a single line of text
static void separate_text_line(
    const string & text_line, /* input single line of text */
    TokenLine & token_line, /* output separated token ids */
    int line_number,
    int module_index,
    vector<Token> & token_list, /* output separated tokens */
    Context & context)
{
    int next_column = 0; /* the beginning of the next token */
    Token new_token;
    Module & module = context.modules[module_index];
    int & id_counter = module.id_counter;

    Location location;
    location.file_index = module_index;
    location.line = line_number;
    // ask the language to tell you the offset of the next token
    // as well as separating the token and analyzing it
    while(language::get_next_token(text_line, next_column, new_token, context, module, location, id_counter))
    {
        location.column = next_column;
        token_list.push_back(new_token);
        token_line.token_indices.push_back(int(token_list.size())-1);
    }
}


static void evaluate_indentations(Context &context, int module_index)
{
    Module & module = context.modules[module_index];
    for(TokenLine & it_kline : module.token_lines)
    {
        it_kline.indentation = 0;
        if(it_kline.token_indices.size() > 0)
        {
            const string & token_txt = get_token(module, it_kline.token_indices[0]).text;
            int spaces = language::space_count(token_txt);
            if(spaces < 0)
            {// invalid space
                continue;
            }
            if(spaces % 4 == 0)
            {// valid 4 space indentation
                it_kline.indentation = spaces / 4;
            }
            else
            {
                // check if there is any existing effective token
                for(int i = 1; i < (int)it_kline.token_indices.size(); i++)
                {
                    const Token & token = get_token(module, it_kline.token_indices[i]);
                    if(is_effective(token))
                    {
                        add_fatal_error(context, module, token, {}, "Invalid indentation level");
                    }
                }
            }
        }
    }
}


// separate tokens of each module
void tokenizer_process_module(Context &context, int module_index)
{
    Module & module = context.modules[module_index];
    if(module.stage == Stage::Streamer)
        module.stage = Stage::Tokenizer;
    else
        throw runtime_error("Wrong stage. A832173086");

    for(int i = 0; i < (int) module.text_lines.size(); i ++ )
    {
        TokenLine token_line;
        separate_text_line(
                module.text_lines[i] /* text_line */, 
                token_line /* token_line */,
                i /* line_number */,
                module.index /* module_index */,
                module.token_list /* token_list */,
                context);
        module.token_lines.push_back(token_line);
    }

    evaluate_indentations(context, module_index);
    check_illegal_characters(context, module_index);
}


// make sure the language plugin does not bypass illegal characters
static void check_illegal_characters(Context &context, int module_index)
{
    Module & module = context.modules[module_index];
    for(const TokenLine & token_line : module.token_lines)
    {
        for(int token_index : token_line.token_indices)
        {
            const Token & token = get_token(module, token_index);
            for(char c : token.text)
                if(c == '\t')
                    add_ordinary_error(context, module, token, {}, "Illegal character detected.");
        }
    }
}


// separate tokens of each module
void tokenizer_process(Context &context)
{
    for(int i = 0; i < (int) context.modules.size(); i ++)
    {
        Module & module = context.modules[i];
        tokenizer_process_module(context, i);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }
}
