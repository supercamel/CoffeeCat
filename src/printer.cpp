#include "printer.h"
#include "nodes.h"

using namespace std;


void Printer::Visit(NBlock* o)
{
    print_block();
    cout << "Block" << endl;
    block_depth++;

    for(auto i : o->items)
        i->Accept(this);

    block_depth--;
}

void Printer::Visit(NBinaryOperator* bo)
{
    print_block();
    cout << "Binary operator ";
    switch(bo->op)
    {
    case BO_MULTIPLY:
        cout << "MULTIPLY" << endl;
    break;
    case BO_ADD:
        cout << "ADD" << endl;
    break;
    case BO_DIVIDE:
        cout << "DIVIDE" << endl;
    break;
    case BO_MODULUS:
        cout << "MODULUS" << endl;
    break;
    case BO_SUBTRACT:
        cout << "SUBTRACT" << endl;
    break;
    case BO_LEFT_SHIFT:
        cout << "LEFT SHIFT" << endl;
    break;
    case BO_RIGHT_SHIFT:
        cout << "RIGHT SHIFT" << endl;
    break;
    case BO_ASSIGN:
        cout << "ASSIGN" << endl;
        break;
    case BO_LOGICAL_OR:
        cout << "LOGICAL OR" << endl;
        break;
    case BO_LOGICAL_AND:
        cout << "LOGICAL AND" << endl;
        break;
    case BO_INCLUSIVE_OR:
        cout << "INCLUSIVE OR" << endl;
        break;
    }

    block_depth++;
    for(auto e : bo->subexpr)
        e->Accept(this);
    block_depth--;
}

void Printer::Visit(NUnaryOperator* bo)
{
    print_block();
    cout << "Unary operator ";
    switch(bo->op)
    {
    case UO_NEGATE:
        cout << "NEGATE" << endl;
    break;
    };

    block_depth++;
    for(auto e : bo->subexpr)
        e->Accept(this);
    block_depth--;
}

void Printer::Visit(NIntegerLiteral* i)
{
    print_block();
    cout << "Int literal " << i->value << endl;
}

void Printer::Visit(NIdentifier* i)
{
    print_block();
    cout << "Identifier " << i->ident << endl;
}

void Printer::Visit(NString* s)
{
    print_block();
    cout << "String " << s->str << endl;
}

void Printer::Visit(NExpressionList* e)
{
    print_block();
    cout << "Expression List" << endl;
    block_depth++;
    for(auto exp : e->items)
        exp->Accept(this);

    block_depth--;
}

void Printer::Visit(NMethodCall* m)
{
    print_block();
    cout << "Method call " << m->handle << endl;
    block_depth++;
    m->list->Accept(this);
    block_depth--;
}

void Printer::Visit(NVariableDeclaration* v)
{
    print_block();
    cout << "Variable declaration " << v->type << " " << v->handle << endl;
    print_block();
    cout << "Args" << endl;
    block_depth++;
    v->list->Accept(this);
    block_depth--;
}

void Printer::Visit(NArgumentList* a)
{
    print_block();
    cout << "Argument list " << endl;
    block_depth++;
    for(auto arg : a->args)
        arg->Accept(this);
    block_depth--;
}

void Printer::Visit(NParameterDeclaration* pd)
{
    print_block();
    cout << "Parameter declaration ";
    cout << pd->type << " ";
    if(pd->refer)
        cout << "refer ";
    cout << pd->handle << endl;
}

void Printer::Visit(NAtomicVariableDeclaration* a)
{
    print_block();
    cout << "Atomic declaration ";
    cout << a->type << " " << a->handle << endl;
    block_depth++;
    a->initialiser->Accept(this);
    block_depth--;
}

void Printer::Visit(NMethod* m)
{
    print_block();
    cout << "Method " << m->return_type << " " << m->foo_name << endl;
    block_depth++;
    m->args->Accept(this);
    m->block.Accept(this);
    block_depth--;
}
void Printer::print_block()
{
    for(int i = 0; i < block_depth; i++)
        cout << "    ";
}

void Printer::Visit(NExtern* e)
{
    print_block();
    cout << "External C++ code" << endl;
    cout << e->code << endl;
}
