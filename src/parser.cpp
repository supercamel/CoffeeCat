#include "parser.h"

#include <iostream>
using namespace std;

void Parser::parse(NBlock& block)
{
    while(lexer.lex(true).tok != "eof")
    {
        while(lexer.lex(true).tok == "newline")
            lexer.lex();

        try
        {
            parse_top_level(block);
            continue;
        }
        catch(backtrack bte)
        {
        }
        return;
    }
}

void Parser::parse_top_level(NBlock& block)
{
    Lexer l = lexer;
    try
    {
        auto n = make_shared<NMethod>();
        parse_method(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte) { lexer = l; }

    try
    {
        auto n = make_shared<NExtern>();
        parse_extern(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte) { lexer = l; }

    throw(ParseError(lexer.lex(), "Expected method or class"));
}

void Parser::parse_method(shared_ptr<NMethod>& m)
{
    auto tok = lexer.lex();
    if(tok.tok != "def")
        throw(backtrack());

    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected identifier"));

    m->return_type = tok.raw;

    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected method name"));
    m->foo_name = tok.raw;

    try
    {
        auto al = make_shared<NArgumentList>();
        parse_argument_list(al);
        m->args = al;
    }
    catch(backtrack())
    { throw(ParseError(tok, "Syntax error in parameter list")); }

    tok = lexer.lex();
    if(tok.tok != ":")
        throw(ParseError(tok, "Expected ':'"));

    parse_indent(1);
    while(indent >= 1)
    {
        parse_block_item(m->block);
        parse_indent(0);
    }
}

void Parser::parse_argument_list(shared_ptr<NArgumentList>& l)
{
    auto tok = lexer.lex();
    if(tok.tok != "(")
        throw(ParseError(tok, "Expected opening parenthesis ("));

    while(tok.tok != ")")
    {
        try
        {
            auto d = make_shared<NParameterDeclaration>();
            parse_parameter_declaration(d);
            l->args.push_back(d);
        }
        catch(backtrack b)
        {
            tok = lexer.lex();
            if((tok.tok != ",") && (tok.tok != ")"))
                throw(ParseError(tok, "Syntax error in argument list."));
        }
    }
}

void Parser::parse_parameter_declaration(shared_ptr<NParameterDeclaration>& d)
{
    auto tok = lexer.lex(true);
    if((tok.tok != "identifier") && (tok.tok != "var"))
        throw(backtrack());

    lexer.lex();
    if(tok.tok == "var")
        d->type = "auto";
    else
        d->type = tok.raw;

    tok = lexer.lex();
    if(tok.tok == "ref")
        d->refer = true;
    else if(tok.tok == "identifier")
    {
        d->handle = tok.raw;
        return;
    }
    else
        throw(ParseError(tok, "Synax error. Expected identifier"));

    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Syntax error. Expected dientifier"));

    d->handle = tok.raw;
}

void Parser::parse_block_item(NBlock& block)
{
    Lexer l = lexer;
    try
    {
        auto e = make_shared<NExpression>();
        parse_expression(e);
        auto tok = lexer.lex();
        if(tok.tok != "newline")
            throw(ParseError(tok, "Expected newline after expression"));
        block.items.push_back(e);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        parse_declaration(block);
        auto tok = lexer.lex();
        if(tok.tok != "newline")
            throw(ParseError(tok, "Expected newline after declaration"));
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        auto e = make_shared<NExtern>();
        parse_extern(e);
        auto tok = lexer.lex();
        if(tok.tok != "newline")
            throw(ParseError(tok, "Expected newline after extern"));
        block.items.push_back(e);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
}

void Parser::parse_declaration(NBlock& block)
{
    Lexer l = lexer;
    try
    {
        parse_variable_declaration(block);
        return;
    }
    catch(backtrack b) { lexer = l; }

    try
    {
        parse_atomic_declaration(block);
        return;
    }
    catch(backtrack b) { lexer = l; }

    throw(backtrack());
}

void Parser::parse_atomic_declaration(NBlock& block)
{
    auto vd = make_shared<NAtomicVariableDeclaration>();
    auto tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(backtrack());
    if(!variable_is_atomic(tok.raw))
        throw(backtrack());

    vd->type = tok.raw;

    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected identifier after var keyword"));
    vd->handle = tok.raw;

    tok = lexer.lex();
    if(tok.tok != "=")
        throw(ParseError(tok, "Expected assignment operator for variable declaration"));

    try
    {
        auto n = make_shared<NExpression>();
        parse_expression(n);
        vd->initialiser = n;
    }
    catch(backtrack b)
    {
        throw(ParseError(tok, "Error parsing atomic declaration initialiser"));
    }
    block.items.push_back(vd);
}

void Parser::parse_variable_declaration(NBlock& block)
{
    auto vd = make_shared<NVariableDeclaration>();
    auto tok = lexer.lex();
    if(tok.tok != "var")
        throw(backtrack());

    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected identifier after var keyword"));
    vd->handle = tok.raw;

    tok = lexer.lex();
    if(tok.tok != "=")
        throw(ParseError(tok, "Expected assignment operator for variable declaration"));

    try
    {
        auto id = make_shared<NIdentifier>();
        parse_identifier(id);
        vd->type = id->ident;
    }
    catch(backtrack bt)
    {
        throw(ParseError(tok, "Expected identifier"));
    }

    try
    {
        auto n = make_shared<NExpressionList>();
        parse_expression_list(n);
        vd->list = n;
        block.items.push_back(vd);
    }
    catch(backtrack b)
    {
        throw(ParseError(tok, "Error parsing initialiser expression list"));
    }
}

void Parser::parse_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_assignment_expression(e);
        return;
    }
    catch(backtrack bte)
    { lexer = l; }

    throw(backtrack());
}


void Parser::parse_expression_list(shared_ptr<NExpressionList>& l)
{
    auto tok = lexer.lex();
    if(tok.tok != "(")
        throw(backtrack());

    while(tok.tok != ")")
    {
        auto e = make_shared<NExpression>();
        try
        {
            parse_expression(e);
            l->items.push_back(e);
        }
        catch(backtrack b)
        {
            tok = lexer.lex();
            if((tok.tok != ",") && (tok.tok != ")"))
                throw(ParseError(tok, "Syntax error in expression list."));
        }
    }
}

void Parser::parse_assignment_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_logical_or_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        if(t.tok == "=")
            op = BO_ASSIGN;
        else
            throw(backtrack());

        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_assignment_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 1;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        e = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_logical_or_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_logical_or_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_logical_and_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        cout << t.tok << endl;
        if(t.tok == "or")
            op = BO_LOGICAL_OR;
        else
            throw(backtrack());

        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_logical_or_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 2;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        e = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_logical_and_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_logical_and_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_inclusive_or_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        cout << t.tok << endl;
        if(t.tok == "and")
            op = BO_LOGICAL_AND;
        else
            throw(backtrack());

        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_logical_and_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 3;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        e = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_inclusive_or_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_inclusive_or_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_shift_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        cout << t.tok << endl;
        if(t.tok == "|")
            op = BO_INCLUSIVE_OR;
        else
            throw(backtrack());

        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_inclusive_or_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 4;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        e = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_shift_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_shift_expression(shared_ptr<NExpression>& chomp)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_additive_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        if(t.tok == "<<")
            op = BO_LEFT_SHIFT;
        else if(t.tok == ">>")
            op = BO_RIGHT_SHIFT;
        else
            throw(backtrack());

        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_shift_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 9;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        chomp = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_additive_expression(chomp);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_additive_expression(shared_ptr<NExpression>& bo)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_multiplicative_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        if(t.tok == "+")
            op = BO_ADD;
        else if(t.tok == "-")
            op = BO_SUBTRACT;
        else
            throw(backtrack());
        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_additive_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 10;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        bo = tino;
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_multiplicative_expression(bo);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}


