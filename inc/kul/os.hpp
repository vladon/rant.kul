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
#include <stdexcept>

#include "kul/except.hpp"

namespace kul { namespace os {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

int execReturn(const std::string& cmd);
int execNoPrintReturn(const std::string& cmd);
int exec(const std::string& cmd, char*const args[]);

const char* 	getEnvVar(const char* c);
void			setEnvVar(const char* var, const char* val);

bool isFile(const std::string& s);
bool delFile(const std::string& s);
bool isDir(const std::string& s);

bool mkFile(const std::string& s);
bool cpFile(const std::string& s, const std::string& d);
bool mkDir(const std::string& s);

bool mv(const std::string& a, const std::string& b);

const bool 			dirSet(const std::string& nwd); // set the cwd / USE WITH CAUTION!
const bool 			dirDel(const std::string& dir); // deletes if exists
const std::string 	dirExp(const std::string& nwd, const std::string& dir) throw (Exception); // Expand relative directories
const std::string 	dirSep();
const std::string 	dirJoin(const std::string& a, const std::string& b);
const std::string 	dirDotDot(const std::string& s);
const std::string 	pathSep();

const std::string	localPath(const std::string& s);

std::string pwd();
std::vector<std::string> dirs(std::string s = pwd(), bool incHidden = false) throw(Exception);
std::vector<std::string> files(std::string s = pwd(), bool recursive = false) throw(Exception);

std::string userDir();
std::string userAppDir(const std::string& app);

std::string newLine();

const unsigned int cores();
const unsigned int threads();

};};
#endif /* _KUL_OS_HPP_ */
