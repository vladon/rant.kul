/*
 * function.hpp
 *
 *  Created on: 9 Feb 2013
 *      Author: philix
 */

#include "kul/xml.hpp"

#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_

namespace kul{ namespace xml{


class NodeTextDateFunction : public NodeTextFunction{
	private:
		const char d; // can be a function itself.
		class NodeTextDateNowFunction{};
		class NodeTextDateLatestFunction{};
	public:
		NodeTextDateFunction(const char* d) : NodeTextFunction("date") , d(d){}
		const char* txt() { return "today"; }
};


};};
#endif /* _FUNCTION_HPP_ */
