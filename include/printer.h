#ifndef PRINTER_H
#define PRINTER_H

#include "walker.h"
#include "nodes.h"

class Printer : public TreeWalker
{
public:
    void Visit(NBlock* block);
    void Visit(NBinaryOperator* bo);
    void Visit(NIntegerLiteral* i);
    void Visit(NIdentifier* i);
    void Visit(NString* s);
    void Visit(NUnaryOperator* uo);
    void Visit(NExpressionList* o);
    void Visit(NMethodCall* o);
    void Visit(NArgumentList* o);
    void Visit(NParameterDeclaration* pd);
    void Visit(NMethod* m);
    void Visit(NVariableDeclaration* v);
    void Visit(NAtomicVariableDeclaration* a);
    void Visit(NExtern* e);

private:
    void print_block();
    int block_depth = 0;
};

#endif // PRINTER_H
