/* ============================================================================
 * I B E X - Function Convexity Analyser
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Yehudi Hollevoet
 * Created     : Mar 16, 2015
 * ---------------------------------------------------------------------------- */

#include "ibex_ConvexityExprVisitor.h"
#include <vector>
#include <stack>
#include <vector>
#include <map>
#include <iostream>
#include "ibex_ExprVisitor.h"
#include "ibex_Expr.h"

using namespace std;

namespace ibex{
	bool CholeskyDecomposition::existsUniqueDecomposition() const{
		assert(this->_m.nb_cols() == this->_m.nb_rows());
		int n = this->_m.nb_rows();
		double sum;
		double matrix[n*n];

		for(int i=0; i < n; ++i){
			for(int j=0; j<i; ++j){
				sum = this->_m[i][j];
				for(int k=0; k<j; ++k)
					sum -= matrix[n*i + k]*matrix[n*j + k];
				matrix[n*i + j] = sum / matrix[n*j + j];
			}
			//Calculation of diagonal
			sum = this->_m[i][i];
			for(int j=0; j < i; ++j)
				sum -= matrix[n*i + j]*matrix[n*i + j];
			if(sum < 0.0){
				return false;
			}
			matrix[n*i+i] = std::sqrt(sum);
		}
		return true;
	}

	Matrix CholeskyDecomposition::getUniqueDecomposition() const{
		assert(this->_m.nb_cols() == this->_m.nb_rows());
		int n = this->_m.nb_rows();
		double sum;
		double matrix[n*n];
		for(int i=0; i < n; ++i){
			for(int j=0; j<i; ++j){
				sum = this->_m[i][j];
				for(int k=0; k<j; ++k)
					sum -= matrix[n*i + k]*matrix[n*j + k];
				matrix[n*i + j] = sum / matrix[n*j + j];
			}
			//fill with 0
			for(int j = i+1; j<n ;++j){
				matrix[n*i + j] = 0.0;
			}
			//Calculation of diagonal
			sum = this->_m[i][i];
			for(int j=0; j < i; ++j)
				sum -= matrix[n*i + j]*matrix[n*i + j];
			matrix[n*i+i] = std::sqrt(sum);
		}
		return Matrix(n, n, matrix);
	}

	ostream& operator<<(ostream& flux, const Convexity &c){
		switch(c){
		case CONVEX:
			flux<<"convex";
			break;
		case CONCAVE:
			flux<<"concave";
			break;
		case CONVEX_AND_CONCAVE:
			flux<<"concave and convex";
			break;
		case UNDEFINED_CONVEXITY:
			flux<<"undefined convexity";
			break;
		default:
			flux<<"not convex or concave";
			break;
		}
		return flux;
	}


	/*
	 * ConvexityExprVisior::Constant
	 */




	/*
	 * ConvexityExprVisior::Monomial
	 */

	ostream& operator<<(ostream& flux, const ConvexityExprVisitor::Monomial &m){
		if(m.coefficient != 1 && m.coefficient != -1){
			flux<<m.coefficient;
		}else if(m.coefficient == -1){
			flux<<"-";
		}
		for(int i=0; i<m.variables.size();++i){
			if(m.degrees[i] != 0){
				flux<<*m.variables[i];
				if(m.degrees[i] != 1){
					flux<<"^"<<m.degrees[i];
				}
			}
		}
		return flux;
	}

	ConvexityExprVisitor::Monomial::Monomial(const ExprSymbol * expr){
		variables.push_back(expr);
		degrees.push_back(1);
		coefficient = 1.0;
	}


	void ConvexityExprVisitor::Monomial::multiplyByConstant(const Constant &c){
		this->coefficient *=c.value;
	}

	void ConvexityExprVisitor::Monomial::multiplyByVariable(const ExprSymbol * expr, int degree){
		bool newVar = true;
		int position = 0;
		for(int i=0; i< this->variables.size();++i ){
			if(this->variables[position] < expr){
				position ++;
			}
			if(variables[i] == expr){
				this->degrees[i] += degree;
				newVar = false;
			}
		}
		if(newVar){
			this->variables.insert(this->variables.begin()+position, expr);
			this->degrees.insert(this->degrees.begin()+position, degree);
		}
	}

	void ConvexityExprVisitor::Monomial::multiplyByMonomial(const Monomial&m){
		this->coefficient *= m.coefficient;
		for(int i=0; i< m.variables.size(); ++i){
			this->multiplyByVariable(m.variables[i],m.degrees[i]);
		}
	}

