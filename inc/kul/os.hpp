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

#include <vector>

#include "kul/except.hpp"
#include "kul/string.hpp"

namespace kul{
namespace cpu{

unsigned int cores();
unsigned int threads();

} // END NAMESPACE cpu
namespace fs {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};
} // END NAMESPACE FileSystem


class Dir;
class File;

namespace env{
const char*		 	CWD();
bool				CWD(const char* c);
bool				CWD(const Dir& d);

const char*			GET(const char* c);
void				SET(const char* var, const char* val);

const char*		 	SEP();
}

class Dir{
	private:
		Dir(){}
		std::string p;

		static const std::string LOCL(std::string s);
		static const std::string PRNT(const std::string& s){
			const std::string& p = s.substr(0, s.rfind(SEP()) + 1);
			return kul::Dir(p).root() ? p : s.substr(0, s.rfind(SEP()));
		}
		static const std::string MINI(const std::string& a){
			return a.find(env::CWD()) == std::string::npos ? a 
				: a.compare(env::CWD()) == 0 ? a.substr(std::string(env::CWD()).size()) 
				: a.substr(std::string(env::CWD()).size() + 1);
		}
	public:
		Dir(const std::string& p, bool m = false) throw(fs::Exception) : p(Dir::LOCL(p)) {
			if(m && !is() && !mk()) KEXCEPT(fs::Exception, "Invalid directory path provided");
		}
		Dir(const Dir& d) : p(d.p){}

		bool is() const;
		bool mk() const;
		void rm() const;
		bool root() const;

		const std::string join(const std::string& s) const{ 
			return p.size() == 0 ? s : root() ? path() + s : JOIN(path(), s); 
		}
		const std::string  locl() const { return LOCL(path()); }
		const std::string& path() const { return p;}
		const std::string  real() const { return REAL(path()); }
		const std::string  mini() const { return MINI(real()); }

		const Dir prnt() const { return Dir(PRNT(path())); }

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
		File(const std::string& n, bool m = false) : n(Dir::LOCL(n)){
			if(n.find(Dir::SEP()) != std::string::npos){
				this->d = Dir(n.substr(0, n.rfind(Dir::SEP())));
				this->n = this->n.substr(n.rfind(Dir::SEP()) + 1);
			}else
				try{
					d = Dir(Dir::PRNT(Dir::REAL(this->n)), m);
				}catch(const kul::fs::Exception& e){
					this->d = Dir(env::CWD());
				}
			if(this->n.find(d.path()) != std::string::npos)
				this->n = this->n.substr(d.path().size() + 1);
		}
		File(const std::string& n, const Dir& d) : n(n), d(d){}
		File(const std::string& n, const char* c) : n(n), d(c){}
		File(const std::string& n, const std::string& d1) : n(n), d(d1){}
		File(const File& f) : n(f.n), d(f.d){}

		bool cp(const File& f) const;
		bool is() const;
		bool mk() const;
		bool mv(const Dir& d) const;
		bool rm() const;

		const std::string& name() const { return n; }

		const std::string full() const { return Dir::JOIN(d.path(), n); }
		const std::string real() const { return Dir::JOIN(d.real(), n); }
		const std::string mini() const { return Dir::MINI(real()); }
		const Dir& dir() const { return d; }

		File& operator=(const File& f) = default;
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
