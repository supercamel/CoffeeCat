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
    case BO_EXCLUSIVE_OR:
        cout << "EXCLUSIVE OR" << endl;
        break;
    case BO_BITWISE_AND:
        cout << "BITWISE AND" << endl;
        break;
    case BO_EQUAL:
        cout << "EQUAL" << endl;
        break;
    case BO_NOT_EQUAL:
        cout << "NOT EQUAL" << endl;
        break;
    case BO_LESS_THAN_EQUAL:
        cout << "LESS THAN EQUAL" << endl;
        break;
    case BO_GREATER_THAN_EQUAL:
        cout << "GREATER THAN EQUAL" << endl;
        break;
    case BO_LESS_THAN:
        cout << "LESS THAN" << endl;
        break;
    case BO_GREATER_THAN:
        cout << "GREATER THAN" << endl;
        break;
    case BO_MUL_ASSIGN:
        cout << "MULTIPLY ASSIGN" << endl;
        break;
    case BO_DIV_ASSIGN:
        cout << "DIVIDE ASSIGN" << endl;
        break;
    case BO_MOD_ASSIGN:
        cout << "MODULUS ASSIGN" << endl;
        break;
    case BO_ADD_ASSIGN:
        cout << "ADD ASSIGN" << endl;
        break;
    case BO_SUB_ASSIGN:
        cout << "SUB ASSIGN" << endl;
        break;
    case BO_LEFT_SHIFT_ASSIGN:
        cout << "LEFT SHIFT ASSIGN" << endl;
        break;
    case BO_RIGHT_SHIFT_ASSIGN:
        cout << "RIGHT SHIFT ASSIGN" << endl;
        break;
    case BO_AND_ASSIGN:
        cout << "AND ASSIGN" << endl;
        break;
    case BO_XOR_ASSIGN:
        cout << "XOR ASSIGN" << endl;
        break;
    case BO_OR_ASSIGN:
        cout << "OR ASSIGN" << endl;
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
    case UO_NOT:
        cout << "LOGICAL NOT" << endl;
        break;
    case UO_BITWISE_NOT:
        cout << "BITWISE NOT" << endl;
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

void Printer::Visit(NFloatLiteral* f)
{
    print_block();
    cout << "Float literal " << f->value << endl;
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
    cout << pd->handle << endl;
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

void Printer::Visit(NBrackets* b)
{
    print_block();
    cout << "Brackets" << endl;
    block_depth++;
    (*b->subexpr.begin())->Accept(this);
    block_depth--;
}

void Printer::Visit(NClass* c)
{
    print_block();
    cout << "Class " << c->handle << " : " << c->parent << endl;
    block_depth++;
    for(auto cl : c->subclasses)
        cl->Accept(this);
    for(auto m : c->methods)
        m->Accept(this);
    for(auto v : c->vars)
        v->Accept(this);
    block_depth--;
}

void Printer::Visit(NBoolLiteral* b)
{
    print_block();
    if(b->value)
        cout << "true";
    else
        cout << "false";
    cout << endl;
}

void Printer::Visit(NIfElse* ie)
{
    print_block();
    cout << "If Statement" << endl;

    block_depth++;
    print_block();
    cout << "Condition" << endl;
    block_depth++;
    ie->condition->Accept(this);
    block_depth--;
    ie->block_if->Accept(this);
    block_depth--;

    if(ie->block_else)
    {
        print_block();
        cout << "Else" << endl;
        block_depth++;
        ie->block_else->Accept(this);
        block_depth--;
    }
    print_block();
    cout << "End If" << endl;
}

void Printer::Visit(NReturn* r)
{
    print_block();
    cout << "Return " << endl;

    if(r->expr)
    {
        block_depth++;
        r->expr->Accept(this);
        block_depth--;
    }
}

void Printer::Visit(NWhile* w)
{
    print_block();
    cout << "While" << endl;
    block_depth++;
    w->condition->Accept(this);
    w->block->Accept(this);
    block_depth--;
}

void Printer::Visit(NControl* c)
{
    print_block();
    cout << c->keyword << endl;
}

void Printer::Visit(NFor* f)
{
    print_block();
    cout << "For" << endl;
    f->lhs->Accept(this);
    f->rhs->Accept(this);
    f->block->Accept(this);
}

void Printer::Visit(NDot* d)
{
    auto i = d->subexpr.begin();
    (*i++)->Accept(this);
    block_depth++;
    print_block();
    cout << "dot" << endl;
    (*i)->Accept(this);
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
