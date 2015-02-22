/*
 * ibex_ConvexityVisitor.h
 *
 *  Created on: Feb 22, 2015
 *      Author: wolkom
 */

#ifndef SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_
#define SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_

#include <vector>
#include <map>
#include <iostream>
#include "ibex_ExprVisitor.h"

using namespace std;

namespace ibex{

class ConvexityExprVisitor : public virtual ExprVisitor{
	public:
		vector<int> varsA;
		vector<int> varsB;
		vector<double> coefs;
		map<const ExprNode*, int> vars;
		double coef;
	public:
		ConvexityExprVisitor(){
			varsA = vector<int>();
			varsB = vector<int>();
			coefs = vector<double>();
			coef = 1;
			vars = map<const ExprNode*, int>();
		}
		virtual ~ConvexityExprVisitor() {}

		virtual void visit(const ExprNode&){}
		virtual void visit(const ExprIndex&){}
		virtual void visit(const ExprLeaf&){}
		virtual void visit(const ExprNAryOp&){}
		virtual void visit(const ExprBinaryOp&){}
		virtual void visit(const ExprUnaryOp& e){
			cout << "Unary expression" << endl;
			const ExprPower * p = dynamic_cast<const ExprPower*>(&e);
			if(p == NULL)
				cout<<e.expr<<endl;
			else
				cout<<"ExprPower"<<endl;
		}

		virtual void visit(const ExprAdd& e) {
			e.left.acceptVisitor((*this));
			e.right.acceptVisitor((*this));
		}


		virtual void visit(const ExprSub& e) {
			e.left.acceptVisitor((*this));
			e.right.acceptVisitor((*this));
		}

		virtual void visit(const ExprSqr& e);
		virtual void visit(const ExprMul&);
		virtual void visit(const ExprConstant&);
	};

	void ConvexityExprVisitor::visit(const ExprConstant& e) {
		this->coef = e.get_value().lb();
	}

	void ConvexityExprVisitor::visit(const ExprSqr& e){
		const ExprSymbol* var = dynamic_cast<const ExprSymbol*>(&e.expr);
		if(var != NULL){
			cout << "Ce nest pas une variable" << endl;
			if(vars.find(var) != vars.end()){
				vars[var] = vars.size();
			}
			coefs.push_back(coef);
			varsA.push_back(vars[var]);
			varsB.push_back(vars[var]);
		}
	}

	void ConvexityExprVisitor::visit(const ExprMul& e){
		const ExprSymbol* left = dynamic_cast<const ExprSymbol*>(&e.left);
		const ExprSymbol* right = dynamic_cast<const ExprSymbol*>(&e.right);
		if(left == NULL || right == NULL){
			cout << "Ce ne sont pas des variable" << endl;
		}else{
			cout << "Ce sont des variables " << (*left) << " " << (*right)<< endl;
		}
	}

}


#endif /* SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_ */
