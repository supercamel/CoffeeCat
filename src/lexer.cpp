#include "lexer.h"
#include <sstream>
#include <iostream>
#include <cctype>

Lexer::Lexer(string& text) : text(text)
{
    col = line = text_pos = 0;
    line = 1;
    this->text += "\n\0";
}

Lexer::~Lexer()
{
    //dtor
}

Token Lexer::lex(bool peeking, int peak_ahead)
{
    Token t;
    if(peeking)
    {
        Lexer backup = *this;
        for(int i = 0; i < peak_ahead; i++)
            t = get_token();
        t = get_token();
        *this = backup;
    }
    else
        t = get_token();
    return t;
}

Token Lexer::get_token()
{
    string raw_tok;
    while(text_pos < text.length()-1)
    {
        if(curc() == '\r')
        {
            next();
            continue;
        }
        if(curc() == '\t')
        {
            next();
            return Token("indent", "    ", line, col);
        }
        if(curc() == '\n')
        {
            next();
            return Token("newline", "\n", line, col);
        }
        if(curc() == ' ')
        {
            next();
            if(curc() == ' ')
            {
                next();
                if(curc() == ' ')
                {
                    next();
                    if(curc() == ' ')
                    {
                        next();
                        return Token("indent", "    ", line, col);
                    }
                }
            }
            continue;
        }
        raw_tok += curc();
        if(curc() == '"')
        {
            string s = read_string();
            return Token("string_literal", s, line, col);
        }
        else if(curc() == '*')
        {
            next();
            if(curc() == '=')
                return Token("*=", raw_tok, line, col);
            return Token("*", raw_tok, line, col);
        }
        else if(curc() == '/')
        {
            next();
            if(curc() == '=')
                return Token("/=", raw_tok, line, col);
            if(curc() == '*')
            {
                block_comment();
                continue;
            }
            return Token("/", raw_tok, line, col);
        }
        else if(curc() == '+')
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token("+=", raw_tok, line, col);
            }
            return Token("+", raw_tok, line, col);
        }
        else if(curc() == '-')
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token("-=", raw_tok, line, col);
            }
            return Token("-", "-", line, col);
        }
        else if(curc() == '(')
        {
            next();
            return Token("(", raw_tok, line, col);
        }
        else if(curc() == ')')
        {
            next();
            return Token(")", raw_tok, line, col);
        }
        else if(curc() == ':')
        {
            next();
            return Token(":", raw_tok, line, col);
        }
        else if(curc() == '=')
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token("==", raw_tok, line, col);
            }
            return Token("=", raw_tok, line, col);
        }
        else if(curc() == '!')
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token("!=", raw_tok, line, col);
            }
            die("Unexpected character");
        }
        else if(curc() == ',')
        {
            next();
            return Token(",", raw_tok, line, col);
        }
        else if(curc() == '<')
        {
            next();
            if(curc() == '<')
            {
                next();
                return Token("<<", raw_tok, line, col);
            }
            if(curc() == '=')
            {
                next();
                return Token("<=", raw_tok, line, col);
            }
            return Token("<", raw_tok, line, col);
        }
        else if(curc() == '>')
        {
            next();
            if(curc() == '>')
            {
                next();
                return Token(">>", raw_tok, line, col);
            }
            if(curc() == '=')
            {
                next();
                return Token(">=", raw_tok, line, col);
            }
            return Token(">", raw_tok, line, col);
        }
        else if(curc() == '.')
        {
            next();
            return Token(".", raw_tok, line, col);
        }
        else if(curc() == '|')
        {
            next();
            return Token("|", raw_tok, line, col);
        }
        else if(curc() == '^')
        {
            next();
            return Token("^", raw_tok, line, col);
        }
        else if(curc() == '&')
        {
            next();
            return Token("&", raw_tok, line, col);
        }
        else if(curc() == '~')
        {
            next();
            return Token("~", raw_tok, line, col);
        }
        if(isalpha(curc()))
        {
            string i = read_identifier();
            if(i == "true")
                return Token("bool_literal", "true", line, col);
            else if(i == "false")
                return Token("bool_literal", "false", line, col);
            else if(i == "var")
                return Token("var", i, line, col);
            else if(i == "ref")
                return Token("ref", i, line, col);
            else if(i == "extern")
                return Token("extern", i, line, col);
            else if(i == "or")
                return Token("or", i, line, col);
            else if(i == "and")
                return Token("and", i, line, col);
            else if(i == "not")
                return Token("not", i, line, col);
            else if(i == "if")
                return Token("if", i, line, col);
            else if(i == "else")
                return Token("else", i, line, col);
            else if(i == "return")
                return Token("return", i, line, col);
            else if(i == "while")
                return Token("while", i, line, col);
            else if(i == "break")
                return Token("break", i, line, col);
            else if(i == "continue")
                return Token("continue", i, line, col);
            else if(i == "for")
                return Token("for", i, line, col);
            else if(i == "in")
                return Token("in", i, line, col);
            else
                return Token("identifier", i, line, col);
        }
        if(isdigit(curc()))
        {
            numeric_const n = read_numeric_const();
            if(n.type == "float")
                return Token("float_literal", n.val, line, col);
            else if(n.type == "int")
                return Token("int_literal", n.val, line, col);
            else
                die("Strange looking number.");
        }
        string msg = "Unexpected character ";
        msg += curc();
        die(msg);
    }
    return Token("eof", "", line, col);
}

