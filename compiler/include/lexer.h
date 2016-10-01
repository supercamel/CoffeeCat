#ifndef LEXER_H
#define LEXER_H

//#include <etk/etk.h>
#include <string>
#include <iostream>
using namespace std;

struct LexerError
{
    LexerError(string msg) : msg(msg) { }
    string msg;
};

enum TOKEN
{
    TOK_INDENT,
    TOK_NEWLINE,
    TOK_STRING_LITERAL,
    TOK_CHAR_LITERAL,
    TOK_MUL_ASSIGN,
    TOK_MUL,
    TOK_DIV_ASSIGN,
    TOK_DIV,
    TOK_ADD_ASSIGN,
    TOK_ADD,
    TOK_POINTER_ACCESS,
    TOK_SUB_ASSIGN,
    TOK_SUB,
    TOK_MOD_ASSIGN,
    TOK_MOD,
    TOK_LH_BRACKET,
    TOK_RH_BRACKET,
    TOK_COLON,
    TOK_DOUBLE_COLON,
    TOK_EQUAL,
    TOK_ASSIGN,
    TOK_NOT_EQUAL,
    TOK_COMMA,
    TOK_SHIFT_LEFT,
    TOK_SHIFT_LEFT_ASSIGN,
    TOK_LESS_THAN_EQUAL,
    TOK_LESS_THAN,
    TOK_SHIFT_RIGHT,
    TOK_SHIFT_RIGHT_ASSIGN,
    TOK_GREATER_THAN_EQUAL,
    TOK_GREATER_THAN,
    TOK_DOT,
    TOK_BAR,
    TOK_BAR_ASSIGN,
    TOK_CARET,
    TOK_CARET_ASSIGN,
    TOK_AMP,
    TOK_AMP_ASSIGN,
    TOK_TILDE,
    TOK_BOOL_LITERAL,
    TOK_VAR,
    TOK_EXTERN,
    TOK_EXTERN_HEADER,
    TOK_ENUM,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_IF,
    TOK_ELSE,
    TOK_RETURN,
    TOK_WHILE,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_FOR,
    TOK_IN,
    TOK_GLOBAL,
    TOK_OUT,
    TOK_SHARED,
    TOK_COPY,
    TOK_CLASS,
    TOK_PASS,
    TOK_IDENTIFIER,
    TOK_FLOAT_LITERAL,
    TOK_INT_LITERAL,
    TOK_INCLUDE,
    TOK_EOF
};

struct Token
{

    Token(TOKEN tok, string raw, int line, int col) :
        tok(tok), raw(raw), line(line), col(col)
    { }

    Token(TOKEN tok,  int line, int col) :
        tok(tok), line(line), col(col)
    { }



    Token() { }

    TOKEN tok;
    string raw;
    int line = 0;
    int col = 0;
};

struct numeric_const
{
    bool is_float = false;
    string val = "";
};


class Lexer
{
public:
    Lexer(string& code);
    virtual ~Lexer();

    Token lex(bool peeking = false, int peek_ahead = 0);
private:
    inline char curc() {
        return text[text_pos];
    }
    Token get_token();
    void next();
    void block_comment();
    void line_comment();
    string read_string();
    string read_char();
    numeric_const read_numeric_const();
    string read_identifier();

    void die(string msg);

    string text;
    int text_pos;
    int col, line;
};

#endif // LEXER_H
