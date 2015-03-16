//============================================================================
//                                  I B E X
// File        : TestConvexity.h
// Author      : Yehudi Hollvoet
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : Mar 11, 2015
// Last Update : Mar 11, 2015
//============================================================================

#ifndef TEST_CONVEXITY_H_
#define TEST_CONVEXITY_H_

#include "cpptest.h"
#include "utils.h"
#include "ibex_ConvexityExprVisitor.h"
#include "ibex_FunctionConvexity.h"

using namespace ibex;

class TestConvexity : public TestIbex {
public:
	TestConvexity() {
		TEST_ADD(TestConvexity::choleskyDecomposition);
		TEST_ADD(TestConvexity::LinearFunctionConvexity);
		TEST_ADD(TestConvexity::quadraticFunctionConvexity);
		TEST_ADD(TestConvexity::specialFunctionConvexity);
	}
private:
	void choleskyDecomposition();
	void LinearFunctionConvexity();
	void quadraticFunctionConvexity();
	void specialFunctionConvexity();
};

#endif // TEST_CONVEXITY


