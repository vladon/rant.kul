/**

./src/kul/scm/Git.cpp

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

void kul::scm::Git::co(const std::string& d, const std::string& r, const std::string& v) const throw(Exception){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	if(!os::isDir(d)) os::mkDir(d);
	p->setDir(d.c_str());
	p->addArg("clone").addArg(r.c_str());
	if(v.compare("master") != 0 && v.compare("") != 0) p->addArg("-b").addArg(v.c_str());
	try{
		p->addArg(".");
		std::cout << "PERFORMING: " << p->toString() << std::endl;
		p->start();
	}catch(const kul::proc::ExitException& e){
		os::dirDel(d);
		KEXCEPT(Exception, "SCM ERROR - Check remote dependency location / version");
	}			
}		
void kul::scm::Git::up(const std::string& d, const std::string& r, const std::string& v) const throw(Exception){
	
	if(!kul::os::isDir(d)) co(d, r);
	else{
		std::shared_ptr<kul::Process> p(kul::Process::create("git"));
		p->setDir(d.c_str());
		p->addArg("pull");
		if(v.compare("") != 0) p->addArg("-u").addArg("origin").addArg(v.c_str());
		try{
			std::cout << "PERFORMING: " << p->toString() << std::endl;
			p->start();
		}catch(const kul::proc::ExitException& e){
			KEXCEPT(Exception, "SCM ERROR - Check remote dependency location / version");
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
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
	return pc.outs()[0];
}
const std::string kul::scm::Git::remoteVersion(const std::string& d, const std::string& url, const std::string branch) throw(Exception){
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("ls-remote").addArg(url).addArg(branch);
	try{		
		p->setDir(d);
		p->start();		
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
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
		p->setDir(d.c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
	return kul::String::split(kul::String::split(pc.outs()[0], "	")[1], " ")[0];
}

bool kul::scm::Git::hasChanges(const std::string& d) const{
	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	kul::ProcessCapture pc(*p.get());
	p->addArg("status").addArg("-sb");
	try{
		p->setDir(d.c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
	return pc.outs().size() > 0;
}

void kul::scm::Git::setOrigin(const std::string& d, const std::string& r){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("remote").addArg("set-url").addArg(r);
	try{
		p->setDir(d.c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
}
void kul::scm::Git::status(const std::string& d){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("status");
	try{
		p->setDir(d.c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
}
void kul::scm::Git::diff(const std::string& d){

	std::shared_ptr<kul::Process> p(kul::Process::create("git"));
	p->addArg("diff");
	try{
		p->setDir(d.c_str());
		p->start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR" + std::string(e.what()));
	}
}
