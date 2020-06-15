#pragma once
#include <light_header>
#include <00_common/protocol.hpp>
#include <languages/language-interface.hpp>


enum class ContainerOperator
{
    Parentheses,
    SquaredBraket,
    CurlyBraces,
};

class ContainerSpot // containter spot
{
public:
    ContainerOperator co_operator;
    // Token token;
    int token_index;
    ContainerSpot(const ContainerOperator & co_operator, int token_index)
        : co_operator(co_operator), token_index(token_index) {}
};

class ContainerRecognition
{
public:
    ContainerOperator container_operator;
    TokenType container_begin;
    TokenType container_end;
    ContainerRecognition(
        ContainerOperator container_operator,
        TokenType container_begin,
        TokenType container_end
    ) :
        container_operator(container_operator),
        container_begin(container_begin),
        container_end(container_end)
    {
    }
};


void verify_container_set(const vector<ContainerRecognition> & crset);
vector<vector<int>> token_comma_separate(Context & context, Module & module, const vector<int> & token_indices);
vector<vector<int>> token_container_groupify(Context & context, Module & module, const vector<int> & token_indices);
BuiltinTypes TokenType_to_BuiltinType(TokenType token_type);
bool is_decl_qualifier(const Token & token);

extern const vector<ContainerRecognition> container_recognition_set;
