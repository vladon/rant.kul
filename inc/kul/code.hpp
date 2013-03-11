/*
 * code.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _CODE_HPP_
#define _CODE_HPP_

#include <stdexcept>

#include "kul/except.hpp"

namespace kul{ namespace code{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class CPP{
	public:
		static const std::string sharedLibrary(const std::string& s);
		static const std::string staticLibrary(const std::string& s);
};


};};
#endif /* _CODE_HPP_ */
