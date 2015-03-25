/**

./src/Git.cpp

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
	Dir dr(d, true);
	kul::Process p("git", d);
	p.addArg("clone").addArg(r);
	if(!v.empty()) p.addArg("-b").addArg(v);
	try{
		p.addArg(".");
		std::cout << "PERFORMING: " << p.toString() << std::endl;
		p.start();
	}catch(const kul::proc::ExitException& e){
		dr.rm();
		KEXCEPT(Exception, "SCM ERROR - Check remote dependency location / version");
	}			
}		
void kul::scm::Git::up(const std::string& d, const std::string& r, const std::string& v) const throw(Exception){
	if(!Dir(d).is()) co(d, r, v);
	else{
		kul::Process p("git", d);
		p.addArg("pull");
		if(!v.empty()) p.addArg("-u").addArg("origin").addArg(v);
		try{
			std::cout << "PERFORMING: " << p.toString() << std::endl;
			p.start();
		}catch(const kul::proc::ExitException& e){
			KEXCEPT(Exception, "SCM ERROR - Check remote dependency location / version");
		}
	}
}
const std::string kul::scm::Git::localVersion(const std::string& d) const{
	kul::Process p("git", d);
	kul::ProcessCapture pc(p);
	try{
		p.addArg("rev-parse").addArg("HEAD").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	if(pc.outs().empty()) KEXCEPT(Exception, "SCM ERROR: Directory may not be git repository : " + d);
	return pc.outs()[0];
}
const std::string kul::scm::Git::remoteVersion(const std::string& d, const std::string& url, const std::string& branch) const throw(Exception){
	kul::Process p("git", d);
	kul::ProcessCapture pc(p);
	try{		
		p.addArg("ls-remote").addArg(url).addArg(branch).start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	if(pc.outs().empty()) KEXCEPT(Exception, "SCM ERROR URL OR BRANCH MAY NOT EXIST: " + url + " / " + branch);
	std::string s(pc.outs()[0]);
	kul::String::trim(s);
	return s.substr(0, s.find("	"));
}
const std::string kul::scm::Git::origin(const std::string& d) const{
	kul::Process p("git", d);
	kul::ProcessCapture pc(p);
	try{
		p.addArg("remote").addArg("-v").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	if(pc.outs().empty()) KEXCEPT(Exception, "SCM ERROR: Directory may not be git repository : " + d);
	return kul::String::split(kul::String::split(pc.outs()[0], "	")[1], " ")[0];
}
bool kul::scm::Git::hasChanges(const std::string& d) const{
	kul::Process p("git", d);
	kul::ProcessCapture pc(p);
	try{
		p.addArg("status").addArg("-sb").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	return pc.outs().size() > 0;
}
void kul::scm::Git::setOrigin(const std::string& d, const std::string& r) const{
	kul::Process p("git", d);
	try{
		p.addArg("remote").addArg("set-url").addArg(r).start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
}
void kul::scm::Git::status(const std::string& d) const{
	kul::Process p("git", d);
	try{
		p.addArg("status").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
}
void kul::scm::Git::diff(const std::string& d) const{
	kul::Process p("git", d);
	p.addArg("diff");
	try{
		p.start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
}
