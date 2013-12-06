/**

./inc/kul/file.hpp

Created on: 21 Jan 2013

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
#ifndef _KUL_FILE_HPP_
#define _KUL_FILE_HPP_

#include <time.h>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "kul/os.hpp"
#include "kul/except.hpp"

namespace kul{  namespace file {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class Reader{
	private:
		std::ifstream f;
		std::auto_ptr<std::string> string;
	public:
		Reader(const char* c) : f(c), string(0) { if(!f) throw Exception(__FILE__, __LINE__, "FileException : file \"" + std::string(c) + "\" not found");}
		std::string* read(){
			string.reset(0);
			std::string s;
			if(f.good()){
				getline(f, s);
				string.reset(new std::string(s));
			}
			return string.get();
		}
};

class Writer{
	private:
		std::ofstream ofs;
	public:
		Writer(const char*f)  {
			if(!f) throw Exception(__FILE__, __LINE__, "FileException : file \"" + std::string(f) + "\" not found");
			if(!kul::OS::isDir(kul::OS::dirDotDot(f))) kul::OS::mkDir(kul::OS::dirDotDot(f));
			ofs.open(f);
		}
		~Writer() { ofs.close();}
		void write(const char*c, bool nl = false){ 
			if(nl) 	ofs << c << "\n";
			else	ofs << c;
		}
};

};};
#endif /* _KUL_FILE_HPP_ */
