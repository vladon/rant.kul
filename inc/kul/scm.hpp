/**

./inc/kul/scm.hpp

Created on: 24 Jan 2013

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
#ifndef _KUL_SCM_HPP_
#define _KUL_SCM_HPP_

#include "glog/logging.h"
 
#include "kul/os.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"

namespace kul{ namespace scm{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ScmNotFoundException : public kul::Exception{
	public:
		ScmNotFoundException(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class Scm{
	protected:
		Scm(){}
	public:
		virtual ~Scm(){}
		virtual void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0;
		virtual void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0;
		virtual const std::string& localVersion() const = 0;
		virtual const std::string& remoteVersion(const std::string& url, const std::string branch = "") const throw(Exception) = 0;
};

class Git : public Scm{
	private:
		std::string lVersion;
		std::string rVersion;
		void captureLocalVersion(std::string s) const {
			const_cast<Git*>(this)->lVersion = s;
		}
		void captureRemoteVersion(std::string s) const{
			const_cast<Git*>(this)->rVersion = s;
		}
	public:
		void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			
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
		
		void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			
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
		const std::string& localVersion() const{
			std::shared_ptr<kul::Process> p(kul::Process::create("git"));
			p->addArg("rev-parse").addArg("HEAD");
			try{
				p->setDir(kul::OS::pwd().c_str());
				std::function<void(std::string)> fun(std::bind(&Git::captureLocalVersion, std::ref(*this), std::placeholders::_1));
				p->setOut(fun);
				p->start();
			}catch(const kul::proc::ExitException& e){
				throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
			}
			return this->lVersion;
		}
		const std::string& remoteVersion(const std::string& url, const std::string branch = "") const throw(Exception){
			std::shared_ptr<kul::Process> p(kul::Process::create("git"));
			p->addArg("ls-remote").addArg(url).addArg(branch);
			try{
				p->setDir(kul::OS::pwd().c_str());
				std::function<void(std::string)> fun(std::bind(&Git::captureRemoteVersion, std::ref(*this), std::placeholders::_1));
				p->setOut(fun);
				p->start();
			}catch(const kul::proc::ExitException& e){
				throw Exception(__FILE__, __LINE__, "SCM ERROR" + std::string(e.what()));
			}
			return this->rVersion;
		}
};

class Svn : public Scm{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			LOG(INFO) << "SVN UP";
			throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
		};
		void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			LOG(INFO) << "SVN UP";
			throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
			if(!kul::OS::isDir(l)) co(l, r);
			else{}
		};
		const std::string& localVersion() const{
			throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
		}
		const std::string& remoteVersion(const std::string& url, const std::string branch = "") const throw(Exception){
			throw Exception(__FILE__, __LINE__, "SCM ERROR - SVN NOT YET IMPLEMENTED");
		}
};

class Manager{
	private:
		Manager(){
			// add scms to map
			scms.insert(std::pair<std::string, Scm*>("git", new Git()));
			scms.insert(std::pair<std::string, Scm*>("svn", new Svn()));
		}
		static Manager* instance;
		hash::map::S2T<Scm*> scms;
	public:
		static Manager* INSTANCE(){ if(instance == 0) instance = new Manager(); return instance;}
		const Scm* get(const std::string& s) throw(ScmNotFoundException){
			if(Manager::scms.count(s) > 0)
				return (*Manager::scms.find(s)).second;
			throw ScmNotFoundException(__FILE__, __LINE__, "Compiler for " + s + " is not implemented");
		}
};




};};
#endif /* _KUL_SCM_HPP_ */
