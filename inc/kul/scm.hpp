/**

./inc/kul/SCM.hpp

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
#include "kul/hash.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"

namespace kul{ namespace scm{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class NotFoundException : public kul::Exception{
	public:
		NotFoundException(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};
}

class SCM{
	protected:
		SCM(){}
	public:
		virtual ~SCM(){}
		const std::string type() { return typeid(*this).name(); }
		virtual void co(const std::string& l, const std::string& r, const std::string& v) const throw(Exception) = 0;
		virtual void up(const std::string& l, const std::string& r, const std::string& v) const throw(Exception) = 0;
		virtual const std::string origin(const std::string& d) const = 0;
		virtual const std::string localVersion(const std::string& d, const std::string& b) const = 0;
		virtual const std::string remoteVersion(const std::string& d, const std::string& url, const std::string& branch) const throw(Exception) = 0;

		virtual bool hasChanges(const std::string& d) const = 0;

		virtual void setOrigin(const std::string& d, const std::string& r) const = 0;
		virtual void status(const std::string& d) const = 0;
		virtual void diff(const std::string& d) const = 0;
};

namespace scm{
class Git : public SCM{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v) const throw(Exception);
		void up(const std::string& l, const std::string& r, const std::string& v) const throw(Exception);
		const std::string origin(const std::string& d) const;
		const std::string localVersion(const std::string& d, const std::string& b) const;
		const std::string remoteVersion(const std::string& d, const std::string& url, const std::string& branch) const throw(Exception);

		bool hasChanges(const std::string& d) const;

		void setOrigin(const std::string& d, const std::string& r) const;
		void status(const std::string& d) const;
		void diff(const std::string& d) const;
};

// SVN URL CHANGE: svn switch –relocate  <from URL> <to URL>
class Svn : public SCM{
	public:
		void co(const std::string& l, const std::string& r, const std::string& v) const throw(Exception);
		void up(const std::string& l, const std::string& r, const std::string& v) const throw(Exception);
		const std::string origin(const std::string& d) const;
		const std::string localVersion(const std::string& d, const std::string& b) const;
		const std::string remoteVersion(const std::string& d, const std::string& url, const std::string& branch) const throw(Exception);

		bool hasChanges(const std::string& d) const;

		void setOrigin(const std::string& d, const std::string& r) const;
		void status(const std::string& d) const;
		void diff(const std::string& d) const;
};

class Manager{
	private:
		Manager(){
			git = std::make_unique<Git>();
			svn = std::make_unique<Svn>();
			SCMs.insert(std::pair<std::string, SCM*>("git", git.get()));
			SCMs.insert(std::pair<std::string, SCM*>("svn", svn.get()));
		}
		hash::map::S2T<SCM*> SCMs;
		std::unique_ptr<SCM> git;
		std::unique_ptr<SCM> svn;
	public:
		static Manager& INSTANCE(){ 
			static Manager instance; 
			return instance;
		}
		const SCM& get(const std::string& s) throw(NotFoundException){
			if(SCMs.count(s) > 0) return *(*SCMs.find(s)).second;
			KEXCEPT(NotFoundException, "Source Control Management for " + s + " is not implemented");
		}
};

}// END NAMESPACE scm
}// END NAMESPACE kul
#endif /* _KUL_SCM_HPP_ */
