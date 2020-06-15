#include <00_common/protocol.hpp>
#include <00_common/serializer.hpp>
#include <00_common/utils.hpp>
#include <libs/filesystem.hpp>
#include <light_header>
#include <tools/tools-common/html/html.hpp>
#include <iomanip>


int get_digits(int x);
string render_AST(const Module & module);
string render_content(const string & inner);
string render_func_details(const FunctionAST & func, const Module & module);
string render_func_details_attribute(const Attribute & attribute);
string render_func_details_input_param(const FuncParamDeclAST & input_param);
string render_func_details_output_type(const TypeDeclAST & output_type);
string render_function_list(const vector<FunctionAST> & functions, const Module & module, const string & title);
string render_highlighted_code(const Module & module);
string render_sidebar(const string & inner);
string render_statement(const StatementAST & statement, const Module & module);
string render_statement_expression(const ExpressionAST & expression, const Module & module);
string render_statement_expression_functioncall(const ExpressionAST & expression, const Module & module);
string render_statement_expression_id(const ExpressionAST & expression, const Module & module);
string render_statement_expression_number(const ExpressionAST & expression, const Module & module);
string render_statement_expression_string(const ExpressionAST & expression, const Module & module);
string to_string(const ExpressionAST & expression, const Module & module);


string empty_node()
{
    return "<span class=\"empty\">(empty)</span>";
}


void render_module_side(std::ostringstream & stream, const Module & module)
{
    stream << "<li><a href=\"#\">" + html::escape(module.filename) + "</a></li>";
    stream << html::br();
}


string token_html_class(TokenCategory token_cat)
{
    switch(token_cat)
    {
        case TokenCategory::not_processed:
            return "token_unknown";
        case TokenCategory::invalid:
            return "token_invalid";
        case TokenCategory::keyword:
            return "token_keyword";
        case TokenCategory::symbol:
            return "token_symbol";
        case TokenCategory::literal:
            return "token_literal";
        case TokenCategory::builtin_type:
            return "token_builtin";
        case TokenCategory::id:
            return "token_id";
        case TokenCategory::space:
            return "token_space";
        case TokenCategory::comment:
            return "token_comment";
        default:
            throw runtime_error("Unhandled token caterogy (" + std::to_string((int) token_cat) + ") . A2348523329483");
    }
}


void add_token(std::ostringstream & stream, const Module & module, int token_index)
{
    const Token & token = module.token_list[token_index];

    stream << "<span class=\"" << token_html_class(token.category) << "\">";
    stream << html::preserve_space(html::escape(token.text));
    stream << "</span>";
}

// get a number and count the number of its digits
int get_digits(int x)
{
    int count = 0;
    while(x > 0)
    {
        count++;
        x /= 10;
    }
    return count;
}

string render_highlighted_code(const Module & module)
{
    std::ostringstream stream;
    int line_count = 0;
    int digits = get_digits((int) module.token_lines.size());
    for(const TokenLine & token_line : module.token_lines)
    {
        stream << std::setfill('0') << std::setw(digits) << ++line_count << ": ";
        for(int token_index : token_line.token_indices)
        {
            add_token(stream, module, token_index);
        }
        stream << "<br>" << endl;
    }
    return stream.str();
}


string render_func_details_attribute(const Attribute & attribute)
{
    std::ostringstream stream;
    stream << attribute.attr_name;
    return stream.str();
}


string render_func_details_input_param(const FuncParamDeclAST & input_param)
{
    std::ostringstream stream;
    stream << input_param.var_name;
    return stream.str();
}


string render_func_details_output_type(const TypeDeclAST & output_type)
{
    std::ostringstream stream;
    stream << "ast_index:" << output_type.ast_index;
    return stream.str();
}


string to_string(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    switch(expression.type)
    {
        case ExpressionType::undefined:
            throw runtime_error("Not implemented. A0987503248750");
            break;
        case ExpressionType::id:
            stream << get_token(module, expression.id_token_index).text;
            break;
        case ExpressionType::Number:
            stream << get_token(module, expression.literal_token_index).text;
            break;
        case ExpressionType::String:
            stream << get_token(module, expression.literal_token_index).encoded_text;
            break;
        case ExpressionType::Assignment:
            throw runtime_error("Not implemented. A4352874058038");
            break;
        case ExpressionType::Addition:
            throw runtime_error("Not implemented. A6548760870870");
            break;
        case ExpressionType::FunctionCall:
            {
                stream << expression.func_name;
                stream << "(";
                string sep = "";
                for(const ExpressionAST & param_exp : expression.func_params)
                {
                    stream << sep;
                    stream << to_string(param_exp, module);
                    sep = ", ";
                }
                stream << ")";
            }
            break;
        case ExpressionType::ParenthesisWrap:
            throw runtime_error("Not implemented. A5487658768708");
            break;
        default:
            throw runtime_error("Unhandled expression type (" + std::to_string((int) expression.type) + "). A7685753387646");
    }
    return stream.str(); 
}


