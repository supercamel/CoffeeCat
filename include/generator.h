#ifndef GENERATOR_H
#define GENERATOR_H


#include "walker.h"
#include "nodes.h"

class Generator : public TreeWalker
{
public:
    void generate(NBlock* block, string fname);

    void Visit(NBlock* block);
    void Visit(NBinaryOperator* bo);
    void Visit(NIntegerLiteral* i);
    void Visit(NFloatLiteral* f);
    void Visit(NBoolLiteral* b);
    void Visit(NIdentifier* i);
    void Visit(NUnaryOperator* uo);
    void Visit(NExpressionList* o);
    void Visit(NMethodCall* o);
    void Visit(NArgumentList* o);
    void Visit(NParameterDeclaration* pd);
    void Visit(NMethod* m);
    void Visit(NObjVariableDeclaration* v);
    void Visit(NString* s);
    void Visit(NAtomicVariableDeclaration* a);
    void Visit(NExtern* e);
    void Visit(NBrackets* b);
    void Visit(NIfElse* ie);
    void Visit(NReturn* r);
    void Visit(NWhile* w);
    void Visit(NFor* f);
    void Visit(NControl* c);
    void Visit(NDot* d);

    string header;
    string source;

private:
    int block_depth = 0;

    void print_block_depth();
};


#endif // GENERATOR_H
