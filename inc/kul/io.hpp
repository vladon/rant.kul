/**

./inc/kul/io.hpp

Created on: 21 Jan 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul (The Kommon Usage Library).

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPosE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _KUL_IO_HPP_
#define _KUL_IO_HPP_

#include <time.h>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "kul/os.hpp"
#include "kul/except.hpp"

namespace kul{  namespace io {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class Reader{
	private:
		std::ifstream f;
		std::auto_ptr<std::string> string;
	public:
		Reader(const char* c) : f(c), string(0) { if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(c) + "\" not found");}
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
			if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(f) + "\" not found");
			if(!kul::os::isDir(kul::os::dirDotDot(f))) kul::os::mkDir(kul::os::dirDotDot(f));
			ofs.open(f);
		}
		~Writer() { ofs.close();}
		void write(const char*c, bool nl = false){ 
			if(nl) 	ofs << c << "\n";
			else	ofs << c;
		}
};

class LineTrimmer{
	public:
		LineTrimmer(const char*f){
			std::fstream fs;
			fs.open(f);
			std::string line;
			while(std::getline(fs, line)) {
				std::cout << line << std::endl;
				fs.seekp(-std::ios::off_type(line.size()) - 1, std::ios_base::cur);
				fs << 'x';
				fs.close();
			}
		}
};


};};
#endif /* _KUL_IO_HPP_ */

