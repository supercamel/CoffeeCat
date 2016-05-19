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
    header += "#include <iostream>\n#include <etk/etk.h>\nusing namespace std;\nusing namespace etk;\n";

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
        auto p = source.end();
        p--;
        if((*p) != '\n')
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
    case BO_EXCLUSIVE_OR:
        source += "^";
        break;
    case BO_BITWISE_AND:
        source += "&";
        break;
    case BO_EQUAL:
        source += "==";
        break;
    case BO_NOT_EQUAL:
        source += "!=";
        break;
    case BO_LESS_THAN_EQUAL:
        source += "<=";
        break;
    case BO_GREATER_THAN_EQUAL:
        source += ">=";
        break;
    case BO_LESS_THAN:
        source += "<";
        break;
    case BO_GREATER_THAN:
        source += ">";
        break;
    case BO_MUL_ASSIGN:
        source += "*=";
        break;
    case BO_DIV_ASSIGN:
        source += "/=";
        break;
    case BO_MOD_ASSIGN:
        source += "%=";
        break;
    case BO_ADD_ASSIGN:
        source += "+=";
        break;
    case BO_SUB_ASSIGN:
        source += "-=";
        break;
    case BO_LEFT_SHIFT_ASSIGN:
        source += "<<=";
        break;
    case BO_RIGHT_SHIFT_ASSIGN:
        source += ">>=";
        break;
    case BO_AND_ASSIGN:
        source += "&=";
        break;
    case BO_XOR_ASSIGN:
        source += "^=";
        break;
    case BO_OR_ASSIGN:
        source += "|=";
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
    case UO_BITWISE_NOT:
    {
        source += "~";
        (*uo->subexpr.begin())->Accept(this);
    }
    break;
    case UO_NOT:
    {
        source += "!";
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
    s += pd->handle;

    header += s;
    source += s;
}

void Generator::Visit(NClass* c)
{
    header += "class ";
    header += c->handle;
    if(c->parent.length())
        header += " : public " + c->parent;
    header += "\n{\npublic:\n";

    auto s = source;
    source = "";

    for(auto cl : c->subclasses)
        cl->Accept(this);

    auto h = header;
    for(auto m : c->methods)
        m->Accept(this);
    header = h;


    header += source;
    source = "";
    source = s;
    header += "};\n";
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

void Generator::Visit(NObjVariableDeclaration* v)
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

void Generator::Visit(NBrackets* b)
{
    source += "(";
    (*b->subexpr.begin())->Accept(this);
    source += ")";
}

void Generator::Visit(NIfElse* ie)
{
    source += "if";
    ie->condition->Accept(this);
    source += "\n";
    ie->block_if->Accept(this);
    if(ie->block_else)
    {
        print_block_depth();
        source += "else\n";
        ie->block_else->Accept(this);
    }
}

void Generator::Visit(NReturn* r)
{
    source += "return(";
    if(r->expr)
        r->expr->Accept(this);
    source += ")";
}

void Generator::Visit(NWhile* w)
{
    source += "while";
    w->condition->Accept(this);
    source += "\n";
    w->block->Accept(this);
}

void Generator::Visit(NFor* f)
{
    source += "for(auto& ";
    f->lhs->Accept(this);
    source += " : ";
    f->rhs->Accept(this);
    source += ")\n";
    f->block->Accept(this);
}

void Generator::Visit(NControl* c)
{
    source += c->keyword;
}

void Generator::Visit(NDot* d)
{
    auto i = d->subexpr.begin();
    (*i++)->Accept(this);
    source += ".";
    (*i)->Accept(this);
}

void Generator::print_block_depth()
{
    for(int i = 0; i < block_depth-1; i++)
        source += "    ";
}
