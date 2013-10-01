/*
 * scm.hpp
 *
 *  Created on: 24 Jan 2013
 *      Author: philix
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
		virtual void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0 ;
		virtual void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0 ;
};

class Git : public Scm{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			LOG(INFO) << "GIT CO";
			std::shared_ptr<kul::proc::Process> p(kul::proc::Process::create("git"));
			if(!OS::isDir(l)) OS::mkDir(l);
			(*p).setDir(l.c_str()).addArg("clone").addArg(r.c_str());
			if(v.compare("master") != 0 && v.compare("") != 0) (*p).addArg("-b").addArg(v.c_str());
			try{
				(*p).addArg(".").start();
			}catch(const kul::proc::ExitException& e){
				OS::dirDel(l);
				throw Exception(__FILE__, __LINE__, "SCM ERROR - Check remote dependency location / version");
			}
			
		};
		void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception){
			LOG(INFO) << "GIT UP";
			if(!kul::OS::isDir(l)) co(l, r);
			else{}
		};
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
};

class Manager{
	private:
		Manager(){
			// add scms to map
			scms.insert(std::pair<std::string, Scm*>("git", new Git()));
			scms.insert(std::pair<std::string, Scm*>("svn", new Svn()));
		}
		static Manager* instance;
		kul::ext::goo_gle::StringToTGMap<Scm*> scms;
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
