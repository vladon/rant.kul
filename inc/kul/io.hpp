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

#include <memory>
#include <time.h>
#include <fstream>
#include <stdexcept>

#include "kul/os.hpp"
#include "kul/except.hpp"
#include "kul/string.hpp"

namespace kul{  namespace io {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class AReader{
	private:
		std::unique_ptr<std::string> str;
	protected:
		const std::string* readLine(std::ifstream& f){
			str.reset();
			std::string s;
			if(f.good()){
				std::vector<char> v;
				v.resize(512);
				f.getline(&v[0], 512);
				v.resize(f.gcount());
				str = std::make_unique<std::string>(std::string(v.begin(), v.end()));
			}
			return str.get();
		}
		const std::string* read(std::ifstream& f, const uint& s){
			str.reset();
			if(f.good()){
				std::vector<char> v;
				std::stringstream ss;
				v.resize(s);
				f.read(&v[0], s);
				v.resize(f.gcount());
				str = std::make_unique<std::string>(std::string(v.begin(), v.end()));
			}
			return str.get();
		}
};
class Reader : public AReader{
	private:
		std::ifstream f;
	public:
		Reader(const char* c) : f(c, std::ios::in) { 
			if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(c) + "\" not found");
		}
		~Reader() { f.close();}
		const std::string* readLine(){
			return AReader::readLine(f);
		}
		const std::string* read(const uint& s){
			return AReader::read(f, s);
		}
};
class BinaryReader : public AReader{
	private:
		std::ifstream f;
	public:
		BinaryReader(const char* c) : f(c, std::ios::in |std::ios::binary){
			if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(c) + "\" not found");
		}
		~BinaryReader() { f.close();}
		const std::string* readLine(){
			return AReader::readLine(f);
		}
		const std::string* read(const uint& s){
			return AReader::read(f, s);
		}
};


class AWriter{
	protected:
		void write(std::ofstream& f, const char*c, bool nl){ 
			if(nl) 	f << c << kul::os::newLine();
			else	f << c;
		}
};
class Writer: public AWriter{
	private:
		std::ofstream f;
	public:
		Writer(const char*c, bool a = 0){ 
			if(a) f.open(c, std::ios::out | std::ios::app);
			else  f.open(c, std::ios::out);
			if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(c) + "\" not found");
		}
		~Writer() { f.close();}
		void write(const char*c, bool nl = false){
			return AWriter::write(f, c, nl);
		}
		template<class T> Writer& operator<<(const T& s){
			f << s;
			return *this;
		}
};
class BinaryWriter : public AWriter{
	private:
		std::ofstream f;
	public:
		BinaryWriter(const char* c) : f(c, std::ios::out |std::ios::binary){ 
			if(!f) KEXCEPT(Exception, "FileException : file \"" + std::string(c) + "\" not found");
			f.unsetf(std::ios_base::skipws);
		}
		~BinaryWriter() { f.close();}
		void write(const char*c, bool nl = false){
			return AWriter::write(f, c, nl);
		}
		template<class T> BinaryWriter& operator<<(const T& s){
			f << s;
			return *this;
		}
};

}}
#endif /* _KUL_IO_HPP_ */

