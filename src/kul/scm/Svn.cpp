/**

./src/kul/scm/Svn.cpp

Created on: 6 Dec 2013

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
#include "kul/scm.hpp"
#include "kul/log.hpp"

void kul::scm::Svn::co(const std::string& l, const std::string& r, const std::string&) const throw(Exception){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
};
void kul::scm::Svn::up(const std::string& l, const std::string& r, const std::string&) const throw(Exception){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
	if(!kul::OS::isDir(l)) co(l, r);
	else{}
};
const std::string kul::scm::Svn::localVersion(const std::string& d) {
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}
const std::string kul::scm::Svn::remoteVersion(const std::string& d, const std::string& url, const std::string branch) throw(Exception){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}
const std::string kul::scm::Svn::origin(const std::string& d) {
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}

const bool kul::scm::Svn::hasChanges(const std::string& d) const{
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}

void kul::scm::Svn::setOrigin(const std::string& d, const std::string& r){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}
void kul::scm::Svn::status(const std::string& d){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}
void kul::scm::Svn::diff(const std::string& d){
	throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
}