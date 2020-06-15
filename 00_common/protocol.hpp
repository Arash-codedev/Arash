#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <iomanip>
#include <sstream>
#include <light_header>
#include <00_common/ast.hpp>



enum class Stage
{
    Unprocessed,
    Streamer,
    Tokenizer,
    BlockBuilder,
    AbstractorInit,
    Pertainer,
    Injector,
    Resolver,
    Optimizer,
    Reformer,
    Codegen,
    Finalized
};



class Location
{
public:
    int file_index=-1;
    int line=-1;
    int column=-1;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
           & BOOST_SERIALIZATION_NVP(file_index)
           & BOOST_SERIALIZATION_NVP(line)
           & BOOST_SERIALIZATION_NVP(column)
           ;
    }
};



enum class NotificationType
{
    Uninitialized,
    Error,
    Warning,
};



class Notification
{
public:
    Location location;
    vector<Location> auxiliary_locations;
    string message_text;
    bool is_fatal; /* prevent to proceed */
    Stage stage;
    NotificationType type = NotificationType::Uninitialized;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
           & BOOST_SERIALIZATION_NVP(location)
           & BOOST_SERIALIZATION_NVP(auxiliary_locations)
           & BOOST_SERIALIZATION_NVP(message_text)
           & BOOST_SERIALIZATION_NVP(is_fatal)
           & BOOST_SERIALIZATION_NVP(stage)
           & BOOST_SERIALIZATION_NVP(type)
           ;
    }
};



enum class TokenCategory
{
    not_processed,    // not processed yet
    invalid,
    keyword,
    symbol, /* like operators */
    literal,
    builtin_type,
    continuation, /* \ */
    id,
    space,
    comment,
};



enum class TokenType
{
    unknown,
    invalid,
    id,
    spaces,
    comment,
    literal_number,
    literal_string,
    continuation, /* \ */

    // keywords

    /* body */      kw_body,
    /* break */     kw_break,
    /* case */      kw_case,
    /* class */     kw_class,
    /* const */     kw_const,
    /* copy */      kw_copy,
    /* ref */       kw_ref,
    /* cref */      kw_cref,
    /* continue */  kw_continue,
    /* fn */        kw_func,
    /* for */       kw_for,
    /* if */        kw_if,
    /* import */    kw_import,
    /* return */    kw_return,
    /* switch */    kw_switch,
    /* type */      kw_type,
    /* var */       kw_var,
    /* while */     kw_while,

    // variable types

    /* Char */      vt_Char,
    /* Bool */      vt_Bool,
    /* Int */       vt_Int,
    /* Int16 */     vt_Int16,
    /* Int32 */     vt_Int32,
    /* Int64 */     vt_Int64,
    /* Int8 */      vt_Int8,
    /* String */    vt_String,
    /* UInt */      vt_UInt,
    /* UInt16 */    vt_UInt16,
    /* UInt32 */    vt_UInt32,
    /* UInt64 */    vt_UInt64,
    /* UInt8 */     vt_UInt8,

    // operators

    /* ! */         sym_not,
    /* # */         sym_hash_tag,
    /* % */         sym_percent,
    /* %= */        sym_perenct_equal,
    /* & */         sym_ampersand,
    /* && */        sym_ampersand_ampersand,
    /* &= */        sym_ampersand_equal,
    /* ( */         sym_parenthesis_open,
    /* ) */         sym_parenthesis_close,
    /* * */         sym_asterisk,
    /* *= */        sym_asterisk_equal,
    /* + */         sym_plus,
    /* ++ */        sym_plus_plus,
    /* += */        sym_plus_equal,
    /* , */         sym_comma,
    /* - */         sym_minus,
    /* -- */        sym_minus_minus,
    /* -= */        sym_minus_equal,
    /* -> */        sym_arrow,
    /* . */         sym_dot,
    /* ... */       sym_3dots,
    /* / */         sym_slash,
    /* /= */        sym_slash_equal,
    /* : */         sym_colon,
    /* := */        sym_colon_equal,
    /* ; */         sym_semicolon,
    /* < */         sym_less,
    /* << */        sym_shift_left,
    /* <<= */       sym_shift_left_equal,
    /* <= */        sym_less_equal,
    /* <=> */       sym_spaceship,
    /* = */         sym_equal,
    /* == */        sym_equal_equal,
    /* > */         sym_greater,
    /* >= */        sym_greater_equal,
    /* >> */        sym_shift_right,
    /* >>= */       sym_shift_right_equal,
    /* @ */         sym_at,
    /* [ */         sym_squared_bracket_open,
    /* ] */         sym_squared_bracket_close,
    /* { */         sym_curly_brace_open,
    /* } */         sym_curly_brace_close,
    /* ^ */         sym_xor,
    /* ^= */        sym_xor_equal,
    /* | */         sym_pipe,
    /* |= */        sym_pipe_equal,
    /* || */        sym_pipe_pipe,
    /* ~ */         sym_bit_not,
};