	bool ConvexityExprVisitor::Monomial::addMonomial(const Monomial &m){
		for(int i=0; i<m.variables.size(); ++i){
			if(m.variables[i] != this->variables[i] || m.degrees[i] != this->degrees[i]){
				return false;
			}
		}
		this->coefficient+= m.coefficient;
		return true;
	}

	int ConvexityExprVisitor::Monomial::degree() const{
		int degree= 0;
		for(int i=0; i< this->degrees.size();++i){
			degree+=this->degrees[i];
		}
		return degree;
	}

	/*
	 * ConvexityExprVisior::ExprProperty
	 */


	/*
	 * ConvexityExprVisior::Expr
	 */

	ostream& operator<<(ostream &flux, const ConvexityExprVisitor::Expr& e){
		for(int i=0; i<e.monomials.size();++i){
			flux<<e.monomials[i];
			if(i<e.monomials.size()-1 && e.monomials[i+1].coefficient > 0)
				flux<<"+";

		}
		if(e.constant.value != 0.0){
			if(e.constant.value > 0)
				flux<<"+";
			flux<<e.constant.value;
		}
		return flux;
	}

	ConvexityExprVisitor::Expr::Expr(Monomial m){
		monomials.push_back(m);
	}

	ConvexityExprVisitor::Expr::Expr(ExprProperty property){
		this->property = property;
	}

	ConvexityExprVisitor::Expr::Expr(Constant constant){
		this->constant = constant;
	}

	void ConvexityExprVisitor::Expr::minus(){
		this->constant.value = -this->constant.value;
		for(int i=0; i<this->monomials.size(); ++i){
			this->monomials[i].coefficient = -this->monomials[i].coefficient;
		}
		if(this->property.convexity == CONVEX)
			this->property.convexity = CONCAVE;
		else if(this->property.convexity == CONCAVE)
			this->property.convexity = CONVEX;

		if(this->property.ordering == INCREASING)
			this->property.ordering = DECREASING;
		else if(this->property.ordering == DECREASING)
			this->property.ordering = INCREASING;
	}

	void ConvexityExprVisitor::Expr::addMonomial(const Monomial &m){
		for(int i=0; i<this->monomials.size();++i){
			if(this->monomials[i].addMonomial(m)){
				if(this->monomials[i].coefficient == 0.0){
					this->monomials.erase(this->monomials.begin() + i);
				}
				return;
			}
		}
		this->monomials.push_back(m);
	}

	void ConvexityExprVisitor::Expr::addConstant(const Constant &c){
		this->constant.value += c.value;
	}

	void ConvexityExprVisitor::Expr::addExprProperty(const ExprProperty &p){
		if(this->property.ordering == CONSTANT){
			this->property.ordering = p.ordering;
			this->property.convexity = p.convexity;
		}
	}

	void ConvexityExprVisitor::Expr::addExpr(const Expr &e){
		for(int i=0; i<e.monomials.size();++i){
			this->addMonomial(e.monomials[i]);
		}
		this->addConstant(e.constant);
	}

	void ConvexityExprVisitor::Expr::multiplyByConstant(const Constant &c){
		this->constant.value *= c.value;
		for(int i=0; i< this->monomials.size(); ++i){
			this->monomials[i].multiplyByConstant(c);
		}
	}

	void ConvexityExprVisitor::Expr::multiplyByMonomial(const Monomial &m){
		Monomial monomial = m;
		monomial.multiplyByConstant(this->constant);
		for(int i=0; i<this->monomials.size(); ++i){
			this->monomials[i].multiplyByMonomial(m);
		}
		if(this->constant.value != 0){
			this->addMonomial(m);
			this->constant = Constant();
		}

	}

	void ConvexityExprVisitor::Expr::multiplyByExpr(const Expr &e){
		vector<Monomial> monomials = this->monomials;
		Constant c = this->constant;

		this->constant.value = 0.0;
		this->monomials.clear();

		for(int i=0; i<e.monomials.size();++i){
			for(int j=0; j<monomials.size();++j){
				Monomial m = monomials[j];
				m.multiplyByMonomial(e.monomials[i]);
				this->addMonomial(m);
			}
			if(c.value != 0.0){
				Monomial m = e.monomials[i];
				m.multiplyByConstant(c);
				this->addMonomial(m);
			}
		}
		if(e.constant.value != 0.0){
			for(int j=0; j<monomials.size();++j){
				Monomial m = monomials[j];
				m.multiplyByConstant(e.constant);
				this->addMonomial(m);
			}
			this->constant.value = c.value * e.constant.value;
		}
	}

