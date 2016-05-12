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
    BO_LOGICAL_OR,
    BO_LOGICAL_AND,
    BO_INCLUSIVE_OR
};

enum UNARY_OPERATOR
{
    UO_NEGATE
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


class NParameterDeclaration : public Node
{
public:
    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }

    string type;
    string handle;
    bool refer = false;
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

class NAtomicVariableDeclaration : public Node
{
public:
    string type;
    string handle;

    shared_ptr<NExpression> initialiser = nullptr;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
};

class NVariableDeclaration : public Node
{
public:
    string type;
    string handle;

    shared_ptr<NExpressionList> list = nullptr;

    virtual void Accept(TreeWalker* t)
    {
        t->Visit(this);
    }
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


#endif // NODES_H
