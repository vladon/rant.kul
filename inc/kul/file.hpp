/*
 * file.hpp
 *
 *  Created on: 21 Jan 2013
 *      Author: philix
 */

#ifndef _FILE_HPP_
#define _FILE_HPP_

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
#endif /* _FILE_HPP_ */
