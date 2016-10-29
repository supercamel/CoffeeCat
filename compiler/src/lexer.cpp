#include "lexer.h"
#include <sstream>
#include <iostream>
#include <cctype>
#include <vector>

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

    restart:
    while(text_pos < text.length()-1)
    {
        switch(curc())
        {
        case '\r':
        {
            next();
            goto restart;
        }
        case '\t':
        {
            next();
            return Token(TOK_INDENT, "    ", line, col);
        }
        break;
        case '\n':
        {
            next();
            return Token(TOK_NEWLINE, "\n", line, col);
        }
        case ' ':
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
                        return Token(TOK_INDENT, "    ", line, col);
                    }
                }
            }
            goto restart;
        }
        break;
        }



        raw_tok += curc();

        switch(curc())
        {
        case '"':
        {

            string s = read_string();
            return Token(TOK_STRING_LITERAL, s, line, col);
        }
        case '\'':
        {
            string c = read_char();
            return Token(TOK_CHAR_LITERAL, c, line, col);
        }
        case '*':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_MUL_ASSIGN, line, col);
            }
            return Token(TOK_MUL, line, col);
        }
        case '/':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_DIV_ASSIGN, line, col);
            }
            if(curc() == '*')
            {
                block_comment();
                goto restart;
            }
            if(curc() == '/')
            {
                line_comment();
                goto restart;
            }
            return Token(TOK_DIV, line, col);
        }
        case '+':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_ADD_ASSIGN, line, col);
            }
            return Token(TOK_ADD, line, col);
        }
        case '-':
        {
            next();
            if(curc() == '>')
            {
                next();
                return Token(TOK_POINTER_ACCESS, line, col);
            }
            if(curc() == '=')
            {
                next();
                return Token(TOK_SUB_ASSIGN, line, col);
            }
            return Token(TOK_SUB, line, col);
        }
        case '(':
        {
            next();
            return Token(TOK_LH_BRACKET, line, col);
        }
        case ')':
        {
            next();
            return Token(TOK_RH_BRACKET, line, col);
        }
        case ':':
        {
            next();
            return Token(TOK_COLON, line, col);
        }
        case '%':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_MOD_ASSIGN, line, col);
            }
            return Token(TOK_MOD, line, col);
        }
        case '=':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_EQUAL, line, col);
            }
            return Token(TOK_ASSIGN, line, col);
        }
        case '!':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_NOT_EQUAL, line, col);
            }
            die("Unexpected character");
        }
        case  ',':
        {
            next();
            return Token(TOK_COMMA, line, col);
        }
        case '<':
        {
            next();
            if(curc() == '<')
            {
                next();
                if(curc() == '=')
                {
                    next();
                    return Token(TOK_SHIFT_LEFT_ASSIGN, line, col);
                }
                return Token(TOK_SHIFT_LEFT, line, col);
            }
            if(curc() == '=')
            {
                next();
                return Token(TOK_LESS_THAN_EQUAL, line, col);
            }
            return Token(TOK_LESS_THAN, line, col);
        }
        case '>':
        {
            next();
            if(curc() == '>')
            {
                next();
                if(curc() == '=')
                {
                    next();
                    return Token(TOK_SHIFT_RIGHT_ASSIGN, line, col);
                }
                return Token(TOK_SHIFT_RIGHT, line, col);
            }
            if(curc() == '=')
            {
                next();
                return Token(TOK_GREATER_THAN_EQUAL, line, col);
            }
            return Token(TOK_GREATER_THAN, line, col);
        }
        case '.':
        {
            next();
            return Token(TOK_DOT, line, col);
        }
        case '|':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_BAR_ASSIGN, line, col);
            }
            return Token(TOK_BAR, line, col);
        }
        case '^':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_CARET_ASSIGN, line, col);
            }
            return Token(TOK_CARET, line, col);
        }
        case '&':
        {
            next();
            if(curc() == '=')
            {
                next();
                return Token(TOK_AMP_ASSIGN,  line, col);
            }
            return Token(TOK_AMP, line, col);
        }
        case '~':
        {
            next();
            return Token(TOK_TILDE, line, col);
        }
        }

        if(isalpha(curc()))
        {
            string i = read_identifier();
            if(i == "true")
                return Token(TOK_BOOL_LITERAL, "true", line, col);
            else if(i == "false")
                return Token(TOK_BOOL_LITERAL, "false", line, col);
            else if(i == "var")
                return Token(TOK_VAR , line, col);
            else if(i == "extern")
                return Token(TOK_EXTERN, line, col);
            else if(i == "enum")
                return Token(TOK_ENUM, line, col);
            else if(i == "extern_header")
                return Token(TOK_EXTERN_HEADER, line, col);
            else if(i == "or")
                return Token(TOK_OR, line, col);
            else if(i == "and")
                return Token(TOK_AND, line, col);
            else if(i == "not")
                return Token(TOK_NOT, line, col);
            else if(i == "if")
                return Token(TOK_IF, line, col);
            else if(i == "else")
                return Token(TOK_ELSE, line, col);
            else if(i == "return")
                return Token(TOK_RETURN, line, col);
            else if(i == "while")
                return Token(TOK_WHILE, line, col);
            else if(i == "break")
                return Token(TOK_BREAK, line, col);
            else if(i == "continue")
                return Token(TOK_CONTINUE, line, col);
            else if(i == "for")
                return Token(TOK_FOR, line, col);
            else if(i == "in")
                return Token(TOK_IN, line, col);
            else if(i == "global")
                return Token(TOK_GLOBAL, line, col);
            else if(i == "out")
                return Token(TOK_OUT, line, col);
            else if(i == "shared")
                return Token(TOK_SHARED, line, col);
            else if(i == "copy")
                return Token(TOK_COPY, line, col);
            else if(i == "class")
                return Token(TOK_CLASS, line, col);
            else if(i == "pass")
                return Token(TOK_PASS, line, col);
            else if(i == "include")
            	return Token(TOK_INCLUDE, line, col);
            else if(i == "volatile")
            	return Token(TOK_VOLATILE, line, col);
            else
                return Token(TOK_IDENTIFIER, i, line, col);
        }
        if(isdigit(curc()))
        {
            numeric_const n = read_numeric_const();
            if(n.is_float)
                return Token(TOK_FLOAT_LITERAL, n.val, line, col);
            else
                return Token(TOK_INT_LITERAL, n.val, line, col);
        }
        string msg = "Unexpected character ";
        msg += curc();
        die(msg);
    }
    return Token(TOK_EOF, "", line, col);
}

