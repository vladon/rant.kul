/**

./inc/kul/except.hpp

Created on: 15 Feb 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _KUL_EXCEPT_HPP_
#define _KUL_EXCEPT_HPP_

#include <stdexcept>

#include "kul/string.hpp"

namespace kul{

class Exception : public std::runtime_error{
	private:
		const char* f;
		const int l;
		Exception& operator=(const Exception& e){ return *this;}
	public:
#if defined(_WIN32) || defined(_WIN64)	// delete when M$ supports noexcept
		~Exception() /*noexcept*/{} 	// delete
		const char* what() const /*noexcept*/{ return std::runtime_error::what();} // delete when M$ supports noexcept
#else									// delete when M$ supports noexcept
		~Exception() noexcept{}
		const char* what() const noexcept{ return std::runtime_error::what();}
#endif									// delete when M$ supports noexcept
		Exception(const char*file, const int& line, std::string s) : std::runtime_error(s), f(file), l(line){}
		Exception(const Exception& e) : std::runtime_error(e.what()), f(e.f),  l(e.l){}

		const std::string debug() 	const { return std::string(std::string(f) + " : " + kul::String::toString(l) + " : " + std::string(std::runtime_error::what()));}
		const char* file() 			const { return f;}
		const int& line() 			const { return l;}
};


};
#endif /* _KUL_EXCEPT_HPP_ */
