/* ============================================================================
 * I B E X - Convexity Function visitor
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Yehudi Hollevoet
 * Created     : Feb 22, 2015
 * ---------------------------------------------------------------------------- */

#ifndef SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_
#define SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_

#include <vector>
#include <stack>
#include <vector>
#include <map>
#include <iostream>
#include "ibex_ExprVisitor.h"

using namespace std;

namespace ibex{

/**
 * \brief Convexity property
 */
typedef enum{
	CONVEX,
	CONCAVE,
	CONVEX_AND_CONCAVE,
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
	UNDEFINED_ORDERING,
	NOT_INCREASING_OR_DECREASING
} Ordering;

/**
 * \brief CholeskyDecomposition of a matrix
 */
class CholeskyDecomposition{
private:
	const Matrix _m;
public:
	CholeskyDecomposition(const Matrix &M) : _m(M){}
	/**
	 * \brief does a unique Cholesky decomposition of the matrix exists
	 */
	virtual bool existsUniqueDecomposition() const;
	/**
	 * \brief get the L matrix of the Cholesky decomposition of M sothat M = L*L transpose
	 */
	virtual Matrix getUniqueDecomposition() const;
	virtual ~CholeskyDecomposition(){}
};

/**
 * \brief ExprVisitor for the study of convexity
 */
class ConvexityExprVisitor : public virtual ExprVisitor{
	friend class FunctionConvexity;
	public:
		/**
		 * \brief Representation of a constant value
		 */
		class Constant{
		public:
			double value;
			Constant() : value(0){}
			Constant(double d): value(d){}
		};

		/**
		 * \brief Representation of a monomial
		 */
		class Monomial{
		public:
			double coefficient;
			vector<const ExprSymbol*> variables;
			vector<int> degrees;
			Monomial(const ExprSymbol * expr);
			void multiplyByConstant(const Constant &c);
			void multiplyByVariable(const ExprSymbol * expr, int degree);
			void multiplyByMonomial(const Monomial&m);
			bool addMonomial(const Monomial &m);
			int degree() const;
		};

		/**
		 * \brief Representation of a function by its property (convexity and ordering)
		 */
		class ExprProperty{
		public:
			Convexity convexity;
			Ordering ordering;
			ExprProperty() : convexity(CONVEX_AND_CONCAVE), ordering(CONSTANT){}
		};

		/**
		 * \brief Expression simplified to a sum of monomials, constants and function properties
		 */
		class Expr{
		public:
			vector<Monomial> monomials;
			ExprProperty property;
			Constant constant;

			Expr(Monomial m);
			Expr(ExprProperty property);
			Expr(Constant constant);
			/**
			 * \brief Opposite of the Expr
			 */
			void minus();
			/**
			 * \brief add a monomial to the expression
			 */
			void addMonomial(const Monomial &m);
			/**
			 * \brief add a constant to the expression
			 */
			void addConstant(const Constant &c);
			/**
			 * \brief add a function represented by its properties to the expression
			 */
			void addExprProperty(const ExprProperty &p);
			/**
			 * \brief add an other expression to the current expression
			 */
			void addExpr(const Expr &e);
			/**
			 * \brief multiply expression by a constant
			 */
			void multiplyByConstant(const Constant &c);
			/**
			 * \brief multiply expression by a monomial
			 */
			void multiplyByMonomial(const Monomial &m);
			/**
			 * \brief multiply expression by an other expression
			 */
			void multiplyByExpr(const Expr &e);
			/**
			 * \brief multiply expression by a function represented by its properties
			 */
			void multiplyByExprProperty(const ExprProperty & prop);
		};

		ConvexityExprVisitor(){
		}

		virtual ~ConvexityExprVisitor() {}

		virtual void visit(const ExprNode&){}
		virtual void visit(const ExprIndex&){}
		virtual void visit(const ExprLeaf&){}
		virtual void visit(const ExprNAryOp&){}
		virtual void visit(const ExprBinaryOp&){}
		virtual void visit(const ExprUnaryOp& e);
		virtual void visit(const ExprAdd& e);
		virtual void visit(const ExprMinus& e);
		virtual void visit(const ExprSub& e);
		virtual void visit(const ExprSqr& e);
		virtual void visit(const ExprMul&);
		virtual void visit(const ExprPower&);
		virtual void visit(const ExprConstant&);
		virtual void visit(const ExprSymbol&);

		/**
		 * \brief sum multiple expression properties
		 */
		virtual ExprProperty sumExprProperties(const vector<ExprProperty> &) const;
		/**
		 * \brief get convexity of an expression
		 */
		virtual Convexity getConvexity(const Expr&);
		/**
		 * \brief convert a second order polynomial P to a matrix M so that P = x*M*x(transpose)
		 */
		Matrix secondOrderToMatrix(int n_vars, const vector<int> &varsA,const vector<int> &varsB, const vector<double> &coefs) const;
	private:
			stack<Expr> _stack;
	public:
		map<const ExprNode*, int> vars;
};

ostream& operator<<(ostream& flux, const Convexity &c);
ostream& operator<<(ostream& flux, const ConvexityExprVisitor::Monomial &m);
ostream& operator<<(ostream &flux, const ConvexityExprVisitor::Expr& e);

}


#endif /* SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_ */
