/**

./inc/kul/os.hpp

Created on: 1 Jan 2013

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
#ifndef _KUL_OS_HPP_
#define _KUL_OS_HPP_

#include <string>
#include <vector>
#include <iostream>

#include "kul/except.hpp"

namespace kul{
namespace cpu{

unsigned int cores();
unsigned int threads();

} // END NAMESPACE cpu
namespace fs {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};
} // END NAMESPACE FileSystem


class File;

class Dir{
	private:
		Dir();
		std::string p;

		static const std::string LOCL(const std::string& a);
		static const std::string PRNT(const std::string& a);
	public:
		Dir(const std::string& p, bool m = false) throw(fs::Exception) : p(p) {
			if(!is() && m && !mk()) KEXCEPT(fs::Exception, "Invalid directory path provided");
		}
		Dir(const Dir& d) : p(d.p){}

		bool is() const;
		bool mk() const;
		void rm() const;
		bool root() const;

		const std::string join(const std::string& s) const;
		const std::string locl() const;
		const std::string& path() const{ return p;}
		const std::string  real() const;

		const Dir prnt() const;

		const std::vector<Dir> 	dirs(bool incHidden = false) const throw(fs::Exception);
		const std::vector<File> files(bool recursive = false) const throw(fs::Exception);

		static const std::string JOIN(const std::string& a, const std::string& b) { return a + SEP() + b; }
		static const std::string REAL(const std::string& a) throw(fs::Exception);
		static const std::string SEP();
		friend class File;

		Dir& operator=(const Dir& d) = default;
};

class File{
	private:
		std::string n;
		Dir d;

	public:
		File(const std::string& n, bool m = false) : n(n), d(Dir::PRNT(Dir::REAL(n)), m){
			if(n.find(d.path()) != std::string::npos)
				this->n = n.substr(d.path().size() + 1);
		}
		File(const std::string& n, const Dir d) : n(n), d(d){}
		File(const File& f) : n(f.n), d(f.d){}

		bool cp(const File& f) const;
		bool is() const;
		bool mv(const Dir& d) const;
		bool rm() const;

		const std::string& name() const { return n; }

		const std::string real() const throw (fs::Exception){
			return d.join(n);
		}

		const Dir& dir() const { return d; }

		File& operator=(const File& f) = default;
};


class Env{
public:
	static const char*		 	CWD();
	static bool					CWD(const char* c);
	static bool					CWD(const Dir& d);

	static const char*			GET(const char* c);
	static void					SET(const char* var, const char* val);

	static const char*		 	SEP();
};

namespace os{

int execReturn(const std::string& cmd);
int execNoPrintReturn(const std::string& cmd);
int exec(const std::string& cmd, char*const args[]);

const std::string newLine();

const kul::Dir userDir();
const kul::Dir userAppDir(const std::string& app);

} // END NAMESPACE os

} // END NAMESPACE kul
#endif /* _KUL_OS_HPP_ */