string render_statement_expression_id(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-expresseion-id\">" << endl;
    stream << "id: " << get_token(module, expression.id_token_index).text;
    stream << "</div>" << endl;
    return stream.str();   
}


string render_statement_expression_number(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-expresseion-literal\">" << endl;
    stream << "number literal: " << get_token(module, expression.literal_token_index).text;
    stream << "</div>" << endl;
    return stream.str();   
}


string render_statement_expression_string(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-expresseion-literal\">" << endl;
    stream << "string literal: " << get_token(module, expression.literal_token_index).text;
    stream << "</div>" << endl;
    return stream.str();   
}


string render_statement_expression_functioncall(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-expresseion-call-func-name\">" << endl;
    stream << "function call <br>" << endl;
    stream << "function name: " << expression.func_name << endl;
    stream << "</div>" << endl;

    stream << "<div class=\"ast-expresseion-call-func-param collapsing-header\">" << endl;
    stream << "function params" << endl;
    stream << "</div>" << endl;
    stream << "<div class=\"ast-contained\">" << endl;
    for(const ExpressionAST & param_exp : expression.func_params)
    {
        stream << render_statement_expression(param_exp, module);
    }
    stream << "</div>" << endl;
    return stream.str();   
}


string render_statement_expression(const ExpressionAST & expression, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-expresseion collapsing-header\">" << endl;
    stream << to_string(expression, module) << endl;
    stream << "</div>" << endl;
    stream << "<div class=\"ast-contained\" style=\"display: none;\">" << endl;
    switch(expression.type)
    {
        case ExpressionType::undefined:
            throw runtime_error("Not implemented. A2348539845345");
            break;
        case ExpressionType::id:
            stream << render_statement_expression_id(expression, module);
            break;
        case ExpressionType::Number:
            stream << render_statement_expression_number(expression, module);
            break;
        case ExpressionType::String:
            stream <<  render_statement_expression_string(expression, module);
            break;
        case ExpressionType::Assignment:
            throw runtime_error("Not implemented. A3485243985394");
            break;
        case ExpressionType::Addition:
            throw runtime_error("Not implemented. A4852347503487");
            break;
        case ExpressionType::FunctionCall:
            stream <<  render_statement_expression_functioncall(expression, module);
            break;
        case ExpressionType::ParenthesisWrap:
            throw runtime_error("Not implemented. A3285324503287");
            break;
        default:
            throw runtime_error("Unhandled expression type (" + std::to_string((int) expression.type) + "). A7685753387646");
    }
    stream << "</div>" << endl;
   
    return stream.str();   
}


string render_statement(const StatementAST & statement, const Module & module)
{
    std::ostringstream stream;
    stream << "<div class=\"ast-statement\">" << endl;
    switch(statement.type)
    {
        case StatementType::Invalid:
            throw runtime_error("Not implemented. A349852345333");
            break;
        case StatementType::Expression:
            stream << render_statement_expression(*((ExpressionAST *) statement.data), module);
            break;
        case StatementType::Jump:
            throw runtime_error("Not implemented. A435872037034");
            break;
        case StatementType::DeclareVar:
            throw runtime_error("Not implemented. A430492870475");
            break;
        case StatementType::DeclareType:
            throw runtime_error("Not implemented. A283473630274");
            break;
        case StatementType::ControlBlock:
            throw runtime_error("Not implemented. A329485239509");
            break;
        default:
            throw runtime_error("Unhandled statement type (" + std::to_string((int) statement.type) + "). A92837746934");
    }
    stream << "</div>" << endl;
    return stream.str();   
}