void Lexer::next()
{
    text_pos++;
    if(text[text_pos] > 127)
        die("Invalid character");
    if(text[text_pos] == '\0')
        die("Unexpected NULL character");
    if(curc() == '\n')
    {
        col = 0;
        line++;
    }
    else
        col++;
}

void Lexer::block_comment()
{
    while(text_pos < (text.length()-1))
    {
        next();
        if(curc() == '*')
        {
            next();
            if(curc() == '/')
            {
                next();
                return;
            }
        }
    }
}

string Lexer::read_string()
{
    string s = "";
    while(text_pos < (text.length()-1))
    {
        next();
        if(curc() == '\\') //reached an escape character
        {
            next();
            if(curc() == 'b')
                s += "\b";
            else if(curc() == 'f')
                s += "\f";
            else if(curc() == 'n')
                s += "\n";
            else if(curc() == 'r')
                s += "\r";
            else if(curc() == 't')
                s += "\t";
            else if(curc() == '\\')
                s += "\\";
            else if(curc() == '"')
                s += "\"";
            else if(curc() == '0')
                s += "\0";
            else if(curc() == 'x')
            {
                next();
                string h = "0x";
                h += curc();
                next();
                h += curc();
                s += std::to_string(std::stoul(h, nullptr, 16));
            }
            else
                die("Unknown escape sequence.");
        }
        else if(curc() == '"')
        {
            next();
            return s;
        }
        else
            s += curc();
    }
    die("String reaches end of file.");
    return "";
}


numeric_const Lexer::read_numeric_const()
{
    numeric_const nc;
    nc.val += curc();
    nc.type = "int";
    while(text_pos < (text.length()-1))
    {
        next();
        if(!isdigit(curc()))
        {
            if(curc() == '.')
            {
                if(nc.type == "float")
                    die("Multiple decimal marks found in floating point literal.");
                nc.type = "float";
            }
            else if(curc() == '\'')
            {
                //ignore
            }
            else
            {
                if(nc.type != "float")
                    nc.type = "int";
                return nc;
            }
        }
        nc.val += curc();
    }
    text_pos++;
    return nc;
}

void Lexer::line_comment()
{
    while(text_pos < (text.length()-1))
    {
        next();
        if(curc() == '\n')
        {
            next();
            return;
        }
    }
}

string Lexer::read_identifier()
{
    string s = "";
    s += curc();
    while(text_pos < (text.length()-1))
    {
        next();
        if(!(isalpha(curc())) && (!isdigit(curc())) && (curc() != '_'))
            return s;
        else
            s += curc();
    }
    die("Unexpected end of file.");
    return s;
}

void Lexer::die(string msg)
{
    stringstream ss;
    ss << "Line " << line << " Col " << col << " : " << msg << "\n";
    ss.flush();
    throw(LexerError(ss.str()));
}
