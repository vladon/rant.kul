/*
 * except.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _EXCEPT_HPP_
#define _EXCEPT_HPP_

#include <stdexcept>

#include "kul/string.hpp"

namespace kul{

class Exception : public std::runtime_error{
	private:
		const char* f;
		const int l;
		Exception& operator=(const Exception& e){ return *this;}
	public:
		~Exception() noexcept{}
		Exception(const char*file, const int line, std::string s) : std::runtime_error(s), f(file), l(line){}
		Exception(const Exception& e) : std::runtime_error(e.what()), f(e.f),  l(e.l){}
		const char* what() const noexcept{ return std::runtime_error::what();}
		const std::string debug() const {return std::string(std::string(f) + " : " + kul::st_d::string::toString(l) + " : " + std::string(std::runtime_error::what()));}
		const char* file() const { return f;}
		const int& line() const { return l;}
};


};
#endif /* _EXCEPT_HPP_ */
