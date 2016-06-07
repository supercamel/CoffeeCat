#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "nodes.h"
#include <functional>

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
    void parse_class(shared_ptr<NClass>& cl);

    void parse_block(NBlock& block);
    void parse_block_item(NBlock& block);

    void parse_if_else_statement(shared_ptr<NIfElse>& ie);
    void parse_return_statement(shared_ptr<NReturn>& ie);
    void parse_while_statement(shared_ptr<NWhile>& w);
    void parse_for_statement(shared_ptr<NFor>& f);
    void parse_control_statement(shared_ptr<NControl>& c);

    void parse_declaration(shared_ptr<NVariableDeclaration>& v);
    void parse_variable_declaration(shared_ptr<NObjVariableDeclaration>& v);
    void parse_atomic_declaration(shared_ptr<NAtomicVariableDeclaration>& a);

    void parse_argument_list(shared_ptr<NArgumentList>& l);
    void parse_parameter_declaration(shared_ptr<NParameterDeclaration>& p);

    void parse_expression_list(shared_ptr<NExpressionList>& l);

    void parse_expression(shared_ptr<NExpression>& expr);
    void parse_assignment_expression(shared_ptr<NExpression>& expr);
    void parse_logical_or_expression(shared_ptr<NExpression>& expr);
    void parse_logical_and_expression(shared_ptr<NExpression>& expr);
    void parse_inclusive_or_expression(shared_ptr<NExpression>& expr);
    void parse_exclusive_or_expression(shared_ptr<NExpression>& expr);
    void parse_bitwise_and_expression(shared_ptr<NExpression>& expr);
    void parse_equality_expression(shared_ptr<NExpression>& expr);
    void parse_relational_expression(shared_ptr<NExpression>& expr);
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
    void parse_string(shared_ptr<NString>& s);
    void parse_extern(shared_ptr<NExtern>& e);
    void parse_method_call(shared_ptr<NMethodCall>& m);
    void parse_dot_list(shared_ptr<NExpression>& expr);

    /*
        Parses an indent and throws an indentation exception if the indent is not as expected.
        returns number of indents parsed
    */
    int parse_indent(int expect);

    /*
        All of the binary operators (addition, subtraction and so on) are all parsed in a similar way.
        There's a left hand size, a right hand side and an operator with a particular precedence.

        parse_binary_expression() handles all binary operators and uses lambda functions for recursion.
    */
    void parse_binary_expression(shared_ptr<NExpression>& e,
                                    vector<BINARY_OPERATOR> ops,
                                    vector<string> toks, int precedence,
                                    std::function<void (shared_ptr<NExpression>& e)> recurse,
                                    std::function<void (shared_ptr<NExpression>& e)> subexpr);
    void check_precedence(shared_ptr<NExpression>& e);

    /*
        Returns true if a variable name is atomic. ie int, float, double etc
    */
    static bool variable_is_atomic(string var);

    Lexer& lexer;
    int indent = 0; //current indent level
    int last_indent = 0;
};

#endif // PARSER_H
