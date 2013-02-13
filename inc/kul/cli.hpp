/*
 * cli.hpp
 *
 *  Created on: 13 Feb 2013
 *      Author: philix
 */

#ifndef _CLI_HPP_
#define _CLI_HPP_

#include "string"

#include "kul/smart.hpp"
#include "kul/ext/google.hpp"

namespace kul{ namespace cli {

class Arg;

class Args{

	private:
		kul::smart::Array<Arg> args;

	public:
		kul::smart::ArrayHelper<Arg>	add()				{ return ArrayHelper<Arg>(args); }
};

class Arg{
	private:
		bool m;
		std::string n;
	public:
		Arg(std::string n, bool m) : n(n), m(m){}
		bool isMandatory() { return m;}
};





};};
#endif /* _CLI_HPP_ */
