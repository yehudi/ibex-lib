/*
 * ibex_ConvexityVisitor.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: Yehudi Hollevoet
 */

#include "ibex_ConvexityVisitor.h"

namespace ibex{
ostream& operator<<(ostream& f, const ExprProperty &e){
	f<<"(";
	switch(e.convexity){
	case CONVEX:
		f<<"convex";
		break;
	case CONCAVE:
		f<<"concave";
		break;
	case NOT_CONVEX_OR_CONCAVE:
		f<<"!convex & !concave";
		break;
	case UNDEFINED_CONVEXITY:
		f<<"undefined";
		break;
	case CONVEX_AND_CONCAVE:
		f<<"convex & concave";
		break;
	case CONVEX_OR_CONCAVE:
		f<<"convex | concave";
		break;
	}
	f<<",";
	switch(e.ordering){
		case INCREASING:
			f<<"increasing";
			break;
		case DECREASING:
			f<<"decreasing";
			break;
		case NOT_INCREASING_OR_DECREASING:
			f<<"!increasing & !decreasing";
			break;
		case UNDEFINED_ORDERING:
			f<<"undefined";
			break;
		case INCREASING_OR_DECREASING:
			f<<"increasing | decreasing";
			break;
		case CONSTANT:
			f<<"constant";
			break;
		}
	f<<")";
	return f;
}


ConvexityVisitor::ConvexityVisitor(const ExprNode& n){
	n.acceptVisitor(*this);
	if(stack.size() > 0){
		prop = stack.top();
		stack.pop();
	}
}

ExprProperty ConvexityVisitor::property() const{
	return prop;
}

void ConvexityVisitor::visit(const ExprNode& e){
	stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
}
void ConvexityVisitor::visit(const ExprIndex& e){
	stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
}
void ConvexityVisitor::visit(const ExprLeaf&e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL
			||  e2p.polynomial()->degree() >0){
		ExprProperty p(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING);
		stack.push(p);
	}else {
		stack.push(ExprProperty());
	}
}
void ConvexityVisitor::visit(const ExprNAryOp&e){
	stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
}
void ConvexityVisitor::visit(const ExprBinaryOp& e){
	stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
}
void ConvexityVisitor::visit(const ExprUnaryOp& e){
	Expr2Polynomial e2p(e.expr);
	if(e2p.polynomial() == NULL
			||  e2p.polynomial()->degree() >0){
		ExprProperty p(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING);
		stack.push(p);
	}else {
		stack.push(ExprProperty());
	}
}
void ConvexityVisitor::visit(const ExprAdd& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		cout<<"NOT POLYNOMIAL"<<endl;
		e.left.acceptVisitor(*this);
		e.right.acceptVisitor(*this);
		ExprProperty l = stack.top();
		stack.pop();
		ExprProperty r = stack.top();
		stack.pop();
		l.addProperty(r);
		stack.push(l);
	}else{
		stack.push(polynomial_property(e2p.polynomial()));
	}
}
void ConvexityVisitor::visit(const ExprMinus& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		e.expr.acceptVisitor(*this);
		ExprProperty prop = stack.top();
		stack.pop();
		prop.minus();
		stack.push(prop);
	}else{
		stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
	}
}
void ConvexityVisitor::visit(const ExprSub& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		e.left.acceptVisitor(*this);
		e.right.acceptVisitor(*this);
		ExprProperty prop2 = stack.top();
		stack.pop();
		ExprProperty prop1 = stack.top();
		stack.pop();
		prop2.minus();
		prop1.addProperty(prop2);
		stack.push(prop1);
	}else{
		stack.push(polynomial_property(e2p.polynomial()));
	}
}
void ConvexityVisitor::visit(const ExprMul& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
	}else{
		stack.push(polynomial_property(e2p.polynomial()));
	}
}

void ConvexityVisitor::visit(const ExprPower& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		stack.push(ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING));
	}else{
		stack.push(polynomial_property(e2p.polynomial()));
	}
}

void ConvexityVisitor::visit(const ExprConstant& e){
	stack.push(ExprProperty());
}
void ConvexityVisitor::visit(const ExprSymbol& e){
	stack.push(ExprProperty(CONVEX_AND_CONCAVE, INCREASING));
}

void ConvexityVisitor::visit(const ExprSqr& e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL){
		e.expr.acceptVisitor(*this);
		ExprProperty p(CONVEX, NOT_INCREASING_OR_DECREASING);
		p.composition(stack.top());
		stack.pop();
		stack.push(p);
	}else{
		stack.push(polynomial_property(e2p.polynomial()));
	}
}


void ConvexityVisitor::visit( const ExprExp &e){
	Expr2Polynomial e2p(e.expr);
	if(e2p.polynomial() == NULL
			||  e2p.polynomial()->degree() >1
			|| !polynomial_property(e2p.polynomial()).isIncreasing()){
		e.expr.acceptVisitor(*this);
		ExprProperty p(CONVEX, NOT_INCREASING_OR_DECREASING);
		p.composition(stack.top());
		stack.pop();
		stack.push(p);
	}else if(e2p.polynomial()->degree() == 0){
		stack.push(ExprProperty());
	}else{
		stack.push(ExprProperty(CONVEX,INCREASING));
	}
}

void ConvexityVisitor::visit(const ExprLog&e){
	Expr2Polynomial e2p(e);
	if(e2p.polynomial() == NULL
			||  e2p.polynomial()->degree() >0){
		ExprProperty p(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING);
		stack.push(p);
	}else {
		stack.push(ExprProperty());
	}
}


void ConvexityVisitor::visit(const ExprMax& e){
	e.left.acceptVisitor(*this);
	e.right.acceptVisitor(*this);
	ExprProperty r = stack.top();
	stack.pop();
	ExprProperty l = stack.top();
	stack.pop();
	Ordering o = CONSTANT;
	if(r.ordering == l.ordering)
		o = l.ordering;
	else if(r.isIncreasing() && l.isIncreasing())
		o = INCREASING;
	else if(r.isDecreasing() && l.isDecreasing())
		o = DECREASING;
	else if(r.ordering == UNDEFINED_ORDERING || l.ordering == UNDEFINED_ORDERING)
		o = UNDEFINED_ORDERING;
	else
		o = NOT_INCREASING_OR_DECREASING;
	if(r.convexity== CONVEX_AND_CONCAVE && l.convexity == CONVEX_AND_CONCAVE){
		stack.push(ExprProperty(CONVEX_AND_CONCAVE, o));
	}else if(r.isConvex() && l.isConvex()){
		stack.push(ExprProperty(CONVEX, o));
	}else{
		stack.push(ExprProperty(UNDEFINED_CONVEXITY, NOT_INCREASING_OR_DECREASING));
	}
}

ConvexityVisitor::~ConvexityVisitor(){

}
}


