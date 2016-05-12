#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <iostream>
using namespace std;

struct LexerError
{
    LexerError(string msg) : msg(msg) { }
    string msg;
};

struct Token
{
    Token(string tok, string raw, int line, int col) :
        tok(tok), raw(raw), line(line), col(col)
    { }

    Token() { }

    int lbp()
    {
        if(tok == "+")
            return 10;
        if(tok == "-")
            return 10;
        if(tok == "*")
            return 20;
        if(tok == "/")
            return 20;
        return 0;
    }
    string tok;
    string raw;
    int line = 0;
    int col = 0;
};

struct numeric_const
{
	string type = ""; //float or int
	string val = "";
};


class Lexer
{
public:
    Lexer(string& code);
    virtual ~Lexer();

    Token lex(bool peeking = false, int peek_ahead = 0);
private:
    inline char curc() { return text[text_pos]; }
    Token get_token();
    void next();
    void block_comment();
    void line_comment();
    string read_string();
    numeric_const read_numeric_const();
    string read_identifier();

    void die(string msg);

    string text;
    int text_pos;
    int col, line;
};

#endif // LEXER_H