void Parser::parse_multiplicative_expression(shared_ptr<NExpression>& bo)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        parse_unary_expression(lhs);
        auto t = lexer.lex();
        BINARY_OPERATOR op;
        if(t.tok == "*")
            op = BO_MULTIPLY;
        else if(t.tok == "/")
            op = BO_DIVIDE;
        else
            throw(backtrack());
        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_multiplicative_expression(rhs);
        auto bino = make_shared<NBinaryOperator>();
        bino->subexpr.push_back(lhs);
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 20;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        bo = tino;
        return;
    }
    catch(backtrack btw) { lexer = l; }
    try
    {
        return parse_unary_expression(bo);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_unary_expression(shared_ptr<NExpression>& expr)
{
    Lexer l = lexer;
    try
    {
        shared_ptr<NExpression> lhs = make_shared<NExpression>();
        auto t = lexer.lex();
        UNARY_OPERATOR op;
        if(t.tok == "-")
            op = UO_NEGATE;
        else
            throw(backtrack());
        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_primary_expression(rhs);
        auto bino = make_shared<NUnaryOperator>();
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 30;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        expr = tino;
        return;
    }
    catch(backtrack bt) { lexer = l; }
    try
    {
        return parse_primary_expression(expr);
    }
    catch(backtrack bt)
    { lexer = l; }
    throw(backtrack());
}

void Parser::parse_primary_expression(shared_ptr<NExpression>& np)
{
    Lexer l = lexer;

    try
    {
        auto ni = make_shared<NMethodCall>();
        parse_method_call(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }


    try
    {
        auto ni = make_shared<NIdentifier>();
        parse_identifier(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    try
    {
        auto ni = make_shared<NIntegerLiteral>();
        parse_integer(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    try
    {
        auto ni = make_shared<NFloatLiteral>();
        parse_float(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    try
    {
        auto ni = make_shared<NBoolLiteral>();
        parse_boolean(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    try
    {
        parse_brackets(np);
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    try
    {
        auto s = make_shared<NString>();
        parse_string(s);
        np = s;
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

    throw(backtrack());
}

void Parser::parse_brackets(shared_ptr<NExpression>& b)
{
    if(lexer.lex().tok != "(")
        throw(backtrack());

    shared_ptr<NExpression> n = make_shared<NExpression>();
    parse_additive_expression(n);

    if(lexer.lex().tok != ")")
        throw(backtrack());

    n->precedence = 100;
    b = n;
}

void Parser::parse_integer(shared_ptr<NIntegerLiteral>& ni)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "int_literal")
        throw(backtrack());
    lexer.lex();
    ni->value = stoi(tok.raw);
}

void Parser::parse_identifier(shared_ptr<NIdentifier>& id)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "identifier")
        throw(backtrack());
    if(variable_is_atomic(tok.raw))
        throw(backtrack());

    if(tok.raw == "string")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected maximum string length to be declared."));
        tok = lexer.lex();
        if(tok.tok != "int_literal")
            throw(ParseError(tok, "Expected an integer to specify maximum string length."));
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected a '>' token after string length"));
        id->ident = "etk::StaticString<";
        id->ident += tok.raw;
        id->ident += ">";
        return;
    }

    lexer.lex();
    id->ident = tok.raw;
}

void Parser::parse_float(shared_ptr<NFloatLiteral>& f)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "float_literal")
        throw(backtrack());
    lexer.lex();
    f->value = stod(tok.raw);
}

void Parser::parse_boolean(shared_ptr<NBoolLiteral>& b)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "bool_literal")
        throw(backtrack());
    lexer.lex();
    if(tok.raw == "true")
        b->value = true;
    else if(tok.raw == "false")
        b->value = false;
    else
        throw(ParseError(tok, "Lexer passed a bool literal that wasn't true or false"));
}

void Parser::parse_method_call(shared_ptr<NMethodCall>& mc)
{
    auto tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(backtrack());
    auto el = make_shared<NExpressionList>();
    mc->handle = tok.raw;
    mc->list = el;
    parse_expression_list(mc->list);
}

void Parser::parse_string(shared_ptr<NString>& s)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "string_literal")
        throw(backtrack());
    lexer.lex();
    s->str = tok.raw;
}

void Parser::parse_extern(shared_ptr<NExtern>& e)
{
    auto tok = lexer.lex(true);
    if(tok.tok != "extern")
        throw(backtrack());

    lexer.lex();
    tok = lexer.lex();
    if(tok.tok != "(")
        throw(ParseError(tok, "Expected opening parenthesis after 'extern'"));
    tok = lexer.lex();
    if(tok.tok != "string_literal")
        throw(ParseError(tok, "Expected string literal"));
    e->code = tok.raw;
    tok = lexer.lex();
    if(tok.tok != ")")
        throw(ParseError(tok, "Expected closing parenthesis"));
}

void Parser::parse_indent(int expect)
{
    indent = 0;
    while((lexer.lex(true).tok == "indent") || (lexer.lex(true).tok == "newline"))
    {
        indent++;
        auto tok = lexer.lex();
        if(tok.tok == "newline")
            indent = 0;
    }
    if(indent < expect)
        throw(IndentError(lexer.lex()));
}

void Parser::check_precedence(shared_ptr<NExpression>& e)
{
    auto rhs = e->subexpr.back();

    if(rhs->precedence <= e->precedence)
    {
        if(rhs->subexpr.size())
        {
            auto temp = rhs->subexpr.front();
            rhs->subexpr.pop_front();
            e->subexpr.pop_back();
            e->subexpr.push_back(temp);
            rhs->subexpr.push_front(e);
            e = rhs;
        }
    }
}

bool Parser::variable_is_atomic(string var)
{
    if(var == "char")
        return true;
    if(var == "int")
        return true;
    if(var == "uint")
        return true;
    if(var == "int8")
        return true;
    if(var == "int16")
        return true;
    if(var == "int32")
        return true;
    if(var == "int64")
        return true;
    if(var == "uint8")
        return true;
    if(var == "uint16")
        return true;
    if(var == "uint32")
        return true;
    if(var == "uint64")
        return true;

    if(var == "float")
        return true;
    if(var == "double")
        return true;
    if(var == "bool")
        return true;
    return false;
}
