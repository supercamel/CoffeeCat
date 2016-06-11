#ifndef GENERATOR_H
#define GENERATOR_H


#include "walker.h"
#include "nodes.h"


struct CompilerError
{
    CompilerError(Token tok, string m)
    {
        msg = "Compiler error Line: ";
        msg += to_string(tok.line);
        msg += " Col: ";
        msg += to_string(tok.col);
        msg += " " + m;
    }
    string msg;
};

struct CofVariable
{
    string ident;
    string type;
    int depth = 0;
    bool copyable = false;
    bool atomic = false;
};


struct CofMethod
{
    string ident;
    string r_type; //return type
    vector<CofVariable> params;
};

struct CofObject
{
    string ident;
    vector<CofMethod> methods;
    vector<CofVariable> vars;
};



class Generator : public TreeWalker
{
public:
    Generator();

    void generate(NBlock* block, string fname);

    void Visit(NBlock* block);
    void Visit(NBinaryOperator* bo);
    void Visit(NIntegerLiteral* i);
    void Visit(NCharLiteral* c);
    void Visit(NFloatLiteral* f);
    void Visit(NBoolLiteral* b);
    void Visit(NIdentifier* i);
    void Visit(NUnaryOperator* uo);
    void Visit(NExpressionList* o);
    void Visit(NMethodCall* o);
    void Visit(NArgumentList* o);
    void Visit(NParameterDeclaration* pd);
    void Visit(NMethod* m);
    void Visit(NVariableDeclaration* v);
    void Visit(NString* s);
    void Visit(NExtern* e);
    void Visit(NBrackets* b);
    void Visit(NIfElse* ie);
    void Visit(NReturn* r);
    void Visit(NWhile* w);
    void Visit(NFor* f);
    void Visit(NControl* c);
    void Visit(NDot* d);
    void Visit(NClass* c);
    void Visit(NEnum* e);
    void Visit(NShared* s);

    string header;
    string source;

private:
    int block_depth = 0;
    bool lhs = true;

    vector<string> in_class;

    void print_block_depth();

    bool generate_auto = true; //if true, prints out auto instead of full type name
    bool generate_decl = false;
    bool var_found = false;
};


#endif // GENERATOR_H
