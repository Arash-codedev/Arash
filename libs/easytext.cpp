#include "easytext.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp> // for using boost::split to explode a string
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class
#include <boost/algorithm/string/trim.hpp>

namespace easytext
{
    const std::string newline("\n");

    std::string replace_once(std::string text,
                        std::string to_find,
                        std::string to_replace)
    {
        size_t found_pos = text.find(to_find);
        if(found_pos == std::string::npos)
            return(text);
        return text.replace(found_pos,to_find.length(),to_replace);
    }

    std::string replace_all(std::string text,
                        std::string to_find,
                        std::string to_replace)
    {
        std::size_t found_pos;
        std::string result=text;
        while((found_pos = result.find(to_find))!=std::string::npos)
        {
            result=result.replace(found_pos,to_find.length(),to_replace);
        }
        return result;
    }

    std::vector<std::string> explode(std::string const & text, std::string const delim)
    {
        std::vector<std::string> results;
        boost::split(results,text,boost::is_any_of(delim),boost::token_compress_off);
        return results;
    }

    // explode skipping parenthesis() and braces[]{}
    // notice that skip_ranges might have duplicat ranges and function should not fail
    std::vector<std::string> explode_shallow(
        std::string const & text,
        char const delim,
        std::vector<std::pair<char,char> > skip_ranges/* = {{'(', ')'},{'[', ']'},{'{', '}'}}*/
        )
    {
        std::vector<std::string> results;
        std::string result;
        std::vector<char> container_heads;
        for(auto skip_range:skip_ranges)
        {
            if(delim==skip_range.first||delim==skip_range.second)
                throw std::runtime_error("Delimiter should not match any range character.");
        }
        for(char c : text)
        {
            if(c==delim && container_heads.empty())
            {
                results.push_back(result);
                result="";
            }
            else
            {
                result+=c;
                
                // look for range head
                bool found_head=false;
                for(auto skip_range:skip_ranges)
                    if(c==skip_range.first)
                        found_head=true;
                if(found_head)
                    container_heads.push_back(c);

                if(container_heads.size()>0)
                {
                    // look for range tail
                    bool found_tail=false;
                    for(auto skip_range:skip_ranges)
                        if(c==skip_range.second && container_heads.back()==skip_range.first)
                            found_tail=true;
                    if(found_tail)
                        container_heads.pop_back();
                }
            }
        }
        results.push_back(result);
        return results;
    }

    std::string format_time(time_t time, std::string format)
    {
        tm *ltm = localtime(&time);
        boost::format formatted_time(format);
        formatted_time % (1900+ltm->tm_year) % (1+ltm->tm_mon) % (ltm->tm_mday);
        formatted_time % ltm->tm_hour % ltm->tm_min % ltm->tm_sec;
        return formatted_time.str();
    }

    // search for a string in a vector
    bool is_string_in(std::string text, std::vector<std::string> vec)
    {
        return (std::find(vec.begin(), vec.end(), text) != vec.end());
    }

    std::string get_indentation(std::string row)
    {
        const std::string delims(" \t");
        std::size_t pos = row.find_first_not_of(delims);
        return row.substr(0,pos);
    }

    std::string get_after_indentation(std::string row)
    {
        const std::string delims(" \t");
        std::size_t pos = row.find_first_not_of(delims);
        return row.substr(pos);
    }

    bool starts_with(std::string text, std::string item)
    {
        return boost::starts_with(text, item);
    }

    std::string strip_comments(std::string const& text_raw)
    {
        std::string result_text;
        typedef boost::wave::cpplexer::lex_token<> token_type;
        typedef boost::wave::cpplexer::lex_iterator<token_type> lexer_type;
        typedef token_type::position_type position_type;
        position_type pos;
        std::string input_c=boost::trim_copy(text_raw)+"\n";

        lexer_type it = lexer_type(input_c.begin(), input_c.end(), pos, 
            boost::wave::language_support(
            boost::wave::support_cpp|boost::wave::support_option_long_long));
        lexer_type end = lexer_type();

        for (;it != end; ++it)
        {
            switch(*it)
            {
                case boost::wave::T_CCOMMENT :
                    // ignore
                    break;
                case boost::wave::T_CPPCOMMENT :
                    // ignore
                    result_text = boost::trim_copy(result_text)+"\n";
                    break;
                default:
                    std::string new_part(it->get_value().begin(), it->get_value().end());
                    result_text += new_part;
            }
        }
        return boost::trim_copy(result_text)+"\n";
    }

}
