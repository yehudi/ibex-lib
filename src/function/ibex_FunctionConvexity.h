/* ============================================================================
 * I B E X - Function Convexity Analyser
 * ============================================================================
 * Copyright   : Ecole des Mines de Nantes (FRANCE)
 * License     : This program can be distributed under the terms of the GNU LGPL.
 *               See the file COPYING.LESSER.
 *
 * Author(s)   : Yehudi Hollevoet
 * Created     : Mar 10, 2015
 * ---------------------------------------------------------------------------- */

#ifndef SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_
#define SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_

#include "ibex_Function.h"
#include "ibex_ConvexityExprVisitor.h"

namespace ibex{

/**
 * \brief Function convexity analyser
 */
class FunctionConvexity{
public:

	/**
	 * \brief get the convexity property of a function
	 */
	Convexity getConvexity(const Function &f){
		ConvexityExprVisitor cev;
		f.expr().acceptVisitor(cev);
		return cev.getConvexity(cev._stack.top());
	}
};

}
#endif /* SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_ */
