/*
 * ibex_ConvexityVisitor.h
 *
 *  Created on: Feb 22, 2015
 *      Author: wolkom
 */

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

typedef enum{
	CONVEX,
	CONCAVE,
	UNDEFINED_CONVEXITY,
	NOT_CONVEX_OR_CONCAVE
} Convexity;

typedef enum{
	INCREASING,
	DECREASING,
	CONSTANT,
	UNDEFINED_ORDERING,
	NOT_INCREASING_OR_DECREASING
} Ordering;

class ConvexityExprVisitor : public virtual ExprVisitor{
	public:

		class Constant{
		public:
			double value;

			Constant(){
				value = 0;
			}
			Constant(double d){
				value = d;
			}
		};

		class Monomial{
		public:
			double coefficient;
			vector<const ExprSymbol*> variables;
			vector<int> degrees;

			Monomial(const ExprSymbol * expr){
				variables.push_back(expr);
				degrees.push_back(1);
				coefficient = 1.0;
			}

			void multiplyByConstant(const Constant &c){
				cout<<"Coef "<<c.value<<" and current "<<this->coefficient<<endl;
				this->coefficient *=c.value;
			}

			void multiplyByVariable(const ExprSymbol * expr, int degree){
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

			void multiplyByMonomial(const Monomial&m){
				cout<<coefficient<<" and "<<m.coefficient<<endl;
				this->coefficient *= m.coefficient;
				for(int i=0; i< m.variables.size(); ++i){
					this->multiplyByVariable(m.variables[i],m.degrees[i]);
				}
			}

			bool addMonomial(const Monomial &m){
				for(int i=0; i<m.variables.size(); ++i){
					if(m.variables[i] != this->variables[i] || m.degrees[i] != this->degrees[i]){
						return false;
					}
				}
				this->coefficient+= m.coefficient;
				return true;
			}

			int degree() const{
				int degree= 0;
				for(int i=0; i< this->degrees.size();++i){
					degree+=this->degrees[i];
				}
				return degree;
			}
		};

		class ExprProperty{
		public:
			Convexity convexity;
			Ordering ordering;

			ExprProperty(){
				convexity = CONVEX;
				ordering = CONSTANT;
			}
		};

		class Expr{
		public:
			vector<Monomial> monomials;
			ExprProperty property;
			Constant constant;

			Expr(Monomial m){
				monomials.push_back(m);
			}
			Expr(ExprProperty property){
				this->property = property;
			}
			Expr(Constant constant){
				this->constant = constant;
			}

			void addMonomial(const Monomial &m){
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

			void addConstant(const Constant &c){
				this->constant.value += c.value;
			}

			void addExprProperty(const ExprProperty &p){
				if(this->property.ordering == CONSTANT){
					this->property.ordering = p.ordering;
					this->property.convexity = p.convexity;
				}
			}

			void addExpr(const Expr &e){
				for(int i=0; i<e.monomials.size();++i){
					this->addMonomial(e.monomials[i]);
				}
				this->addConstant(e.constant);
			}

			void multiplyByConstant(const Constant &c){
				this->constant.value *= c.value;
				for(int i=0; i< this->monomials.size(); ++i){
					this->monomials[i].multiplyByConstant(c);
				}
			}

			void multiplyByMonomial(const Monomial &m){
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

			void multiplyByExpr(const Expr &e){
				vector<Monomial> monomials;
				for(int i=0; i< this->monomials.size(); ++i){
					monomials = this->monomials;
				}
				this->monomials.clear();
				for(int i=0; i<e.monomials.size();++i){
					for(int j=0; j<monomials.size();++j){
						Monomial m = monomials[j];
						m.multiplyByMonomial(e.monomials[i]);
						this->addMonomial(m);
					}
					if(this->constant.value != 0.0){
						Monomial m = e.monomials[i];
						m.multiplyByConstant(this->constant);
						this->addMonomial(m);
					}
				}
				if(e.constant.value != 0.0){
					for(int j=0; j<monomials.size();++j){
						Monomial m = monomials[j];
						m.multiplyByConstant(e.constant);
						this->addMonomial(m);
					}
				}
			}

			void multiplyByExprProperty(const ExprProperty & prop){

			}
		};
	private:
		stack<Expr> _stack;
	public:
		map<const ExprNode*, int> vars;
	public:
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
		virtual void visit(const ExprSub& e);
		virtual void visit(const ExprSqr& e);
		virtual void visit(const ExprMul&);
		virtual void visit(const ExprConstant&);
		virtual void visit(const ExprSymbol&);

		virtual ExprProperty sumExprProperties(const vector<ExprProperty> &) const;
		virtual Convexity getConvexity(const Expr&);
		virtual bool isPositiveDefinite(const Matrix &M) const;
		Matrix secondOrderToMatrix(int n_vars, const vector<int> &varsA,const vector<int> &varsB, const vector<double> &coefs) const;
	};

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
			if(res.convexity == NOT_CONVEX_OR_CONCAVE
					|| v[i].convexity == NOT_CONVEX_OR_CONCAVE
					|| (v[i].convexity == CONVEX && res.convexity == CONCAVE)
					|| (v[i].convexity == CONCAVE && res.convexity == CONVEX))
				res.convexity = NOT_CONVEX_OR_CONCAVE;
			//TODO
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
			if(e.monomials[i].degree() == 2){
				for(int j=0 ; j<e.monomials[i].variables.size(); ++j){
					if(vars.find(e.monomials[i].variables[j]) == vars.end()){
						vars[e.monomials[i].variables[j]] = vars.size()-1;
					}
				}
				varsA.push_back(vars[e.monomials[i].variables[0]]);
				cout<<"Debug -- size "<<e.monomials[i].variables.size()<<endl;
				if(e.monomials[i].variables.size() == 1){
					varsB.push_back(vars[e.monomials[i].variables[0]]);
				}else{
					varsB.push_back(vars[e.monomials[i].variables[1]]);
				}
				coefs.push_back(e.monomials[i].coefficient);
			}
			else if(e.monomials[i].degree() ==1 && e.monomials[i].coefficient != 0.0){
				ExprProperty p;
				p.convexity = CONVEX;
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

		Matrix M = this->secondOrderToMatrix(vars.size(), varsA, varsB, coefs);
		if(this->isPositiveDefinite(M)){
			ExprProperty p;
			p.convexity = CONVEX;
			p.ordering = NOT_INCREASING_OR_DECREASING;
			expressions.push_back(p);
		}
		return this->sumExprProperties(expressions).convexity;
	}

