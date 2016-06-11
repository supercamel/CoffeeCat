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
	virtual void Visit(class NCharLiteral* c) { unused(c); }
	virtual void Visit(class NBoolLiteral* o) { unused(o); }
	virtual void Visit(class NIdentifier* o) { unused(o); }
	virtual void Visit(class NString* s) { unused(s); }
	virtual void Visit(class NBinaryOperator* o) { unused(o); }
	virtual void Visit(class NUnaryOperator* o) { unused(o); }
	virtual void Visit(class NBlock* o) { unused(o); }
	virtual void Visit(class NMethod* o) { unused(o); }
	virtual void Visit(class NVariableDeclaration* o) { unused(o); }
	virtual void Visit(class NExpressionList* o) { unused(o); }
	virtual void Visit(class NMethodCall* o) { unused(o); }
	virtual void Visit(class NParameterDeclaration* o) { unused(o); }
	virtual void Visit(class NArgumentList* o) { unused(o); }
	virtual void Visit(class NExtern* e) { unused(e); }
	virtual void Visit(class NBrackets* e) { unused(e); }
	virtual void Visit(class NIfElse* ie) { unused(ie); }
	virtual void Visit(class NStatement* s) { unused(s); }
	virtual void Visit(class NReturn*r) { unused(r); }
	virtual void Visit(class NWhile* w) { unused(w); }
	virtual void Visit(class NControl* c) { unused(c); }
	virtual void Visit(class NFor* f) { unused(f); }
	virtual void Visit(class NDot* d) { unused(d); }
	virtual void Visit(class NClass* c) { unused(c); }
	virtual void Visit(class NShared* s) { unused(s); }
	virtual void Visit(class NEnum* e) { unused(e); }
};


#endif
