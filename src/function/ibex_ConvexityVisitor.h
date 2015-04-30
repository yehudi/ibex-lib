/*
 * ibex_ConvexityVisitor.h
 *
 *  Created on: Apr 29, 2015
 *      Author: Yehudi Hollevoet
 */

#ifndef SRC_FUNCTION_IBEX_CONVEXITYVISITOR_H_
#define SRC_FUNCTION_IBEX_CONVEXITYVISITOR_H_

#include "ibex_Expr2Polynomial.h"


using namespace std;

namespace ibex{


class ConvexityVisitor : public ExprVisitor {
protected:
	ExprProperty prop;
	std::stack<ExprProperty> stack;
public:
	ConvexityVisitor(const ExprNode&);

	ExprProperty property() const;

	virtual void visit(const ExprNode&);
	virtual void visit(const ExprIndex&);
	virtual void visit(const ExprLeaf&);
	virtual void visit(const ExprNAryOp&);
	virtual void visit(const ExprBinaryOp&);
	virtual void visit(const ExprUnaryOp& e);
	virtual void visit(const ExprAdd& e);
	virtual void visit(const ExprMinus& e);
	virtual void visit(const ExprSub& e);
	virtual void visit(const ExprMul&);
	virtual void visit(const ExprPower&);
	virtual void visit(const ExprConstant&);
	virtual void visit(const ExprSymbol&);
	virtual void visit(const ExprSqr&);
	virtual void visit(const ExprExp&);
	virtual void visit(const ExprLog&);
	virtual void visit(const ExprMax&);

	virtual ~ConvexityVisitor();

};


}

#endif /* SRC_FUNCTION_IBEX_CONVEXITYVISITOR_H_ */