void Lexer::next()
{
    text_pos++;
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
        char c = curc();
        if(c == '\\') //reached an escape character
        {
            next();
            c = curc();
            if(c == '"')
                s += "\"";
            else
            {
                s += "\\";
                s += c;
            }

        }
        else if(c == '"')
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

string Lexer::read_char()
{
    string s = "";
    while(text_pos < (text.length()-1))
    {
        next();
        if(curc() == '\'')
        {
            next();
            return s;
        }
        else
            s += curc();
    }
    die("Character declaration reaches end of file.");
    return "";
}

numeric_const Lexer::read_numeric_const()
{
    numeric_const nc;
    nc.val += curc();
    nc.is_float = false;
    
    std::vector<char> allowed_chars;
    
    if((curc() == '0') && (text[text_pos+1] == 'x'))
    {
    	next();
    	nc.val = "0x";
    	allowed_chars = {'A', 'B', 'C', 'D', 'E', 'F'};
    }
    else 
    	allowed_chars = {'.'};
    
    while(text_pos < (text.length()-1))
    {
        next();
        if(!isdigit(curc()))
        {
        	bool is_ok = false;
            for(auto c : allowed_chars)
            {
            	if(curc() == c)
            	{
            		is_ok = true;
            		break;
            	}
            }
            if((curc() == '\'') || (is_ok))
            {
            	//igore ' characters
            }
            else
            {
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
    char s[128];
    s[0] = curc();
    int count = 0;
    while(text_pos < (text.length()-1))
    {
        next();
        if(!(isalpha(curc())) && (!isdigit(curc())) && (curc() != '_'))
        {
            s[++count] = '\0';
            return s;
        }
        s[++count] = curc();
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
