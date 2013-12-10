/**

./src/kul/scm/Git.cpp

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

void kul::scm::Git::co(const std::string& l, const std::string& r, const std::string& v) const throw(Exception){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	if(!OS::isDir(l)) OS::mkDir(l);
	p->setDir(l.c_str());
	p->addArg("clone").addArg(r.c_str());
	if(v.compare("master") != 0 && v.compare("") != 0) p->addArg("-b").addArg(v.c_str());
	try{
		p->addArg(".");
		std::cout << "PERFORMING: " << p->toString() << std::endl;
		p->start();
	}catch(const kul::proc::ExitException& e){
		OS::dirDel(l);
		throw Exception(__FILE__, __LINE__, "SCM ERROR - Check remote dependency location / version");
	}			
}		
void kul::scm::Git::up(const std::string& l, const std::string& r, const std::string& v) const throw(Exception){
	
	if(!kul::OS::isDir(l)) co(l, r);
	else{
		std::shared_ptr<kul::Process> p(kul::Process::create("git"));
		p->setDir(l.c_str());
		p->addArg("pull");
		if(v.compare("") != 0) p->addArg("-u").addArg("origin").addArg(v.c_str());
		try{
			std::cout << "PERFORMING: " << p->toString() << std::endl;
			p->start();
		}catch(const kul::proc::ExitException& e){
			OS::dirDel(l);
			throw Exception(__FILE__, __LINE__, "SCM ERROR - Check remote dependency location / version");
		}
	}
}
const std::string kul::scm::Git::localVersion(const std::string& d) {
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("rev-parse").addArg("HEAD");
	try{
		p->setDir(d);
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
	return pc.outs()[0];
}
const std::string kul::scm::Git::remoteVersion(const std::string& url, const std::string branch) throw(Exception){
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("ls-remote").addArg(url).addArg(branch);
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}	
	std::string s(pc.outs()[0]);
	kul::String::trim(s);
	return s.substr(0, s.find("	"));
}

const std::string kul::scm::Git::origin(const std::string& d) {
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("remote").addArg("-v");
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
	return kul::String::split(kul::String::split(pc.outs()[0], "	")[1], " ")[0];
}

const bool kul::scm::Git::hasChanges(const std::string& d) const{
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("status").addArg("-sb");
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
	return pc.outs().size() > 0;
}

void kul::scm::Git::setOrigin(const std::string& d, const std::string& r){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("remote").addArg("set-url").addArg("r");
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
}
void kul::scm::Git::status(const std::string& d){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("status");
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
}
void kul::scm::Git::diff(const std::string& d){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("diff");
	try{
		p->setDir(kul::OS::pwd().c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
	}
}
