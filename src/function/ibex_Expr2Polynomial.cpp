/*
 * ibex_Expr2Polynomial.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: HOLLEVOET Yehudi
 */

#include <utility>

#include "ibex_Expr2Polynomial.h"
namespace ibex{

Monomial Monomial::getMonomial(int i) const{
	if(i == 0)
		return *this;
	else
		throw Exception();
}

int Monomial::size()const{
	return 1;
}

int Monomial::degree()const{
	int sum = 0;
	for(int i=0; i<degrees.size(); ++i){
		sum += degrees[i];
	}
	return sum;
}

int Monomial::absDegree()const{
	int sum = 0;
	for(int i=0; i<degrees.size(); ++i){
		if(degrees[i]>= 0 )
			sum += degrees[i];
		else
			sum-= degrees[i];
	}
	return sum;
}

void Monomial::addMonomial(Monomial m){
	throw Exception();
}

void Monomial::multiplyByConstant(Interval c){
	coef *=c;
}

void Monomial::multiplyByVariable(const ExprSymbol * expr, int degree){
	bool newVar = true;
	int position = 0;
	for(int i=0; i< this->variables.size();++i ){
		if(this->variables[position] < expr){
			position ++;
		}
		if(variables[i] == expr){
			this->degrees[i] += degree;
			if(this->degrees[i] == 0){
				this->variables.erase(this->variables.begin() + i);
				this->degrees.erase(this->degrees.begin()+i);
			}
			newVar = false;
			break;
		}
	}
	if(newVar){
		this->variables.insert(this->variables.begin()+position, expr);
		this->degrees.insert(this->degrees.begin()+position, degree);
	}
}

void Monomial::multiplyByMonomial(const Monomial&m){
	coef *= m.coef;
	for(int i=0; i< m.variables.size(); ++i){
		this->multiplyByVariable(m.variables[i],m.degrees[i]);
	}
}

ostream& operator<<(ostream& f, const Monomial& m){
	f<<m.coef;
	for(int i=0; i<m.variables.size(); ++i){
		f<<*m.variables[i];
		if(m.degrees[i] != 1)
			f<<"^"<<m.degrees[i];
	}
	return f;
}

ostream& operator<<(ostream& f, const Polynomial* m){
	for(int i=0; i<m->size(); ++i){
		if(i>0)
			f<<"+";
		f<<m->getMonomial(i);
	}
	return f;
}

int MonomialSum::size() const{
	return monos.size();
}

Monomial MonomialSum::getMonomial(int i) const{
	return monos[i];
}

void MonomialSum::addMonomial(Monomial m){
	monos.push_back(m);
}

int MonomialSum::degree()const{
	int degree = 0;
	for(int i =0; i< monos.size(); ++i){
		if(monos[i].degree() > degree)
			degree = monos[i].degree();
	}
	return degree;
}

Polynomial * Polynomial::quadra_polynomial() const{
	MonomialSum * ms = new MonomialSum();
	for(int i=0; i<size(); ++i){
		Monomial m = getMonomial(i);
		if(m.absDegree() <= 2 && m.degree() >= 0 && m.variables.size() <= 2 ){
			ms->addMonomial(m);
		}
	}
	return ms;
}

std::pair<IntervalMatrix, IntervalVector> Polynomial::quadra_form() const{
	vector<int> varsA;
	vector<int> varsB;
	vector<Interval> coefs;
	map<const ExprNode*,int> vars_pos;
	vector<int> varsDeg1;
	vector<Interval> deg1;

	for(int i =0; i<size(); ++i){
		Monomial m = getMonomial(i);
		if(m.absDegree() == 1 && m.variables.size() == 1){
			if(vars_pos.find(m.variables[0]) == vars_pos.end()){
				vars_pos[m.variables[0]] = vars_pos.size()-1;
			}
			varsDeg1.push_back(vars_pos.find(m.variables[0])->second);
			deg1.push_back(m.coef);
		}else if(m.variables.size() == 0)
			continue;
		else if(m.absDegree() != 2 || m.variables.size() > 2)
			throw Exception();
		else{
			if(vars_pos.find(m.variables[0]) == vars_pos.end()){
				vars_pos[m.variables[0]] = vars_pos.size()-1;
			}
			varsA.push_back(vars_pos[m.variables[0]]);
			if(m.variables.size() == 2){
				if(vars_pos.find(m.variables[1]) == vars_pos.end()){
					vars_pos[m.variables[1]] = vars_pos.size()-1;
				}
				varsB.push_back(vars_pos[m.variables[1]]);
			}else{
				varsB.push_back(vars_pos[m.variables[0]]);
			}
			coefs.push_back(m.coef);
		}
	}

	int n_vars = vars_pos.size();

	IntervalMatrix M(n_vars,n_vars,0.0);
	IntervalVector V(n_vars,0.0);

	for(int i=0; i<deg1.size(); ++i){
		V.put(varsDeg1[i],IntervalVector(1,deg1[i]));
	}
	for(int i = 0; i<varsA.size();++i){
		if(varsA[i] == varsB[i])
			M[varsA[i]][varsB[i]] = coefs[i];
		else{
			M[varsA[i]][varsB[i]] = coefs[i]/2;
			M[varsB[i]][varsA[i]] = coefs[i]/2;
		}
	}
	return std::make_pair(M,V);
}

bool ExprProperty::isConstant() const{
	return ordering == CONSTANT;
}

bool ExprProperty::isConvex() const{
	return convexity == CONVEX || convexity == CONVEX_AND_CONCAVE;
}

bool ExprProperty::isConcave() const{
	return convexity == CONCAVE || convexity == CONVEX_AND_CONCAVE;
}

bool ExprProperty::isIncreasing() const{
	return ordering == INCREASING || ordering == CONSTANT;
}

bool ExprProperty::isDecreasing() const{
	return ordering == DECREASING || ordering == CONSTANT;
}

void ExprProperty::addProperty( const ExprProperty &p){
	if(p.convexity == convexity || convexity == UNDEFINED_CONVEXITY){}
	else if(convexity == CONVEX_AND_CONCAVE)
			convexity = p.convexity;
	else if((p.convexity == CONVEX && convexity == CONCAVE)
			|| (p.convexity == CONCAVE && convexity == CONVEX)
			|| p.convexity == UNDEFINED_CONVEXITY)
		convexity = UNDEFINED_CONVEXITY;
	else if((convexity == NOT_CONVEX_OR_CONCAVE && (p.convexity == CONVEX || p.convexity == CONCAVE))
			|| (p.convexity == NOT_CONVEX_OR_CONCAVE && (convexity == CONVEX || convexity == CONCAVE)))
		convexity = NOT_CONVEX_OR_CONCAVE;
}

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

Expr2Polynomial::Expr2Polynomial(const ExprNode &n){
	notMonomialDetected = false;
	n.acceptVisitor(*this);
}

Expr2Polynomial::~Expr2Polynomial(){
}

Polynomial * Expr2Polynomial::polynomial(){
	if(notMonomialDetected)
		return NULL;
	while(!stack.empty()){
		poly.addMonomial(stack.top());
		stack.pop();
	}
	return new MonomialSum(poly);
}

void Expr2Polynomial::visit(const ExprNode&){
	notMonomialDetected = true;
}
void Expr2Polynomial::visit(const ExprIndex&){
	notMonomialDetected = true;
}
void Expr2Polynomial::visit(const ExprLeaf&){
	notMonomialDetected = true;
}
void Expr2Polynomial::visit(const ExprNAryOp&){
	notMonomialDetected = true;
}
void Expr2Polynomial::visit(const ExprBinaryOp&){
	notMonomialDetected = true;
}
void Expr2Polynomial::visit(const ExprUnaryOp& e){
	notMonomialDetected = true;
}

void Expr2Polynomial::visit(const ExprAdd& e){
	int before_stack_size = stack.size();
	e.left.acceptVisitor(*this);
	e.right.acceptVisitor(*this);
	if(!notMonomialDetected){
		for(int i=stack.size(); i>before_stack_size;--i){
			poly.addMonomial(stack.top());
			stack.pop();
		}
	}
}

void Expr2Polynomial::visit(const ExprMinus& e){
	int before_stack_size = stack.size();
	e.expr.acceptVisitor(*this);
	if(!notMonomialDetected){
		if(before_stack_size != stack.size()){
			Monomial top = stack.top();
			stack.pop();
			top.coef = -top.coef;
			stack.push(top);
		}else{
			notMonomialDetected = true;
		}
	}
}

void Expr2Polynomial::visit(const ExprSub& e){
	int before_stack_size = stack.size();
	e.left.acceptVisitor(*this);
	if(!notMonomialDetected && before_stack_size < stack.size()){
		poly.addMonomial(stack.top());
		stack.pop();
	}
	e.right.acceptVisitor(*this);
	if(!notMonomialDetected && before_stack_size < stack.size()){
		Monomial top = stack.top();
		stack.pop();
		top.coef = -top.coef;
		poly.addMonomial(top);
	}
}
void Expr2Polynomial::visit(const ExprMul& e){
	int before_stack_size = stack.size();
	e.left.acceptVisitor(*this);
	e.right.acceptVisitor(*this);
	if(before_stack_size != stack.size() - 2)
		notMonomialDetected = true;
	else if(!notMonomialDetected){
		Monomial top = stack.top();
		stack.pop();
		top.multiplyByMonomial(stack.top());
		stack.pop();
		stack.push(top);
	}
}

void Expr2Polynomial::visit(const ExprPower& e){
	int before_stack_size = stack.size();
	e.expr.acceptVisitor(*this);
	if(!notMonomialDetected){
		if(before_stack_size != stack.size()){
			Monomial top = stack.top();
			stack.pop();
			if(top.coef.lb() == 1 && top.coef.ub() == 1 && top.variables.size() == 1){
				top.degrees[0] *= e.expon;
				stack.push(top);
			}else{
				notMonomialDetected = true;
			}
		}
		else{
			notMonomialDetected = true;
		}
	}
}
void Expr2Polynomial::visit(const ExprConstant& e){
	stack.push(Monomial(e.get_value()));
}

void Expr2Polynomial::visit(const ExprSymbol& e){
	stack.push(Monomial(&e));
}

void Expr2Polynomial::visit(const ExprSqr& e){
	int before_stack_size = stack.size();
	e.expr.acceptVisitor(*this);
	if(!notMonomialDetected){
		if(before_stack_size != stack.size()){
			Monomial top = stack.top();
			stack.pop();
			if(top.coef.lb() == 1 && top.coef.ub() == 1 && top.variables.size() == 1){
				top.degrees[0] *= 2;
				stack.push(top);
			}else{
				notMonomialDetected = true;
			}
		}
		else{
			notMonomialDetected = true;
		}
	}
}


ExprProperty function_property( const Function &f){
	Expr2Polynomial visitor(f.expr());
	Polynomial * p = visitor.polynomial();
	if(p == NULL){
		return ExprProperty(UNDEFINED_CONVEXITY, UNDEFINED_ORDERING);
	}
	Polynomial * quadra = p->quadra_polynomial();
	pair<IntervalMatrix,IntervalVector> matrixRepr = quadra->quadra_form();
	ExprProperty prop;
	if(quadra->degree()==2){
		if(is_definite_positive(matrixRepr.first)){
			prop.addProperty(ExprProperty(CONVEX, NOT_INCREASING_OR_DECREASING));
		}else{
			MonomialSum s;
			for(int i=0; i< quadra->size(); ++i){
				Monomial m = quadra->getMonomial(i);
				m.coef = -m.coef;
				s.addMonomial(m);
			}
			matrixRepr = s.quadra_form();
			if(is_definite_positive(matrixRepr.first)){
				prop.addProperty(ExprProperty(CONCAVE, NOT_INCREASING_OR_DECREASING));
			}else{
				prop.addProperty(ExprProperty(NOT_CONVEX_OR_CONCAVE, NOT_INCREASING_OR_DECREASING));
			}
		}
	}
	delete quadra;
	delete p;
	return prop;
}





}
