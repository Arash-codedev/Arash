#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace easytext
{
    extern const std::string newline;

    std::string replace_once(std::string text,
                        std::string to_find,
                        std::string to_replace);

    std::string replace_all(std::string text,
                        std::string to_find,
                        std::string to_replace);

    std::vector<std::string> explode(std::string const & text, std::string const delim);

    // explode skipping parenthesis() and braces[]{}
    // notice that skip_ranges might have duplicat ranges and function should not fail
    std::vector<std::string> explode_shallow(
        std::string const & text,
        char const delim,
        std::vector<std::pair<char,char> > skip_ranges = {{'(', ')'},{'[', ']'},{'{', '}'}}
        );

    std::string format_time(time_t time, std::string format);

    // search for a string in a vector
    bool is_string_in(std::string text, std::vector<std::string> vec);

    std::string get_indentation(std::string row);
    std::string get_after_indentation(std::string row);

    bool starts_with(std::string text, std::string item);

    std::string strip_comments(std::string const& text_raw);

    template <typename T>
    std::string to_string(T val)
    {
        std::stringstream stream;
        stream << val;
        return stream.str();
    }

}