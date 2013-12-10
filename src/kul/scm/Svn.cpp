/**

./src/kul/scm/Svn.cpp

Created on: 6 Dec 2013

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
const std::string kul::scm::Svn::remoteVersion(const std::string& url, const std::string branch) throw(Exception){
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