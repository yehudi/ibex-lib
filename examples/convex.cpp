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

int main() {
	cout << "==========================================" << endl;
	cout << "==========================================" << endl;
	cout << "TEST 2 : " << endl;

	const ExprSymbol& x = Variable("x");
	const ExprSymbol& y = Variable("y");

	const ExprSymbol& z = Variable("z");

	ConvexityExprVisitor cev = ConvexityExprVisitor();

	Function g(z,pow(z,2));
	//g.expr().acceptVisitor(cev);

	cout << g <<endl;

	Function f(x,y,pow(x,2) + pow(y,2) - x*y);
	f.expr().acceptVisitor(cev);

	cout<< f <<endl;
	return 0;
}

