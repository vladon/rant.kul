/*
 * code.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_HPP_
#define _KUL_CODE_HPP_

#include "glog/logging.h"

#include "kul/except.hpp"

namespace kul{ namespace code{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};


};};
#endif /* _KUL_CODE_HPP_ */