	void ConvexityExprVisitor::Expr::multiplyByExprProperty(const ExprProperty & prop){

	}

	/*
	 * ConvexityExprVisior
	 */

	Matrix ConvexityExprVisitor::secondOrderToMatrix(int n_vars, const vector<int> &varsA,const vector<int> &varsB, const vector<double> &coefs) const{
		assert(varsA.size() == varsB.size() && varsA.size() == coefs.size());

		Matrix M(n_vars,n_vars,0.0);

		for(int i = 0; i<varsA.size();++i){
			if(varsA[i] == varsB[i])
				M[varsA[i]][varsB[i]] = coefs[i];
			else{
				M[varsA[i]][varsB[i]] = coefs[i]/2;
				M[varsB[i]][varsA[i]] = coefs[i]/2;
			}
		}

		return M;
	}


	ConvexityExprVisitor::ExprProperty ConvexityExprVisitor::sumExprProperties(const vector<ExprProperty> &v) const{
		assert(!v.empty());
		ExprProperty res = v[0];
		for(int i=1; i<v.size(); ++i){
			if(res.convexity == v[i].convexity)
				continue;
			else if((res.convexity == CONVEX && v[i].convexity == CONCAVE)
					|| (res.convexity == CONCAVE && v[i].convexity == CONVEX)
					|| v[i].convexity == UNDEFINED_CONVEXITY
					|| res.convexity == UNDEFINED_CONVEXITY)
				res.convexity = UNDEFINED_CONVEXITY;
			else if(res.convexity == CONVEX_AND_CONCAVE)
				res.convexity = v[i].convexity;
			else if((res.convexity == NOT_CONVEX_OR_CONCAVE && (v[i].convexity == CONVEX || v[i].convexity == CONCAVE))
					|| (v[i].convexity == NOT_CONVEX_OR_CONCAVE && (res.convexity == CONVEX || res.convexity == CONCAVE)))
				res.convexity = NOT_CONVEX_OR_CONCAVE;
		}
		return res;
	}

	Convexity ConvexityExprVisitor::getConvexity(const ConvexityExprVisitor::Expr& e){
		vector<ExprProperty> expressions;
		map<const ExprNode*,int> vars;
		vector<int> varsA;
		vector<int> varsB;
		vector<double> coefs;
		expressions.push_back(e.property);

		for(int i=0; i<e.monomials.size(); ++i){
			if(e.monomials[i].coefficient != 0.0){
				if(e.monomials[i].degree() == 0){
					continue;
				}
				else if(e.monomials[i].degree() == 2){
					for(int j=0 ; j<e.monomials[i].variables.size(); ++j){
						if(vars.find(e.monomials[i].variables[j]) == vars.end()){
							vars[e.monomials[i].variables[j]] = vars.size()-1;
						}
					}
					varsA.push_back(vars[e.monomials[i].variables[0]]);
					if(e.monomials[i].variables.size() == 1){
						varsB.push_back(vars[e.monomials[i].variables[0]]);
					}else{
						varsB.push_back(vars[e.monomials[i].variables[1]]);
					}
					coefs.push_back(e.monomials[i].coefficient);
				}
				else if(e.monomials[i].degree() ==1){
					ExprProperty p;
					p.convexity = CONVEX_AND_CONCAVE;
					if(e.monomials[i].coefficient > 0)
						p.ordering = INCREASING;
					else
						p.ordering = DECREASING;
					expressions.push_back(p);
				}else{
					ExprProperty p;
					p.convexity = CONVEX;
					p.ordering = INCREASING;
					for(int j=0; j<e.monomials[i].variables.size();++j){
						if(e.monomials[i].degrees[j] % 2 != 0){
							p.convexity = UNDEFINED_CONVEXITY;
							break;
						}else if(p.ordering == INCREASING){
							p.ordering = NOT_INCREASING_OR_DECREASING;
						}
					}
					if(e.monomials[i].coefficient < 0){
						if(p.convexity == CONVEX)
							p.convexity = CONCAVE;
						if(p.ordering == INCREASING)
							p.ordering = DECREASING;
					}
					expressions.push_back(p);
				}
			}
		}
		if(varsA.size() > 0 ){
			Matrix M = this->secondOrderToMatrix(vars.size(), varsA, varsB, coefs);
			CholeskyDecomposition choleskyDecomposition(M);

			if(choleskyDecomposition.existsUniqueDecomposition()){
				ExprProperty p;
				p.convexity = CONVEX;
				p.ordering = NOT_INCREASING_OR_DECREASING;
				expressions.push_back(p);
			}else{
				for(int i=0; i<coefs.size(); ++i){
					coefs[i] = -coefs[i];
				}
				M = this->secondOrderToMatrix(vars.size(), varsA, varsB, coefs);
				CholeskyDecomposition choleskyDecomposition2(M);
				if(choleskyDecomposition2.existsUniqueDecomposition()){
					ExprProperty p;
					p.convexity = CONCAVE;
					p.ordering = NOT_INCREASING_OR_DECREASING;
					expressions.push_back(p);
				}else{
					ExprProperty p;
					p.convexity = NOT_CONVEX_OR_CONCAVE;
					p.ordering = NOT_INCREASING_OR_DECREASING;
					expressions.push_back(p);
				}
			}
		}
		return this->sumExprProperties(expressions).convexity;
	}

