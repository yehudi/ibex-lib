/*
 * ibex_FunctionConvexity.h
 *
 *  Created on: Mar 10, 2015
 *      Author: wolkom
 */

#ifndef SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_
#define SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_

#include "ibex_Function.h"
#include "ibex_ConvexityExprVisitor.h"

namespace ibex{


class FunctionConvexity{
public:
	Convexity getConvexity(const Function &f){
		ConvexityExprVisitor cev;
		f.expr().acceptVisitor(cev);
		return cev.getConvexity(cev._stack.top());
	}
};

}
#endif /* SRC_FUNCTION_IBEX_FUNCTIONCONVEXITY_H_ */
