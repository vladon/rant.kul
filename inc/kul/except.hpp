/**

./inc/kul/except.hpp

Created on: 15 Feb 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul (The Kommon Usage Library).

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _KUL_EXCEPT_HPP_
#define _KUL_EXCEPT_HPP_

#include <sstream>
#include <stdexcept>

#include <kul/def.hpp>

namespace kul{

class Exception : public std::runtime_error{
	private:
		const char* f;
		const int l;
		const std::exception_ptr ep;
	public:
		~Exception() KNOEXCEPT{}
		Exception(const char*f, const int& l, const std::string& s) : std::runtime_error(s), f(f), l(l), ep(std::current_exception()){}
		Exception(const Exception& e) : std::runtime_error(e.what()), f(e.file()),  l(e.line()), ep(e.ep) {}

		const std::string debug() 			const { return std::string(std::string(f) + " : " + std::to_string(l) + " : " + std::string(what()));}
		const char* file() 					const { return f;}
		const int& line() 					const { return l;}
		const std::exception_ptr& cause() 	const { return ep;}
		const std::string stack()	const {
			std::stringstream ss;
			if(ep){
				try{  								std::rethrow_exception(ep); 	}
				catch(const kul::Exception& e){     ss << e.stack() << std::endl;	}
				catch(const std::exception& e){		ss << e.what() << std::endl;	}
				catch(...){							ss << "UNKNOWN EXCEPTION TYPE" << std::endl; }
			}
			ss << debug();
			return ss.str();
		}
};

#define KEXCEPT(c, m) throw c(__FILE__, __LINE__, m)
#define KEXCEPTION(m) throw Exception(__FILE__, __LINE__, m)

}
#endif /* _KUL_EXCEPT_HPP_ */