	void ConvexityExprVisitor::visit(const ExprUnaryOp& e){
		e.expr.acceptVisitor(*this);
		this->_stack.pop();
		ExprProperty p;
		p.convexity = NOT_CONVEX_OR_CONCAVE;
		p.ordering = NOT_INCREASING_OR_DECREASING;
		this->_stack.push(Expr(p));
	}

	void ConvexityExprVisitor::visit(const ExprAdd& e) {
		e.left.acceptVisitor((*this));
		e.right.acceptVisitor((*this));
		Expr expr = this->_stack.top();
		this->_stack.pop();
		//cout<<"Add +\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		this->_stack.top().addExpr(expr);
		//cout<<this->_stack.top()<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprMinus& e) {
		e.expr.acceptVisitor(*this);
		this->_stack.top().minus();
		//cout<<"Minus -\t"<<this->_stack.top()<<endl;
		//cout<<this->_stack.top()<<endl;
	}


	void ConvexityExprVisitor::visit(const ExprSub& e) {
		e.left.acceptVisitor((*this));
		e.right.acceptVisitor((*this));

		Expr expr = this->_stack.top();
		this->_stack.pop();

		//cout<<"Sub -\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		expr.multiplyByConstant(Constant(-1.0));
		this->_stack.top().addExpr(expr);
		//cout<<this->_stack.top()<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprConstant& e) {
		Constant expr(e.get_value().lb());
		this->_stack.push(Expr(expr));
	}

	void ConvexityExprVisitor::visit(const ExprSymbol& e) {
		Monomial expr(&e);
		this->_stack.push(Expr(expr));
	}

	void ConvexityExprVisitor::visit(const ExprSqr& e){
		e.expr.acceptVisitor(*this);
		Expr expr = this->_stack.top();
		//cout<<"ExprSqr"<<endl;
		this->_stack.top().multiplyByExpr(expr);
		//cout<<this->_stack.top()<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprMul& e){
		e.left.acceptVisitor(*this);
		e.right.acceptVisitor(*this);
		Expr expr = this->_stack.top();
		this->_stack.pop();
		//cout<<"Mult *\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		this->_stack.top().multiplyByExpr(expr);
		//cout<<this->_stack.top()<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprPower& e){
		e.expr.acceptVisitor(*this);
		//cout<<"Power \t"<<this->_stack.top()<<"^"<<e.expon<<endl;
		Expr &expr = this->_stack.top();
		if(expr.property.ordering == CONSTANT){
			if(expr.monomials.size() == 1 && expr.constant.value == 0){
				for(int i=0; i<expr.monomials[0].degrees.size();++i){
					this->_stack.top().monomials[0].degrees[i] *= e.expon;
				}
			}else if(expr.monomials.size() == 0){
				this->_stack.top().constant =  std::pow(expr.constant.value, e.expon);
			}else{
				//TODO
				//cout<<"/!/ more than one monomial"<<endl;
			}
		}else{
			//TODO
			//cout<<"/!/ expr ordering is not constant"<<endl;
		}
		//cout<<this->_stack.top()<<endl;
	}
}
