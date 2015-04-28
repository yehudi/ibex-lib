/*
 * ibex_Expr2Polynomial.h
 *
 *  Created on: Apr 28, 2015
 *      Author: wolkom
 */

#ifndef SRC_FUNCTION_IBEX_EXPR2POLYNOMIAL_H_
#define SRC_FUNCTION_IBEX_EXPR2POLYNOMIAL_H_

#include <vector>
#include <stack>
#include <map>

#include "ibex_ExprVisitor.h"
#include "ibex_Linear.h"
#include "ibex_Function.h"

using namespace std;

namespace ibex{

class Monomial;


class MathExpection : Exception{

};

class Polynomial{
public:
	/**
	 * \pre must be a polynomial of degree 2
	 * \throw ...
	 *
	 * \return the matrix A and vector b such that P(x) = x^T Ax + b^T x
	 */
	virtual pair<IntervalMatrix, IntervalVector> quadra_form() const;

	virtual Polynomial * quadra_polynomial() const;

	virtual Monomial getMonomial(int i) const = 0;

	virtual void addMonomial(Monomial m) = 0;

	virtual int size() const = 0;

	/**
	 * \return degree of the highest monomial
	 */
	virtual int degree() const = 0;

	virtual ~Polynomial(){};
};



/**
 * \brief representation of a monomial
 */
class Monomial : public Polynomial{
public:
	Interval coef;
	vector<const ExprSymbol *> variables;
	vector<int> degrees;

	Monomial(Interval i) : coef(i){}
	Monomial(const ExprSymbol *e): coef(1.){
		variables.push_back(e);
		degrees.push_back(1);
	}
	Monomial(const ExprSymbol *e, int degree): coef(1.){
		if(degree != 0){
			variables.push_back(e);
			degrees.push_back(degree);
		}
	}

	virtual Monomial getMonomial(int i) const;
	virtual int size() const;
	virtual int degree()const;
	virtual int absDegree() const;
	virtual void addMonomial(Monomial m);

	void multiplyByConstant(Interval c);
	void multiplyByVariable(const ExprSymbol * expr, int degree);
	void multiplyByMonomial(const Monomial &m);

	virtual ~Monomial(){}
};



class MonomialSum : public Polynomial{
public:
	vector<Monomial> monos;

	MonomialSum(){}
	MonomialSum(const MonomialSum& s){
		for(int i = 0; i<s.monos.size(); ++i){
			monos.push_back(s.monos[i]);
		}
	}

	virtual Monomial getMonomial(int i) const;

	virtual void addMonomial(Monomial m);

	virtual int size() const;

	/**
	 * \return degree of the highest monomial
	 */
	virtual int degree() const;

	virtual ~MonomialSum(){}
};

/**
 * \brief Convexity property
 */
typedef enum{
	CONVEX,
	CONCAVE,
	CONVEX_AND_CONCAVE,
	CONVEX_OR_CONCAVE,
	UNDEFINED_CONVEXITY,
	NOT_CONVEX_OR_CONCAVE
} Convexity;

/**
 * \brief Ordering property
 */
typedef enum{
	INCREASING,
	DECREASING,
	CONSTANT,
	INCREASING_OR_DECREASING,
	UNDEFINED_ORDERING,
	NOT_INCREASING_OR_DECREASING
} Ordering;

/**
 * propreties of any expression
 */
class ExprProperty{
public:
	Convexity convexity;
	Ordering ordering;
	ExprProperty() : convexity(CONVEX_AND_CONCAVE), ordering(CONSTANT){}
	ExprProperty(Convexity c, Ordering o) : convexity(c), ordering(o){}

	bool isConvex() const;

	bool isConcave() const;

	bool isIncreasing() const;

	bool isDecreasing() const;

	bool isConstant() const;

	void addProperty(const ExprProperty &p);
};



class Expr2Polynomial : public ExprVisitor {
protected:
	MonomialSum poly;
	std::stack<Monomial> stack;
	bool notMonomialDetected;
public:
	Expr2Polynomial(const ExprNode&);

	Polynomial* polynomial();

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

	virtual ~Expr2Polynomial();

};

ExprProperty function_property( const Function &f);

}



#endif /* SRC_FUNCTION_IBEX_EXPR2POLYNOMIAL_H_ */
