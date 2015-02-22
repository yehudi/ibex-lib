//============================================================================
//                                  I B E X
// File        : arith03.cpp
// Author      : Jordan Ninin
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Fev 28, 2013
// Last Update : Fev 28, 2013
//============================================================================

#include "ibex.h"
#include <time.h>
#include <vector>
#include <map>

using namespace std;
using namespace ibex;

Matrix secondOrderToMatrix(Function f){
	const ExprNode& root = f.expr();

	ConvexityExprVisitor exprOrder2 = ConvexityExprVisitor();
	std::vector<int> varsA = exprOrder2.varsA;
	std::vector<int> varsB = exprOrder2.varsB;
	std::vector<double> coefs = exprOrder2.coefs;

	int n_vars = f.nb_used_vars();
	int n_exprs = 3;//coefs.size();

	varsA.push_back(0);
	varsB.push_back(0);
	coefs.push_back(1.0);

	varsA.push_back(0);
	varsB.push_back(1);
	coefs.push_back(-1.0);

	varsA.push_back(1);
	varsB.push_back(1);
	coefs.push_back(1.0);

	Matrix M = Matrix(n_vars,n_vars,0.0);

	for(int i = 0; i<n_exprs;++i){
		if(varsA[i] == varsB[i])
			M[varsA[i]][varsB[i]] = coefs[i];
		else{
			M[varsA[i]][varsB[i]] = coefs[i]/2;
			M[varsB[i]][varsA[i]] = coefs[i]/2;
		}
	}

	return M;
}

bool isPositiveDefinite(Matrix M){
	double sum;
	//assert(M.nb_cols() == M.nb_rows());
	for(int i=0; i < M.nb_cols(); ++i){
		sum = M[i][i];
		for(int j=0; j < i; ++j)
			sum -= M[i][j]*M[j][i];
		if(sum <= 0.0)
			return false;
	}
	return true;
}

int main() {
	cout << "==========================================" << endl;
	cout << "==========================================" << endl;
	cout << "TEST 2: " << endl;

	const ExprSymbol& x = Variable("x");
	const ExprSymbol& y = Variable("y");

	const ExprSymbol& z = Variable("z");

	ConvexityExprVisitor cev = ConvexityExprVisitor();

	Function g(z,pow(z,2));
	g.expr().acceptVisitor(cev);

	Function f(x,y,pow(x,2) + pow(y,2) - x*y);
	f.expr().acceptVisitor(cev);

	Matrix M = secondOrderToMatrix(f);
	cout<<M<<endl;
	if(isPositiveDefinite(M))
		cout<<"Matrice definie positive"<<endl;
	else
		cout<<"Matrice pas definie positive"<<endl;

	return 0;
}

