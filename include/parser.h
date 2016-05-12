#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "nodes.h"

//fatal parse error
struct ParseError
{
    ParseError(Token tok, string msg) : tok(tok), msg(msg) { }
    Token tok;
    string msg;
};

struct IndentError
{
    IndentError(Token tok) : tok(tok) { }
    Token tok;
    string msg;
};

//backtrack exception
struct backtrack
{
    backtrack(string msg) : msg(msg) { }
    backtrack() { }
    string msg;
};


class Parser
{
public:
    Parser(Lexer& lexer) : lexer(lexer) { }
    void parse(NBlock& node);

//private:
    void parse_top_level(NBlock& block);
    void parse_method(shared_ptr<NMethod>& block);

    void parse_block_item(NBlock& block);

    void parse_declaration(NBlock& block);
    void parse_variable_declaration(NBlock& block);
    void parse_atomic_declaration(NBlock& block);

    void parse_argument_list(shared_ptr<NArgumentList>& l);
    void parse_parameter_declaration(shared_ptr<NParameterDeclaration>& p);

    void parse_expression_list(shared_ptr<NExpressionList>& l);

    void parse_expression(shared_ptr<NExpression>& expr);
    void parse_assignment_expression(shared_ptr<NExpression>& expr);
    void parse_logical_or_expression(shared_ptr<NExpression>& expr);
    void parse_logical_and_expression(shared_ptr<NExpression>& expr);
    void parse_inclusive_or_expression(shared_ptr<NExpression>& expr);
    void parse_shift_expression(shared_ptr<NExpression>& expr);
    void parse_additive_expression(shared_ptr<NExpression>& expr);
    void parse_multiplicative_expression(shared_ptr<NExpression>& expr);
    void parse_unary_expression(shared_ptr<NExpression>& expr);
    void parse_primary_expression(shared_ptr<NExpression>& np);

    void parse_brackets(shared_ptr<NExpression>& b);
    void parse_integer(shared_ptr<NIntegerLiteral>& ni);
    void parse_identifier(shared_ptr<NIdentifier>& ni);
    void parse_float(shared_ptr<NFloatLiteral>& nf);
    void parse_boolean(shared_ptr<NBoolLiteral>& nf);
    void parse_method_call(shared_ptr<NMethodCall>& m);
    void parse_string(shared_ptr<NString>& s);
    void parse_extern(shared_ptr<NExtern>& e);

    void parse_indent(int expect);

    void check_precedence(shared_ptr<NExpression>& e);

    bool variable_is_atomic(string var);

    Lexer& lexer;
    int indent = 0;
};

#endif // PARSER_H
