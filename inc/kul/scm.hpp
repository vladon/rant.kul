/**

./inc/kul/scm.hpp

Created on: 24 Jan 2013

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
#ifndef _KUL_SCM_HPP_
#define _KUL_SCM_HPP_
 
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
		const std::string type() { return typeid(*this).name(); }
		virtual void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0;
		virtual void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception) = 0;
		virtual const std::string origin(const std::string& d) = 0;
		virtual const std::string localVersion(const std::string& d) = 0;
		virtual const std::string remoteVersion(const std::string& d, const std::string& url, const std::string branch = "")  throw(Exception) = 0;

		virtual const bool hasChanges(const std::string& d) const = 0;

		virtual void setOrigin(const std::string& d, const std::string& r) = 0;
		virtual void status(const std::string& d) = 0;
		virtual void diff(const std::string& d) = 0;
};

class Git : public Scm{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception);		
		void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception);
		const std::string origin(const std::string& d);
		const std::string localVersion(const std::string& d);
		const std::string remoteVersion(const std::string& d, const std::string& url, const std::string branch = "") throw(Exception);

		const bool hasChanges(const std::string& d) const;

		void setOrigin(const std::string& d, const std::string& r);
		void status(const std::string& d);
		void diff(const std::string& d);
};

// SVN URL CHANGE: svn switch –relocate  <from URL> <to URL>
class Svn : public Scm{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception);
		void up(const std::string& l, const std::string& r, const std::string& v = "") const throw(Exception);
		const std::string origin(const std::string& d);
		const std::string localVersion(const std::string& d);
		const std::string remoteVersion(const std::string& d, const std::string& url, const std::string branch = "") throw(Exception);

		const bool hasChanges(const std::string& d) const;

		void setOrigin(const std::string& d, const std::string& r);
		void status(const std::string& d);
		void diff(const std::string& d);
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
