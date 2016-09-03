#ifndef NODES_H
#define NODES_H

#include <vector>
#include <string>
#include <memory>
#include "walker.h"
#include "lexer.h"

using namespace std;


enum BINARY_OPERATOR
{
    BO_MULTIPLY,
    BO_DIVIDE,
    BO_ADD,
    BO_SUBTRACT,
    BO_MODULUS,
    BO_LEFT_SHIFT,
    BO_RIGHT_SHIFT,
    BO_ASSIGN,
    BO_MUL_ASSIGN,
    BO_DIV_ASSIGN,
    BO_MOD_ASSIGN,
    BO_ADD_ASSIGN,
    BO_SUB_ASSIGN,
    BO_LEFT_SHIFT_ASSIGN,
    BO_RIGHT_SHIFT_ASSIGN,
    BO_AND_ASSIGN,
    BO_XOR_ASSIGN,
    BO_OR_ASSIGN,
    BO_LOGICAL_OR,
    BO_LOGICAL_AND,
    BO_INCLUSIVE_OR,
    BO_EXCLUSIVE_OR,
    BO_BITWISE_AND,
    BO_EQUAL,
    BO_NOT_EQUAL,
    BO_LESS_THAN_EQUAL,
    BO_GREATER_THAN_EQUAL,
    BO_LESS_THAN,
    BO_GREATER_THAN
};

enum UNARY_OPERATOR
{
    UO_NEGATE,
    UO_NOT,
    UO_BITWISE_NOT
};

class Node
{
public:
    virtual ~Node() { }
    virtual void Accept(TreeWalker* t) = 0;
    Token token;
    std::string file;
};

class NBlock : public Node
{
public:
    std::vector<shared_ptr<Node>> items;
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NExpression : public Node
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    std::list<shared_ptr<NExpression>> subexpr;
    int precedence = 0;
};


class NStatement : public Node
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};


class NIfElse : public NStatement
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    shared_ptr<NExpression> condition;
    shared_ptr<NBlock> block_if;
    shared_ptr<NBlock> block_else;
};

class NReturn : public NStatement
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    shared_ptr<NExpression> expr;
};


class NWhile : public NStatement
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    shared_ptr<NExpression> condition;
    shared_ptr<NBlock> block;
};

class NFor : public NStatement
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    shared_ptr<NIdentifier> lhs;
    shared_ptr<NExpression> rhs;

    shared_ptr<NBlock> block;
};


class NControl : public NStatement
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string keyword;
};


class NExpressionList : public Node
{
public:
    vector<shared_ptr<NExpression>> items;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NIntegerLiteral : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    int64_t value;
};

class NCharLiteral : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string value;
};


class NFloatLiteral : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    double value;
};

class NBoolLiteral : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    bool value;
};

class NIdentifier : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string ident;
    bool copyable = false;
    bool output = false;
};

class NString : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string str;
};



class NExtern : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string code;
    bool header = false;
};

class NShared : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string type;
    string pool_ident;
    shared_ptr<NExpressionList> args;
};

class NBrackets : public NExpression
{
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};


class NBinaryOperator : public NExpression
{
public:
    BINARY_OPERATOR op;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NUnaryOperator : public NExpression
{
public:
    UNARY_OPERATOR op;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NDot : public NExpression
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    bool ptr = false;
};

class NParameterDeclaration : public Node
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string type;
    string handle;

    bool clone = false;
    bool output = false;
};

class NArgumentList : public Node
{
public:
    vector<shared_ptr<NParameterDeclaration>> args;
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NMethod : public Node
{
public:
    string return_type;
    string foo_name;
    NBlock block;
    shared_ptr<NArgumentList> args;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NVariableDeclaration : public Node
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    bool global = false;
    bool is_auto = false;
    string type;
    string handle;

    shared_ptr<NExpression> initialiser = nullptr;
};


class NMethodCall : public NExpression
{
public:
    string handle;
    shared_ptr<NExpressionList> list = nullptr;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};


class NClass : public Node
{
public:
    string parent;
    string handle;
    vector<shared_ptr<NMethod>> methods;
    vector<shared_ptr<NVariableDeclaration>> vars;
    vector<shared_ptr<NClass>> subclasses;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};


class NEnum : public Node
{
public:
    string handle;
    vector<string> numbers;
    vector<shared_ptr<NMethod>> methods;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};



#endif // NODES_H
