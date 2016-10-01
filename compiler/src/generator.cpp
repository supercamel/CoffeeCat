#include "generator.h"
#include "parser.h"
#include <algorithm>

Generator::Generator()
{

}


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
    header += "#include \"stdlib/print.h\"\n#include \"stdlib/enum.h\"\n#include <etk/etk.h>\nusing namespace etk;\n";

    source = "#include \"coffee_header.h\"\n\n";

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

void Generator::Visit(NInclude* i)
{
	header += "#include \"" + i->path + ".h\"\n";
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

    if(BO_ASSIGN)
        lhs = false;
    (*it)->Accept(this);
    if(BO_ASSIGN)
        lhs = false;
}

void Generator::Visit(NIntegerLiteral* i)
{
    source += to_string(i->value);
}

void Generator::Visit(NCharLiteral* i)
{
    source += "'";
    source += i->value;
    source += "'";
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
    source += "(";

    int count = 0;
    for(auto i : o->args)
    {
        i->Accept(this);
        count++;
        if(count < o->args.size())
            source += ", ";
    }

    source += ")\n";
}

void Generator::Visit(NParameterDeclaration* pd)
{
    string s;
    if(pd->output)
    {
        s = pd->type;
        if(s == "auto")
            var_found = true;
        s += "& ";
        s += pd->handle;
    }
    else if(pd->clone)
    {
        s = pd->type;
        if(s == "auto")
            var_found = true;
        s += " ";
        s += pd->handle;
    }
    else
    {
        s = "const ";
        s += pd->type;
        if(s == "const auto")
            var_found = true;
        s += "& ";
        s += pd->handle;
    }

    source += s;
}

void Generator::Visit(NEnum* e)
{
    header += "class ";
    header += e->handle;
    header += " : public __Enum\n{\n\tpublic:\n\t\t";

    header += e->handle;
    header += "() { }\n\t\t";

    header += e->handle;
    header += "(const int q) { value = q; }\n\t\t";

    auto s = source;
    source = "";

    in_class.push_back(e->handle);

    auto h = header;
    /*
    generate_decl = true;

    for(auto m : e->methods)
    {
        //source += "virtual ";
        m->Accept(this);
    }

    generate_decl = false;
    */
    header = h;

    for(auto m : e->methods)
        m->Accept(this);

    in_class.pop_back();

    uint32_t count = 0;
    for(auto& n : e->numbers)
    {
        header += "\t\tstatic const int ";
        header += n;
        header += " = ";
        header += to_string(count++);
        header += ";\n";
    }

	header += source;
    source = "";
    source = s;
    
    header += "};\n";
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

    in_class.push_back(c->handle);

    for(auto cl : c->subclasses)
        cl->Accept(this);

    auto h = header;
    /*
    generate_decl = true;

    for(auto m : c->methods)
    {
        //source += "virtual ";
        m->Accept(this);
    }

    generate_decl = false;
*/
    generate_auto = false;
    for(auto v : c->vars)
    {
        source += "\t";
        v->Accept(this);
        source += ";\n";
    }
    generate_auto = true;

    header = h;

    for(auto m : c->methods)
        m->Accept(this);

    in_class.pop_back();
    
    header += source;
    source = "";
    source = s;

    header += "};\n";
}

void Generator::Visit(NMethod* m)
{
    if(generate_decl)
    {
        var_found = false;
        auto s = source;
        auto h = header;
        m->args->Accept(this);
        source = s;
        header = h;

        string dec;
        if(in_class.size())
        {
            if((m->foo_name[0] != '~') && (m->foo_name != in_class.back()))
            {
                if(!var_found)
                    dec += "virtual ";
               dec += m->return_type;
            }
        }
        else
            dec = m->return_type;
        dec += " ";
        dec += m->foo_name;

        source += dec;
        m->args->Accept(this);
        source += ";\n";
    }
    else
    {
        string dec;
        if(in_class.size())
        {
            if((m->foo_name[0] != '~') && (m->foo_name != in_class.back()))
               dec = m->return_type;
        }
        else
            dec = m->return_type;
        dec += " ";
        /*
        for(auto& s : in_class)
            dec += s + "::";
        */
        dec += m->foo_name;

        if(!in_class.size())
            header += dec;

        source += dec;
        auto s = source;
        source = "";

        m->args->Accept(this);
        if(!in_class.size())
        {
            header += source;
            header += ";\n";
        }

        m->block.Accept(this);

        source = s + source + "\n";
    }
}

void Generator::Visit(NVariableDeclaration* v)
{
    if(v->global)
    {
        header += "extern ";
        header += v->type + " ";
        header += v->handle + ";\n";

        source += v->type;
    }
    else
    {
        string type = v->type;
        if(generate_auto)
        {
            if(!Parser::variable_is_atomic(type))
                type = "auto";
        }
        source += type;
    }

    source += " ";
    source += v->handle;
    source += " = ";

    v->initialiser->Accept(this);

}

void Generator::Visit(NString* s)
{
    source += "\"";
    source += s->str;
    source += "\"";
}

void Generator::Visit(NExtern* e)
{
    if(e->header)
    {
        header += e->code;
        header += "\n";
    }
    else
        source += e->code;
}

void Generator::Visit(NBrackets* b)
{
    source += "(";
    if((*b->subexpr.begin()))
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
    source += "return";
    if(r->expr)
    {
        source += "(";
        r->expr->Accept(this);
        source += ")";
    }
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
    if(d->ptr)
        source += "->";
    else
        source += ".";
    (*i)->Accept(this);
}

void Generator::Visit(NShared* s)
{
    //make_pool_ptr<Object>(pool);
    source += "make_pool_ptr<";
    source += s->type;
    source += ">(";
    source += s->pool_ident;

    if(s->args)
    {
        auto src = source;
        source = "";
        s->args->Accept(this);
        if(source.length())
        {
            src += ", ";
            src += source;
        }
        source = src;
    }

    source += ")";
}

void Generator::print_block_depth()
{
    for(int i = 0; i < block_depth-1; i++)
        source += "    ";
}