enum class BuiltinModule
{
    invalid,
    std,
    fs,
    os,
};



class Token
{
public:
    TokenCategory category = TokenCategory::not_processed;
    int id; /* token id is unique per module */
    TokenType type = TokenType::unknown;
    Location location;
    string text;
    string encoded_text;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
           & BOOST_SERIALIZATION_NVP(category)
           & BOOST_SERIALIZATION_NVP(id)
           & BOOST_SERIALIZATION_NVP(type)
           & BOOST_SERIALIZATION_NVP(location)
           & BOOST_SERIALIZATION_NVP(text)
           & BOOST_SERIALIZATION_NVP(encoded_text)
           ;
    }
};



class TokenLine
{
public:
    vector<int> token_indices;
    int indentation = -1;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
           & BOOST_SERIALIZATION_NVP(token_indices)
           & BOOST_SERIALIZATION_NVP(indentation)
           ;
    }
};



class RawBlockTree
{
public:
    // vector<BlockDecorator> decorators;
    // vector<BlockProperty> properties;
    TokenLine line;
    vector<RawBlockTree> children;
    vector<vector<int>> block_groups;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
           // & BOOST_SERIALIZATION_NVP(decorators)
           // & BOOST_SERIALIZATION_NVP(properties)
           & BOOST_SERIALIZATION_NVP(line)
           & BOOST_SERIALIZATION_NVP(children)
           & BOOST_SERIALIZATION_NVP(block_groups)
           ;
    }
};



class SyntaxAbstractTree
{
public:
    SyntaxAbstractTree() {}
};



class Module
{
public:
    string filename;
    string filepath;
    int index = -1;
    Stage stage = Stage::Unprocessed;
    int ordinary_error_count = 0;
    int fatal_error_count = 0;
    int warning_count = 0;
    int id_counter = 0;

/************************************************
            Stage: Streamer
************************************************/
    string content;
    vector<string> text_lines;

/************************************************
            Stage: Separator and tokenizerer
************************************************/
    vector<Token> token_list;
    vector<TokenLine> token_lines;

/************************************************
            Stage: Blocker
************************************************/
    RawBlockTree root_raw_block;

/************************************************
            Stage: Abstractor
************************************************/

    ModuleAST ast;

/************************************************
            Stage: Pertainer
************************************************/

    set<string> pertained_modules;
    set<BuiltinModule> pertained_builtin_modules;

/************************************************
            Stage: Injector
************************************************/

    // nothing

/************************************************
            Stage: Resolver
************************************************/

    // nothing

// ----------------------------------------------
    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(filename)
            & BOOST_SERIALIZATION_NVP(filepath)
            & BOOST_SERIALIZATION_NVP(index)
            & BOOST_SERIALIZATION_NVP(stage)
            & BOOST_SERIALIZATION_NVP(ordinary_error_count)
            & BOOST_SERIALIZATION_NVP(fatal_error_count)
            & BOOST_SERIALIZATION_NVP(warning_count)
            & BOOST_SERIALIZATION_NVP(id_counter)
            // streamer
            & BOOST_SERIALIZATION_NVP(content)
            & BOOST_SERIALIZATION_NVP(text_lines)
            // separator
            & BOOST_SERIALIZATION_NVP(token_list)
            & BOOST_SERIALIZATION_NVP(token_lines)
            // blocker
            & BOOST_SERIALIZATION_NVP(root_raw_block)
            // abstractor
            & BOOST_SERIALIZATION_NVP(ast)
            // pertainer
            & BOOST_SERIALIZATION_NVP(pertained_modules)
            & BOOST_SERIALIZATION_NVP(pertained_builtin_modules)
            ;
    }

};


class Compiler_Parameters
{
public:
    vector<string> input_files;
    string output_file;
    int arch_integer_bits = 64;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(input_files)
            & BOOST_SERIALIZATION_NVP(output_file)
            & BOOST_SERIALIZATION_NVP(arch_integer_bits)
            ;
    }
};



// Each time a compilation forms, a new context is created
class Context
{
public:
    vector<Module> modules; // This vector should be always appended. No member is allowed to be removed or swaped otherwise the indices will mess up.
    vector<Notification> notifications;
    Compiler_Parameters compiler_params;

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(modules)
            & BOOST_SERIALIZATION_NVP(notifications)
            & BOOST_SERIALIZATION_NVP(compiler_params)
            ;
    }
};
