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
            while(lexer.lex(true).tok == "newline")
                lexer.lex();
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
    parse_indent(0);
    try
    {
        auto n = make_shared<NMethod>();
        parse_method(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        auto n = make_shared<NClass>();
        parse_class(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        auto n = make_shared<NExtern>();
        parse_extern(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        auto n = make_shared<NEnum>();
        parse_enum(n);
        block.items.push_back(n);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    if(lexer.lex(true).tok == "global")
    {
        lexer.lex();
        auto n = make_shared<NVariableDeclaration>();
        parse_declaration(n);

        n->global = true;
        block.items.push_back(n);
        return;
    }

    throw(ParseError(lexer.lex(), "Expected method, class or global variable declaration"));
}

void Parser::parse_class(shared_ptr<NClass>& c)
{
    auto tok = lexer.lex();
    if(tok.tok != "class")
        throw(backtrack());
    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expcted an identifier after class keyword"));
    c->handle = tok.raw;

    tok = lexer.lex();
    if(tok.tok != ":")
        throw(ParseError(tok, "Expected ':' after class declaration"));

    tok = lexer.lex();
    if(tok.tok == "identifier")
    {
        c->parent = tok.raw;
        tok = lexer.lex();
    }

    if(tok.tok != "newline")
        throw(ParseError(tok, "Expected new line after ':'"));

    indent++;
    parse_indent(indent);
    while(true)
    {
        auto l = lexer;
        try
        {
            auto m = make_shared<NMethod>();
            parse_method(m);
            c->methods.push_back(m);
            if(last_indent < indent)
            {
                indent--;
                return;
            }
            continue;
        }
        catch(backtrack b)
        {
            lexer = l;
        }

        try
        {
            auto v = make_shared<NVariableDeclaration>();
            parse_declaration(v);
            if(lexer.lex().tok != "newline")
                throw ParseError(lexer.lex(), "Expected new line after variable declaration");
            c->vars.push_back(v);
            goto newline;
        }
        catch(backtrack b)
        {
            lexer = l;
        }

        try
        {
            auto cl = make_shared<NClass>();
            parse_class(cl);
            c->subclasses.push_back(cl);
            goto newline;
        }
        catch(backtrack b)
        {
            lexer = l;
        }

        if(lexer.lex(true).tok == "pass")
        {
            lexer.lex();
            goto newline;
        }

        throw ParseError(lexer.lex(), "Parse error");
        newline:

        int i = parse_indent(0);
        if(i < indent)
        {
            indent--;
            return;
        }
    }

}

void Parser::parse_enum(shared_ptr<NEnum>& c)
{
    auto tok = lexer.lex();
    if(tok.tok != "enum")
        throw(backtrack());
    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expcted an identifier after enum keyword"));
    c->handle = tok.raw;

    tok = lexer.lex();
    if(tok.tok != ":")
        throw(ParseError(tok, "Expected ':' after enum declaration"));

    tok = lexer.lex();
    if(tok.tok != "newline")
        throw(ParseError(tok, "Expected new line after ':'"));

    indent++;
    parse_indent(indent);

    if(lexer.lex(true).tok != "(")
        throw(ParseError(lexer.lex(), "Expected '(' on line after enum declaration"));
    
    lexer.lex();

    while(true)
    {
        tok = lexer.lex(true);
        if(tok.tok == "identifier")
        {
            c->numbers.push_back(tok.raw);
            lexer.lex();

            tok = lexer.lex();
            if(tok.tok == ")")
                break;
            
            if(tok.tok != "newline")
                throw(ParseError(lexer.lex(), "Expected new line after identifier in enum list"));
            parse_indent(indent);
        }
        else
            throw(ParseError(lexer.lex(), "Expected identifier"));
    }

    int i = parse_indent(0);
    if(i < indent)
    {
        indent--;
        return;
    }

    while(true)
    {
        auto l = lexer;
        try
        {
            auto m = make_shared<NMethod>();
            parse_method(m);
            c->methods.push_back(m);
            if(last_indent < indent)
            {
                indent--;
                return;
            }
            continue;
        }
        catch(backtrack b)
        {
            throw(ParseError(lexer.lex(), "Error parsing enum method"));
        }
    }

}


void Parser::parse_method(shared_ptr<NMethod>& m)
{
    auto id = make_shared<NIdentifier>();
    parse_identifier(id, true);

    m->return_type = id->ident;
    auto tok = lexer.lex();
    m->foo_name = "";

    if(tok.tok == "~") //special case for destructors
    {
        m->foo_name += "~";
        tok = lexer.lex();
    }

    if(tok.tok != "identifier")
        throw(backtrack());
    m->foo_name += tok.raw;
    if(lexer.lex(true).tok != "(")
        throw(backtrack());
    try
    {
        auto al = make_shared<NArgumentList>();
        parse_argument_list(al);
        m->args = al;
    }
    catch(backtrack())
    {
        throw(ParseError(tok, "Syntax error in parameter list"));
    }
    tok = lexer.lex();
    if(tok.tok != ":")
        throw(ParseError(tok, "Expected ':' after method declaration"));

    parse_block(m->block);
    last_indent = parse_indent(0);
}

void Parser::parse_argument_list(shared_ptr<NArgumentList>& l)
{
    auto tok = lexer.lex();
    if(tok.tok != "(")
        throw(ParseError(tok, "Expected opening parenthesis ("));
    while(tok.tok != ")")
    {
        auto lex = lexer;
        try
        {
            auto d = make_shared<NParameterDeclaration>();
            parse_parameter_declaration(d);
            l->args.push_back(d);
        }
        catch(backtrack b)
        {
            lexer = lex;
            tok = lexer.lex();
            if((tok.tok != ",") && (tok.tok != ")"))
                throw(ParseError(tok, "Syntax error in argument list."));
        }
    }
}

void Parser::parse_parameter_declaration(shared_ptr<NParameterDeclaration>& d)
{
    auto tok = lexer.lex();
    if((tok.tok != "identifier") && (tok.tok != "var"))
    {
        if((tok.tok != "in") && (tok.tok != "out") && (tok.tok != "copy"))
            throw(backtrack());

        if(tok.tok == "out")
            d->output = true;
        if(tok.tok == "copy")
            d->clone = true;

        tok = lexer.lex();
    }

    if((tok.tok != "identifier") && (tok.tok != "var"))
        throw(backtrack());

    if(tok.tok == "var")
        d->type = "auto";
    else
        d->type = tok.raw;
    tok = lexer.lex();
    if(tok.tok == "identifier")
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

void Parser::parse_block(NBlock& block)
{
    indent++;

    parse_indent(indent);
    while(true)
    {
        parse_block_item(block);
        Lexer l = lexer;
        int i = parse_indent(0);
        if(i < indent)
        {
            lexer = l;
            indent--;
            return;
        }
    }
}

int Parser::parse_indent(int expect)
{
    int i = 0;
    while((lexer.lex(true).tok == "indent") || (lexer.lex(true).tok == "newline"))
    {
        i++;
        auto tok = lexer.lex();
        if(tok.tok == "newline")
            i = 0;
    }
    if(i < expect)
        throw(IndentError(lexer.lex()));

    return i;
}

void Parser::parse_block_item(NBlock& block)
{
    Lexer l = lexer;
    try
    {
        auto d = make_shared<NVariableDeclaration>();
        parse_declaration(d);
        auto tok = lexer.lex();
        if(tok.tok != "newline")
            throw(ParseError(tok, "Expected newline after declaration"));
        block.items.push_back(d);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }

    try
    {
        auto ie = make_shared<NIfElse>();
        parse_if_else_statement(ie);
        block.items.push_back(ie);
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        auto r = make_shared<NReturn>();
        parse_return_statement(r);
        block.items.push_back(r);
        return;
    }
    catch(backtrack bt)
    { lexer = l; }

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
    { lexer = l; }

    try
    {
        auto e = make_shared<NWhile>();
        parse_while_statement(e);
        block.items.push_back(e);
        return;
    }
    catch(backtrack bte)
    { lexer = l; }


    try
    {
        auto e = make_shared<NFor>();
        parse_for_statement(e);
        block.items.push_back(e);
        return;
    }
    catch(backtrack bte)
    { lexer = l; }

    try
    {
        auto c = make_shared<NControl>();
        parse_control_statement(c);
        block.items.push_back(c);
        return;
    }
    catch(backtrack bte)
    { lexer = l; }

    try
    {
        auto e = make_shared<NExpression>();
        parse_expression(e);
        while(lexer.lex(true).tok == "indent")
            lexer.lex();

        auto tok = lexer.lex();
        if(tok.tok != "newline")
            throw(ParseError(tok, "Expected newline after expression"));
        block.items.push_back(e);
        return;
    }
    catch(backtrack bte)
    { lexer = l; }

    auto tok = lexer.lex();
    throw ParseError(tok, "Did not expect " + tok.raw);
}

void Parser::parse_if_else_statement(shared_ptr<NIfElse>& ie)
{
    if(lexer.lex(true).tok != "if")
        throw(backtrack());
    lexer.lex();
    try
    {
        parse_expression(ie->condition);
    }
    catch(backtrack bt)
    {
        throw(ParseError(lexer.lex(), "Expected expression after if statement"));
    }
    if(lexer.lex().tok != ":")
        throw(ParseError(lexer.lex(), "Expected : after if expression"));

    ie->block_if = make_shared<NBlock>();

    parse_block(*ie->block_if);

    Lexer l = lexer;
    int i = parse_indent(0);

    if(lexer.lex(true).tok == "else")
    {
        if(i != indent)
            throw(IndentError(lexer.lex()));

        ie->block_else = make_shared<NBlock>();
        lexer.lex();
        auto t = lexer.lex(true);
        if(t.tok == ":")
        {
            lexer.lex();
            parse_block(*ie->block_else);
            return;
        }
        else if(t.tok == "if")
        {
            auto sie = make_shared<NIfElse>();
            parse_if_else_statement(sie);
            ie->block_else->items.push_back(sie);
            return;
        }
        else
        {
            throw(ParseError(t, "Expected either : or if after else statement"));
        }
    }
    lexer = l;
}

void Parser::parse_return_statement(shared_ptr<NReturn>& r)
{
    if(lexer.lex(true).tok != "return")
        throw(backtrack());

    lexer.lex();

    try
    {
        auto e = make_shared<NExpression>();
        parse_expression(e);
        r->expr = e;
    }
    catch(backtrack bt)
    { }
}

void Parser::parse_while_statement(shared_ptr<NWhile>& w)
{
    if(lexer.lex(true).tok != "while")
        throw(backtrack());
    lexer.lex();

    try
    {
        auto e = make_shared<NExpression>();
        parse_expression(e);
        w->condition = e;
    }
    catch(backtrack bt)
    {
        throw(ParseError(lexer.lex(), "Expected expression after while statement"));
    }
    if(lexer.lex().tok != ":")
        throw(ParseError(lexer.lex(), "Expected : after while condition"));

    w->block = make_shared<NBlock>();
    parse_block(*w->block);
}


void Parser::parse_for_statement(shared_ptr<NFor>& f)
{
    if(lexer.lex(true).tok != "for")
        throw(backtrack());
    lexer.lex();
    auto tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(lexer.lex(), "Expected identifier after for statement"));
    f->lhs = make_shared<NIdentifier>();
    f->lhs->ident = tok.raw;

    tok = lexer.lex();
    if(tok.tok != "in")
        throw(ParseError(tok, "Expected 'in' keyword after identifier"));

    Lexer l = lexer;
    try
    {
        auto n = make_shared<NExpression>();
        parse_expression(n);
        f->rhs = static_pointer_cast<NExpression>(n);
    }
    catch(backtrack bt)
    {
        { throw ParseError(lexer.lex(), "Expected an expression after 'in'"); }
    }
    if(lexer.lex().tok != ":")
        throw ParseError(lexer.lex(), "Expected ':' after for statement");

    f->block = make_shared<NBlock>();
    parse_block(*f->block);
}

void Parser::parse_control_statement(shared_ptr<NControl>& c)
{
    auto tok = lexer.lex();
    if(tok.tok == "continue")
        c->keyword = "continue";
    else if(tok.tok == "break")
        c->keyword = "break";
    else
        throw(backtrack());

    tok = lexer.lex();
    if(tok.tok != "newline")
        throw(ParseError(lexer.lex(), "Expected new line after control statement"));
}

void Parser::parse_declaration(shared_ptr<NVariableDeclaration>& v)
{
    auto tok = lexer.lex();
    if((tok.tok != "identifier") && (tok.tok != "var"))
        throw(backtrack());
    if((!variable_is_atomic(tok.raw)) && (tok.tok != "var"))
        throw(backtrack());
    v->type = tok.raw;
    if(tok.tok == "var")
        v->type = "auto";
    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected identifier after type keyword"));
    v->handle = tok.raw;
    tok = lexer.lex();
    if(tok.tok != "=")
        throw(ParseError(tok, "Expected assignment operator for variable declaration"));

    auto l = lexer;
    try
    {
        if(lexer.lex(true).raw == "shared")
        {
            auto d = make_shared<NExpression>();
            parse_shared_expression(d);
            v->type = "pool_pointer<";
            v->type += ((NShared*)&(*d))->type;
            v->type += ">";
            v->initialiser = d;
            return;
        }
        else
        {
                auto id = make_shared<NIdentifier>();
                auto n = make_shared<NExpression>();

                string ident = lexer.lex(true).raw;
                try
                {
                    parse_identifier(id);
                }
                catch(...)
                {

                }
                
                lexer = l;
                parse_expression(n);

                v->initialiser = n;
                if((ident == "String") || (ident == "Pool") || (ident == "Array") || (ident == "List"))
                    v->type = id->ident;
        }
    }
    catch(backtrack b)
    {
        lexer = l;
        throw(ParseError(tok, "Error parsing declaration initialiser"));
    }
}

/*
void Parser::parse_atomic_declaration(shared_ptr<NAtomicVariableDeclaration>& vd)
{
    auto tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(backtrack());
    if(!variable_is_atomic(tok.raw))
        throw(backtrack());
    vd->type = tok.raw;
    tok = lexer.lex();
    if(tok.tok != "identifier")
        throw(ParseError(tok, "Expected identifier after atomic type keyword"));
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
}
/*
void Parser::parse_variable_declaration(shared_ptr<NObjVariableDeclaration>& vd)
{
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


    if(lexer.lex(true).tok == "shared")
    {
        lexer.lex();
        vd->copyable = true;
        if(lexer.lex(true).tok == "<")
        {
            lexer.lex();
            tok = lexer.lex();
            if(tok.tok != "identifier")
                throw(ParseError(tok, "Expected pool name"));
            vd->pool = tok.raw;
            if(lexer.lex().tok != ",")
                throw(ParseError(tok, "Expected comma after pool name"));
            tok = lexer.lex();
            if(tok.tok != "int_literal")
                throw(ParseError(tok, "Expected int literal to specify pool size"));
            vd->pool_size = stoi(tok.raw);
            if(lexer.lex().tok != ">")
                throw(ParseError(tok, "Expected > after int literal"));
        }
    }

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
    }
    catch(backtrack b)
    {
        throw(ParseError(tok, "Error parsing initialiser expression list"));
    }
}
*/

void Parser::parse_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    if(lexer.lex().tok == "pass")
        return;
    lexer = l;
    try
    {
        parse_assignment_expression(e);
        return;
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
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
        parse_binary_expression(e, {BO_ASSIGN, BO_MUL_ASSIGN, BO_DIV_ASSIGN, BO_MOD_ASSIGN,
                                    BO_ADD_ASSIGN, BO_SUB_ASSIGN, BO_LEFT_SHIFT_ASSIGN,
                                    BO_RIGHT_SHIFT_ASSIGN, BO_AND_ASSIGN, BO_XOR_ASSIGN, BO_OR_ASSIGN},
                                    {"=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|="}, 1,
                                [&](shared_ptr<NExpression>& e) { parse_assignment_expression(e);},
                                [&](shared_ptr<NExpression>& e) { parse_unary_expression(e);} );
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
        parse_binary_expression(e, {BO_LOGICAL_OR}, {"or"}, 2,
                [&](shared_ptr<NExpression>& e) { parse_logical_or_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_logical_and_expression(e);} );
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
        parse_binary_expression(e, {BO_LOGICAL_AND}, {"and"}, 3,
                [&](shared_ptr<NExpression>& e) { parse_logical_and_expression(e);} ,
                [&](shared_ptr<NExpression>& e) { parse_inclusive_or_expression(e);} );
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
        parse_binary_expression(e, {BO_INCLUSIVE_OR}, {"|"}, 4,
                [&](shared_ptr<NExpression>& e) { parse_inclusive_or_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_exclusive_or_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_exclusive_or_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_exclusive_or_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_EXCLUSIVE_OR}, {"^"}, 5,
                [&](shared_ptr<NExpression>& e) { parse_exclusive_or_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_bitwise_and_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_bitwise_and_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_bitwise_and_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_BITWISE_AND}, {"&"}, 6,
                [&](shared_ptr<NExpression>& e) { parse_bitwise_and_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_equality_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_equality_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_equality_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_EQUAL, BO_NOT_EQUAL}, {"==", "!="}, 7,
                [&](shared_ptr<NExpression>& e) { parse_equality_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_relational_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_relational_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_relational_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_LESS_THAN_EQUAL, BO_GREATER_THAN_EQUAL,
                                    BO_LESS_THAN, BO_GREATER_THAN},
                                    {"<=", ">=", "<", ">"}, 8,
                [&](shared_ptr<NExpression>& e) { parse_relational_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_shift_expression(e);} );
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

void Parser::parse_shift_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_LEFT_SHIFT, BO_RIGHT_SHIFT}, {"<<", ">>"}, 9,
                [&](shared_ptr<NExpression>& e) { parse_shift_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_additive_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_additive_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_additive_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_ADD, BO_SUBTRACT}, {"+", "-"}, 10,
                [&](shared_ptr<NExpression>& e) { parse_additive_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_multiplicative_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_multiplicative_expression(e);
    }
    catch(backtrack bte)
    {
        lexer = l;
    }
    throw(backtrack());
}


void Parser::parse_multiplicative_expression(shared_ptr<NExpression>& e)
{
    Lexer l = lexer;
    try
    {
        parse_binary_expression(e, {BO_MULTIPLY, BO_DIVIDE}, {"*", "/"}, 20,
                [&](shared_ptr<NExpression>& e) { parse_multiplicative_expression(e);},
                [&](shared_ptr<NExpression>& e) { parse_unary_expression(e);} );
        return;
    }
    catch(backtrack btw)
    {
        lexer = l;
    }
    try
    {
        return parse_unary_expression(e);
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

    auto t = lexer.lex();
    UNARY_OPERATOR op;
    try
    {
        if(t.tok == "-")
            op = UO_NEGATE;
        else if(t.tok == "not")
            op = UO_NOT;
        else if(t.tok == "~")
            op = UO_BITWISE_NOT;
        else
            throw(backtrack());
        shared_ptr<NExpression> rhs = make_shared<NExpression>();
        parse_shared_expression(rhs);
        auto bino = make_shared<NUnaryOperator>();
        bino->subexpr.push_back(rhs);
        bino->op = op;
        bino->precedence = 30;
        shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
        check_precedence(tino);
        expr = tino;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        return parse_shared_expression(expr);
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_shared_expression(shared_ptr<NExpression>& expr)
{
    Lexer l = lexer;

    auto t = lexer.lex();
    try
    {
        if(t.tok != "shared")
            throw(backtrack());

        auto ns = make_shared<NShared>();

        t = lexer.lex();
        if(t.tok != "<")
            throw(ParseError(t, "Expected '<' after shared"));
        auto id = lexer.lex();
        if(id.tok != "identifier")
            throw(ParseError(t, "Expected identifier as first parameter to 'shared'"));
        ns->type = id.raw;
        t = lexer.lex();
        if(t.tok != ",")
            throw(ParseError(t, "Expected ',' after shared<identifier"));
        t = lexer.lex();
        if(t.tok != "identifier")
            throw(ParseError(t, "Expected identifier as second parameter to 'shared'"));
        ns->pool_ident = t.raw;
        t = lexer.lex();
        if(t.tok != ">")
            throw(ParseError(t, "Expected '>' after shared"));


        ns->args = make_shared<NExpressionList>();
        parse_expression_list(ns->args);

        expr = ns;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        return parse_primary_expression(expr);
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_primary_expression(shared_ptr<NExpression>& np)
{
    Lexer l = lexer;

    try
    {
        parse_dot_list(np);
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        auto ni = make_shared<NMethodCall>();
        parse_method_call(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    try
    {
        auto ni = make_shared<NIdentifier>();
        parse_identifier(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        auto ni = make_shared<NCharLiteral>();
        parse_char(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }

    try
    {
        auto ni = make_shared<NIntegerLiteral>();
        parse_integer(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    try
    {
        auto ni = make_shared<NFloatLiteral>();
        parse_float(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    try
    {
        auto ni = make_shared<NBoolLiteral>();
        parse_boolean(ni);
        np = ni;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    try
    {
        parse_brackets(np);
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    try
    {
        auto s = make_shared<NString>();
        parse_string(s);
        np = s;
        return;
    }
    catch(backtrack bt)
    {
        lexer = l;
    }
    throw(backtrack());
}

void Parser::parse_dot_list(shared_ptr<NExpression>& dlist)
{
    Lexer l = lexer;
    auto d = make_shared<NDot>();
    try
    {
        auto e = make_shared<NMethodCall>();
        parse_method_call(e);
        d->subexpr.push_back(e);
    }
    catch(backtrack b)
    {
        lexer = l;

            auto e = make_shared<NIdentifier>();
            parse_identifier(e);
            d->subexpr.push_back(e);

    }

    if((lexer.lex(true).tok != ".") && (lexer.lex(true).tok != "->"))
    {
        dlist = (*d->subexpr.begin());
        return;
    }

    auto tok = lexer.lex();
    if(tok.tok == "->")
        d->ptr = true;
    try
    {
        auto e = make_shared<NExpression>();
        parse_dot_list(e);
        d->subexpr.push_back(e);
    }
    catch(backtrack())
    {
        throw(ParseError(lexer.lex(), "Syntax error"));
    }

    dlist = d;
}

void Parser::parse_brackets(shared_ptr<NExpression>& b)
{
    auto br = make_shared<NBrackets>();
    if(lexer.lex().tok != "(")
        throw(backtrack());
    if(lexer.lex(true).tok != ")")
    {
        auto sub = make_shared<NExpression>();
        parse_expression(sub);
        br->subexpr.push_back(sub);
        br->precedence = 100;
    }
    if(lexer.lex().tok != ")")
        throw(backtrack());
    b = static_pointer_cast<NExpression>(br);
}

void Parser::parse_integer(shared_ptr<NIntegerLiteral>& ni)
{
    auto tok = lexer.lex(true);

    if(tok.tok == "int_literal")
        ni->value = stoi(tok.raw);
    else
        throw(backtrack());

    lexer.lex();
}

void Parser::parse_char(shared_ptr<NCharLiteral>& c)
{
    auto tok = lexer.lex(true);

    if(tok.tok == "char_literal")
        c->value = tok.raw;
    else
        throw(backtrack());
    lexer.lex();
}

void Parser::parse_identifier(shared_ptr<NIdentifier>& id, bool can_be_atomic)
{
    auto tok = lexer.lex(true);
    auto t = tok;

    if(t.tok == "out")
    {
        id->output = true;
        lexer.lex();
        tok = lexer.lex(true);
    }
    else if(t.tok == "copy")
    {
        id->copyable = true;
        lexer.lex();
        tok = lexer.lex(true);
    }

    if(tok.tok != "identifier")
        throw(backtrack());
    if((variable_is_atomic(tok.raw)) && (!can_be_atomic))
        throw(backtrack());
    if(tok.raw == "String")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected maximum string length to be declared."));
        tok = lexer.lex();
        if((tok.tok != "int_literal") && (tok.tok != "identifier"))
            throw(ParseError(tok, "Expected an integer to specify maximum string length."));
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected a '>' token after string length"));
        id->ident = "etk::StaticString<";
        id->ident += tok.raw;
        id->ident += ">";
        return;
    }
    else if(tok.raw == "Pool")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected maximum pool size to be declared."));
        tok = lexer.lex();
        if((tok.tok != "int_literal") && (tok.tok != "identifier"))
            throw(ParseError(tok, "Expected an integer to specify maximum pool size."));
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected a '>' token after pool size"));
        id->ident = "etk::Pool<";
        id->ident += tok.raw;
        id->ident += ">";
        return;
    }
    else if(tok.raw == "Array")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected maximum array size to be declared."));

        string type;
        try
        {
            auto ident = make_shared<NIdentifier>();
            parse_identifier(ident, true);
            type = ident->ident;
        }
        catch(backtrack b)
        {
            throw(ParseError(lexer.lex(), "Expected identifier"));
        }

        tok = lexer.lex();
        if(tok.tok != ",")
            throw(ParseError(tok, "Expected ','"));
        tok = lexer.lex();
        if((tok.tok != "int_literal") && (tok.tok != "identifier"))
            throw(ParseError(tok, "Expected an integer to specify array size."));
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected a '>' token after array size"));
        id->ident = "etk::Array<";
        id->ident += type + ", ";
        id->ident += tok.raw;
        id->ident += ">";
        return;
    }
    else if(tok.raw == "List")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected maximum list size to be declared."));
                string type;
        try
        {
            auto ident = make_shared<NIdentifier>();
            parse_identifier(ident, true);
            type = ident->ident;
        }
        catch(backtrack b)
        {
            throw(ParseError(lexer.lex(), "Expected identifier"));
        }

        tok = lexer.lex();
        if(tok.tok != ",")
            throw(ParseError(tok, "Expected ','"));
        tok = lexer.lex();
        if((tok.tok != "int_literal") && (tok.tok != "identifier"))
            throw(ParseError(tok, "Expected an integer to specify list size."));
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected a '>' token after list size"));
        id->ident = "etk::List<";
        id->ident += type + ", ";
        id->ident += tok.raw;
        id->ident += ">";
        return;
    }
    else if(tok.raw == "static_cast")
    {
        lexer.lex();
        if(lexer.lex().tok != "<")
            throw(ParseError(tok, "Expected '<' after static_cast"));
        tok = lexer.lex();
        if(tok.tok != "identifier")
            throw(ParseError(tok, "Expected identifier as first parameter to static_cast."));
        string type = tok.raw;
        if(lexer.lex().tok != ",")
            throw(ParseError(tok, "Expected ','"));
        tok = lexer.lex();
        if(tok.tok != "identifier")
            throw(ParseError(tok, "Expected variable as second argument to static_cast"));
        string v = tok.raw;
        if(lexer.lex().tok != ">")
            throw(ParseError(tok, "Expected '>'"));

        //pool_pointer<Node>(rh.get_pool(), static_cast<Node*>(&(*rhs)));
        id->ident = "pool_pointer<";
        id->ident += type;
        id->ident += ">(";
        id->ident += v + ".get_pool(), static_cast<";
        id->ident += type + "*>(&(*";
        id->ident += v + ")))";
        return;
    }

    lexer.lex();
    if(tok.raw == "this")
        id->ident = "(*this)";
    else
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
    auto ident = make_shared<NIdentifier>();
    parse_identifier(ident);

    auto el = make_shared<NExpressionList>();
    mc->handle = ident->ident;
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
    if((tok.tok != "extern") && (tok.tok != "extern_header"))
        throw(backtrack());

    if(tok.tok == "extern_header")
        e->header = true;
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

void Parser::parse_binary_expression(shared_ptr<NExpression>& e,
                                     vector<BINARY_OPERATOR> ops,
                                     vector<string> toks, int precedence,
                                     std::function<void (shared_ptr<NExpression>&)> recurse,
                                     std::function<void (shared_ptr<NExpression>&)> subexpr)
{
    auto tok = lexer.lex(true);
    int count = 0;
    while(count < 100)
    {
        if(tok.tok == "newline")
            throw(backtrack());
        if(tok.tok == "eof")
            throw(backtrack());

        for(auto t : toks)
        {
            if(tok.tok == t)
            {
                count = 100;
                break;
            }
        }
        tok = lexer.lex(true, count++);
    }

    shared_ptr<NExpression> lhs = make_shared<NExpression>();
    subexpr(lhs);
    auto t = lexer.lex();
    BINARY_OPERATOR op;
    bool got_tok = false;
    count = 0;
    for(auto o : toks)
    {
        if(t.tok == o)
        {
            op = ops[count];
            got_tok = true;
            break;
        }
        count++;
    }
    if(!got_tok)
        throw(backtrack());

    shared_ptr<NExpression> rhs = make_shared<NExpression>();
    recurse(rhs);
    auto bino = make_shared<NBinaryOperator>();
    bino->subexpr.push_back(lhs);
    bino->subexpr.push_back(rhs);
    bino->op = op;
    bino->precedence = precedence;
    shared_ptr<NExpression> tino = static_pointer_cast<NExpression>(bino);
    check_precedence(tino);
    e = tino;
    return;
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
    if(var == "void")
        return true;
    return false;
}
