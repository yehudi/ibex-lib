//============================================================================
//                                  I B E X
// File        : TestConvexity.h
// Author      : Yehudi Hollevoet
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Mar 16, 2015
// Last Update : Mar 16, 2015
//============================================================================

#include "TestConvexity.h"

using namespace std;
using namespace ibex;

void TestConvexity::LinearFunctionConvexity(){
	FunctionConvexity conv;
	const ExprSymbol& x1 = Variable("x");
	Function f1(x1,x1+1);
	TEST_ASSERT(conv.getConvexity(f1) == CONVEX_AND_CONCAVE);

	//more than one variable
	const ExprSymbol& x2 = Variable("x");
	const ExprSymbol& y2 = Variable("y");
	Function f2(x2,y2,x2-y2+1);
	TEST_ASSERT(conv.getConvexity(f2) == CONVEX_AND_CONCAVE);

	//multiplication by constants
	const ExprSymbol& x3 = Variable("x");
	const ExprSymbol& y3 = Variable("y");
	Function f3(x3,y3,x3-2*y3+1);
	TEST_ASSERT(conv.getConvexity(f3) == CONVEX_AND_CONCAVE);

	//multiplication by constants + distributivity
	const ExprSymbol& x4 = Variable("x");
	const ExprSymbol& y4 = Variable("y");
	Function f4(x4,y4,-4*(x4-2*y4+1));
	TEST_ASSERT(conv.getConvexity(f4) == CONVEX_AND_CONCAVE);

	//addition of same variable and sum = 0
	const ExprSymbol& x5 = Variable("x");
	Function f5(x5,9*(2*x5+x5-3*x5));
	TEST_ASSERT(conv.getConvexity(f5) == CONVEX_AND_CONCAVE);

	//addition of same variable and sum != 0
	const ExprSymbol& x6 = Variable("x");
	Function f6(x6,9*(2*x6+x6-4*x6));
	TEST_ASSERT(conv.getConvexity(f6) == CONVEX_AND_CONCAVE);

	//multiplication of different degrees totaling 1 and 0
	const ExprSymbol& x7 = Variable("x");
	Function f7(x7, pow(x7, 2) * pow(x7, -2) + pow(x7, 2) * pow(x7, -1));
	TEST_ASSERT(conv.getConvexity(f7) == CONVEX_AND_CONCAVE);

}

void TestConvexity::quadraticFunctionConvexity(){
	FunctionConvexity conv;

	//single variable
	const ExprSymbol& x1 = Variable("x");
	Function f1(x1,pow(x1,2));
	TEST_ASSERT(conv.getConvexity(f1) == CONVEX);

	const ExprSymbol& x2 = Variable("x");
	Function f2(x2,pow(x2,2) + x2 + 2);
	TEST_ASSERT(conv.getConvexity(f2) == CONVEX);
	//concave
	const ExprSymbol& x2b = Variable("x");
	Function f2b(x2b,-(pow(x2b,2) + x2b + 2));
	TEST_ASSERT(conv.getConvexity(f2b) == CONCAVE);

	//two variables
	//convex
	const ExprSymbol& x3 = Variable("x");
	const ExprSymbol& y3 = Variable("y");
	Function f3(x3, y3, pow(x3,2) + x3*y3 + pow(y3, 2));
	TEST_ASSERT(conv.getConvexity(f3) == CONVEX);

	//not convex or concave
	const ExprSymbol& x4 = Variable("x");
	const ExprSymbol& y4 = Variable("y");
	Function f4(x4, y4, pow(x4,2) - 8*x4*y4 + pow(y4, 2));
	TEST_ASSERT(conv.getConvexity(f4) == NOT_CONVEX_OR_CONCAVE);

	//with associativity
	const ExprSymbol& x5 = Variable("x");
	const ExprSymbol& y5 = Variable("y");
	Function f5(x5, y5, x5*(x5 + 2*y5) + pow(y5, 2));
	TEST_ASSERT(conv.getConvexity(f5) == CONVEX);

}

void TestConvexity::specialFunctionConvexity(){
	FunctionConvexity conv;

	//trigonometry
	const ExprSymbol& x1 = Variable("x");
	Function f1(x1,ibex::sin(x1));
	TEST_ASSERT(conv.getConvexity(f1) == NOT_CONVEX_OR_CONCAVE);

	const ExprSymbol& x2 = Variable("x");
	Function f2(x2,ibex::cos(x2));
	TEST_ASSERT(conv.getConvexity(f2) == NOT_CONVEX_OR_CONCAVE);

	const ExprSymbol& x3 = Variable("x");
	Function f3(x3,ibex::tan(x3));
	TEST_ASSERT(conv.getConvexity(f3) == NOT_CONVEX_OR_CONCAVE);
}

void TestConvexity::choleskyDecomposition(){
	double test1[] = {1.0,0.5,0.5,1.0};
	CholeskyDecomposition d1(Matrix(2,2,test1));
	TEST_ASSERT(d1.existsUniqueDecomposition());
	double test2[] = {1.0,-0.5,-0.5,1.0};
	CholeskyDecomposition d2(Matrix(2,2,test2));
	TEST_ASSERT(d2.existsUniqueDecomposition());
	double test3[] = {1.0,4,4,1.0};
	CholeskyDecomposition d3(Matrix(2,2,test3));
	TEST_ASSERT(!d3.existsUniqueDecomposition());
	Matrix m = Matrix(2,2,test1);
	Matrix l = d1.getUniqueDecomposition();
	Matrix mbis = l*l.transpose();

	//For the moment we test with floats to remove the tiny calculation errors
	for(int i=0;i<m.nb_cols();++i){
		for(int j=0; j<m.nb_rows();++j){
			TEST_ASSERT(((float) m[i][j]) == ((float) mbis[i][j]));
		}
	}

	//TEST_ASSERT(m == (l*l.transpose())); //===========> Propagation errors due to square root calculation
}