string render_func_details(const FunctionAST & func, const Module & module)
{
    std::ostringstream stream;
    // attributes
    stream << "<div class=\"collapsing-header\">Attributes</div>" << endl;
    stream << "<div class=\"ast-contained ast-function-attribute-group\">" << endl;
    for(const Attribute & attribute : func.attributes)
    {
        stream << "<div class=\"ast-function-attribute\">" << endl;
        stream << render_func_details_attribute(attribute);
        stream << "</div>" << endl;
    }
    if(func.attributes.size() == 0)
        stream << empty_node();
    stream << "</div>" << endl;
    // input params
    stream << "<div class=\"collapsing-header\">Input params</div>" << endl;
    stream << "<div class=\"ast-contained ast-function-input-param-group\">" << endl;
    for(const FuncParamDeclAST & input_param : func.input_params)
    {
        stream << "<div class=\"ast-function-input-param\">" << endl;
        stream << render_func_details_input_param(input_param);
        stream << "</div>" << endl;
    }
    if(func.input_params.size() == 0)
        stream << empty_node();
    stream << "</div>" << endl;
    // output params
    stream << "<div class=\"collapsing-header\">Output params</div>" << endl;
    stream << "<div class=\"ast-contained ast-function-output-type-group\">" << endl;
    for(const TypeDeclAST & output_type : func.output_types)
    {
        stream << "<div class=\"ast-function-output-type\">" << endl;
        stream << render_func_details_output_type(output_type);
        stream << "</div>" << endl;
    }
    if(func.output_types.size() == 0)
        stream << empty_node();
    stream << "</div>" << endl;
    // body
    stream << "<div class=\"collapsing-header\">Body</div>" << endl;
    stream << "<div class=\"ast-contained ast-function-statement-group\">" << endl;
    for(const StatementAST & statement : func.compound_stmt.statements)
    {
        stream << "<div class=\"ast-function-statement\">" << endl;
        stream << render_statement(statement, module);
        stream << "</div>" << endl;
    }
    if(func.compound_stmt.statements.size() == 0)
        stream << empty_node();
    stream << "</div>" << endl;
    return stream.str();
}


string render_function_list(const vector<FunctionAST> & functions, const Module & module, const string & title)
{
    std::ostringstream stream;
    stream << "<div class=\"collapsing-header\">" << title << "</div>" << endl;
    stream << "<div class=\"ast-contained ast-function-group\">" << endl; // collpsable box
    for(const FunctionAST & func : functions)
    {
        stream << "<div class=\"ast-function collapsing-header\">" << func.func_name << "</div>" << endl;

        stream << "<div class=\"ast-contained ast-function-details\" style=\"display: none;\">" << endl;
        stream << render_func_details(func, module);
        stream << "</div>" << endl;
    }
    if(functions.size() == 0)
        stream << empty_node();
    stream << "</div>" << endl; // function declarations
    return stream.str();
}


string render_AST(const Module & module)
{
    std::ostringstream stream;
    // Function declarations
    stream << render_function_list(module.ast.decl_only_functions, module, "Function declarations");
    // Function implementations
    stream << render_function_list(module.ast.functions, module, "Function implementations");
 
    return stream.str();
}


void render_module_content(std::ostringstream & stream, const Module & module)
{
    stream << html::h2(html::escape(module.filename));
    stream << html::br();

    stream << "<div class=\"collapsing-header\"><span>Source code</span></div>";
    stream << "<div class=\"ast-contained source-code\">" << endl;
    stream << render_highlighted_code(module);
    stream << "</div>" << endl;

    stream << "<div class=\"collapsing-header\"><span>AST content</span></div>";
    stream << "<div class=\"ast-contained ast-content\">" << endl;
    stream << render_AST(module);
    stream << "</div>" << endl;
}


string render_sidebar(const string & inner)
{
    std::ostringstream stream;
    stream << "<nav class=\"modules-menu\">" << endl;
    stream << "<h3>Module</h3>" << endl;
    stream << "<ul>" << endl;
    stream << inner << endl;
    stream << "</ul>" << endl;
    stream << "</nav>" << endl;
    return stream.str();
}


string render_content(const string & inner)
{
    std::ostringstream stream;
    stream << "<div id=\"content\">" << endl;
    stream << "<article>" << endl;
    stream << inner << endl;
    stream << "</article>" << endl;
    stream << "</div>" << endl;
    return stream.str();
}


int main(int argc, char** argv)
{
    if(argc != 2)
        throw runtime_error("A single argument is expected");
    string context_filename = argv[1];
    Context context = context_from_string(filesystem::read_file(context_filename));
    std::ostringstream body_stream;
    std::ostringstream inner_sidebar_stream;
    std::ostringstream inner_content_stream;

    for(const Module & module : context.modules)
    {
        render_module_side(inner_sidebar_stream, module);
        render_module_content(inner_content_stream, module);
    }

    body_stream << "<header><h2>AST viewer</h2></header>";
    body_stream << "<section class=\"page-content\">" << endl;

    body_stream << "<div class=\"nav-pane\">" << endl;
    body_stream << render_sidebar(inner_sidebar_stream.str());
    body_stream << "</div>" << endl;

    body_stream << "<div class=\"content-pane\">" << endl;
    body_stream << render_content(inner_content_stream.str());
    body_stream << "</div>" << endl;

    body_stream << "</section>" << endl;

    fs::path html_filepath =  fs::path(context_filename).replace_extension("html");
    string html_content = html::wrap_body(body_stream.str(), "AST view");
    filesystem::write_file(html_filepath, html_content);
    return 0;
}