	bool ConvexityExprVisitor::isPositiveDefinite(const Matrix &M) const{
		double sum;
		assert(M.nb_cols() == M.nb_rows());
		for(int i=0; i < M.nb_cols(); ++i){
			sum = M[i][i];
			for(int j=0; j < i; ++j)
				sum -= M[i][j]*M[j][i];
			if(sum <= 0.0)
				return false;
		}
		return true;
	}

	void ConvexityExprVisitor::visit(const ExprUnaryOp& e){
		cout << "Unary expression" << endl;
		const ExprPower * p = dynamic_cast<const ExprPower*>(&e);
		if(p == NULL)
			cout<<e.expr<<endl;
		else
			cout<<"--ExprPower"<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprAdd& e) {
		e.left.acceptVisitor((*this));
		e.right.acceptVisitor((*this));
		Expr expr = this->_stack.top();
		this->_stack.pop();
		cout<<"Add +\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		this->_stack.top().addExpr(expr);
		cout<<this->_stack.top()<<endl;
	}


	void ConvexityExprVisitor::visit(const ExprSub& e) {
		e.left.acceptVisitor((*this));
		e.right.acceptVisitor((*this));

		Expr expr = this->_stack.top();
		this->_stack.pop();

		cout<<"Sub -\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		expr.multiplyByConstant(Constant(-1.0));
		this->_stack.top().addExpr(expr);
		cout<<this->_stack.top()<<endl;

		Convexity test = this->getConvexity(this->_stack.top());
		if(test == CONVEX)
			cout<<"FUNCTION IS CONVEX"<<endl;
		else if(test == CONCAVE)
			cout<<"FUNCTION IS CONCAVE"<<endl;
		else
			cout<<"FUNCTION IS NEITHER CONVEX OR CONCAVE"<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprConstant& e) {
		cout<<"Constant "<<e<<endl;
		Constant expr(e.get_value().lb());
		this->_stack.push(Expr(expr));
	}

	void ConvexityExprVisitor::visit(const ExprSymbol& e) {
		cout<< "Symbol "<<e<<endl;
		Monomial expr(&e);
		this->_stack.push(Expr(expr));
	}

	void ConvexityExprVisitor::visit(const ExprSqr& e){
		e.expr.acceptVisitor(*this);
		Expr expr = this->_stack.top();
		cout<<"ExprSqr"<<endl;
		this->_stack.top().multiplyByExpr(expr);
		cout<<this->_stack.top()<<endl;
	}

	void ConvexityExprVisitor::visit(const ExprMul& e){
		e.left.acceptVisitor(*this);
		e.right.acceptVisitor(*this);
		Expr expr = this->_stack.top();
		this->_stack.pop();
		cout<<"Mult *\t"<<expr<<"\tand\t"<<this->_stack.top()<<endl;
		this->_stack.top().multiplyByExpr(expr);
		cout<<this->_stack.top()<<endl;
	}

}


#endif /* SRC_FUNCTION_IBEX_CONVEXITYEXPRVISITOR_H_ */
