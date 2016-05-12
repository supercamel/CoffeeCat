#include "generator.h"
#include <algorithm>

void Generator::generate(NBlock* block, string fname)
{
    header = "#ifndef ";
    auto fname_upper = fname;
    std::transform(fname_upper.begin(), fname_upper.end(), fname_upper.begin(), ::toupper);
    header += fname_upper;
    header += "_H\n";
    header += "#define ";
    header += fname_upper;
    header += "_H\n\n";
    header += "#include <iostream>\n#include <etk/etk.h>\nusing namespace std;\n\n";

    source = "#include \"";
    source += fname;
    source += ".h\"\n\n";

    block->Accept(this);

    header += "\n#endif\n";
}

void Generator::Visit(NBlock* block)
{

    if(block_depth)
    {
        print_block_depth();
        source += "{\n";
    }

    block_depth++;
    for(auto i : block->items)
    {
        print_block_depth();
        i->Accept(this);
        if(block_depth > 1)
            source += ";\n";
    }
    block_depth--;

    if(block_depth)
    {
        print_block_depth();
        source += "}\n";
    }
}

void Generator::Visit(NBinaryOperator* bo)
{
    auto it = bo->subexpr.begin();

    (*it++)->Accept(this);

    switch(bo->op)
    {
    case BO_ADD:
        source += "+";
        break;
    case BO_SUBTRACT:
        source += "-";
        break;
    case BO_MULTIPLY:
        source += "*";
        break;
    case BO_DIVIDE:
        source += "/";
        break;
    case BO_MODULUS:
        source += "%";
        break;
    case BO_LEFT_SHIFT:
        source += " << ";
        break;
    case BO_RIGHT_SHIFT:
        source += " >> ";
        break;
    case BO_ASSIGN:
        source += " = ";
        break;
    case BO_LOGICAL_OR:
        source += " or ";
        break;
    case BO_LOGICAL_AND:
        source += " and ";
        break;
    case BO_INCLUSIVE_OR:
        source += "|";
        break;
    }

    (*it)->Accept(this);
}

void Generator::Visit(NIntegerLiteral* i)
{
    source += to_string(i->value);
}

void Generator::Visit(NFloatLiteral* f)
{
    source += to_string(f->value);
}

void Generator::Visit(NBoolLiteral* b)
{
    if(b->value)
        source += "true";
    else
        source += "false";
}

void Generator::Visit(NIdentifier* i)
{
    source += i->ident;
}

void Generator::Visit(NUnaryOperator* uo)
{
    switch(uo->op)
    {
    case UO_NEGATE:
    {
        source += "-";
        (*uo->subexpr.begin())->Accept(this);
    }
    break;
    }
}

void Generator::Visit(NExpressionList* o)
{
    int length = o->items.size();
    int count = 0;
    for(auto i : o->items)
    {
        i->Accept(this);
        count++;
        if(count < length)
            source += ", ";
    }
}

void Generator::Visit(NMethodCall* o)
{
    source += o->handle;
    source += "(";
    o->list->Accept(this);
    source += ")";
}

void Generator::Visit(NArgumentList* o)
{
    header += "(";
    source += "(";

    int count = 0;
    for(auto i : o->args)
    {
        i->Accept(this);
        count++;
        if(count < o->args.size())
        {
            header += ", ";
            source += ", ";
        }
    }

    header += ");\n";
    source += ")\n";
}

void Generator::Visit(NParameterDeclaration* pd)
{
    string s = pd->type;
    s += " ";
    if(pd->refer)
        s += "&";
    s += pd->handle;

    header += s;
    source += s;
}

void Generator::Visit(NMethod* m)
{
    string dec = m->return_type;
    dec += " ";
    dec += m->foo_name;

    header += dec;
    source += dec;

    m->args->Accept(this);
    m->block.Accept(this);

    source += "\n";
}

void Generator::Visit(NVariableDeclaration* v)
{
    source += "auto ";
    source += v->handle;
    source += " = ";
    source += v->type;
    source += "(";
    v->list->Accept(this);
    source += ")";
}

void Generator::Visit(NString* s)
{
    source += "\"";
    source += s->str;
    source += "\"";
}

void Generator::Visit(NAtomicVariableDeclaration* a)
{
    source += a->type;
    source += " ";
    source += a->handle;
    source += " = ";
    a->initialiser->Accept(this);
}

void Generator::Visit(NExtern* e)
{
    source += e->code;
}

void Generator::print_block_depth()
{
    for(int i = 0; i < block_depth-1; i++)
        source += "    ";
}
