#ifndef WALKER_H_INCLUDED
#define WALKER_H_INCLUDED

#include <iostream>
#include <memory>
#include <list>

#define unused(expr) (void)(expr)

class TreeWalker
{
public:
	virtual void Visit(class NExpression* o) { unused(o); }
	virtual void Visit(class NFloatLiteral* o) { unused(o); }
	virtual void Visit(class NIntegerLiteral* o) { unused(o); }
	virtual void Visit(class NBoolLiteral* o) { unused(o); }
	virtual void Visit(class NIdentifier* o) { unused(o); }
	virtual void Visit(class NString* s) { unused(s); }
	virtual void Visit(class NBinaryOperator* o) { unused(o); }
	virtual void Visit(class NUnaryOperator* o) { unused(o); }
	virtual void Visit(class NBlock* o) { unused(o); }
	virtual void Visit(class NMethod* o) { unused(o); }
	virtual void Visit(class NVariableDeclaration* o) { unused(o); }
	virtual void Visit(class NAtomicVariableDeclaration* o) { unused(o); }
	virtual void Visit(class NExpressionList* o) { unused(o); }
	virtual void Visit(class NMethodCall* o) { unused(o); }
	virtual void Visit(class NParameterDeclaration* o) { unused(o); }
	virtual void Visit(class NArgumentList* o) { unused(o); }
	virtual void Visit(class NExtern* e) { unused(e); }
};


#endif
