/* ============================================================================
 * I B E X - Diff Tests
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Gilles Chabert
 * Created     : Apr 02, 2012
 * ---------------------------------------------------------------------------- */

#ifndef __TEST_GRADIENT_H__
#define __TEST_GRADIENT_H__

#include "cpptest.h"
#include "utils.h"
#include "ibex_Expr.h"

namespace ibex {

class TestGradient : public TestIbex {

public:
	TestGradient() {
		TEST_ADD(TestGradient::deco01);
		TEST_ADD(TestGradient::deco02);

		TEST_ADD(TestGradient::add01);
		TEST_ADD(TestGradient::add02);
		TEST_ADD(TestGradient::add03);
		TEST_ADD(TestGradient::add04);
		//TEST_ADD(TestGradient::add05);
		TEST_ADD(TestGradient::dist);
		TEST_ADD(TestGradient::jac01);
		TEST_ADD(TestGradient::jac02);
		TEST_ADD(TestGradient::hansen01);
		TEST_ADD(TestGradient::mulVV);
		TEST_ADD(TestGradient::transpose01);
		TEST_ADD(TestGradient::mulMV01);
		TEST_ADD(TestGradient::mulVM01);
		TEST_ADD(TestGradient::mulVM02);
	}

	void deco01();
	void deco02();

	void add01();
	void add02();
	void add03();
	void add04();
	void dist();
	void jac01();
	void jac02();
	void hansen01();

	void mulVV();
	// for vectors
	void transpose01();
	// with constant matrix
	void mulMV01();
	void mulVM01();
	void mulVM02();
private:
	void check_deco(const ExprNode& e);
};

} // end namespace

#endif // __TEST_GRADIENT_H__
