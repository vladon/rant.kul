/**

./inc/kul/proc.hpp

Created on: 24 Feb 2013

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
#ifndef _KUL_PROC_BASE_HPP_
#define _KUL_PROC_BASE_HPP_

#include <vector>
#include <sstream>
#include <iostream>
#include <functional>

#include "kul/except.hpp"

namespace kul { 

namespace this_proc{
	int id();
	void kill(const int& e);
}

namespace proc{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class ExitException : public kul::proc::Exception{
	public:
		ExitException(const char*f, const int l, const std::string& s) : Exception(f, l, s){}
};
}

class AProcess{
	private:
		bool f, s;
		const bool wfe;
		unsigned int pi;
		const std::string d;
		std::function<void(std::string)> e;
		std::function<void(std::string)> o;
		std::vector<std::string> argv;
		std::vector<std::pair<const std::string, const std::string> > evs;
		friend std::ostream& operator<<(std::ostream&, const AProcess&);
	protected:
		AProcess(const std::string& cmd, const bool& wfe) : f(0), s(0), wfe(wfe), pi(0) { argv.push_back(cmd); }
		AProcess(const std::string& cmd, const std::string& d, const bool& wfe) : f(0), s(0), wfe(wfe), d(d){ argv.push_back(cmd); }
		virtual ~AProcess(){}

		const std::string&	directory()const { return d; }
		void setFinished()	{ f = 1; }
		virtual bool kill(int k = 6) = 0;
		virtual void preStart() {}
		virtual void finish()	{}
		virtual void tearDown()	{}
		virtual void run() throw (kul::Exception) = 0;
		bool waitForExit()	const { return wfe; }
		void pid(const unsigned int& pi )  { this->pi = pi; }

		const std::vector<std::string>&		 									args()	const { return argv; };
		const std::vector<std::pair<const std::string, const std::string> >& 	vars()	const { return evs; }
		virtual void out(const std::string& s){
			if(this->o) this->o(s);
			else 		printf("%s", s.c_str());
		}
		virtual void err(const std::string& s){
			if(this->e) this->e(s);
			else 		fprintf(stderr, "%s", s.c_str());
		}
		void error(const int line, std::string s) throw (kul::Exception){
			tearDown();
			KEXCEPT(kul::proc::Exception, s);
		}
	public:
		template <class T> AProcess& arg(const T& a) { 
			std::stringstream ss;
			ss << a;
			if(ss.str().size()) argv.push_back(ss.str());
			return *this; 
		}
		AProcess& arg(const std::string& a) { if(a.size()) argv.push_back(a); return *this; }
		AProcess& var(const std::string& n, const std::string& v) { evs.push_back(std::pair<const std::string, const std::string>(n, v)); return *this;}
		virtual void start() throw(kul::Exception){
			if(this->s) KEXCEPT(kul::proc::Exception, "Process is already started");
			this->s = true;
			this->run();
		}
		const unsigned int& pid() 	const { return pi; }
		bool started()		const { return pi > 0; }
		bool finished()		const { return f; }
		virtual const std::string toString() const{
			std::string s;
			for(const std::string& a : args()) s += a + " ";
			s.pop_back();
			return s;
		}
		void setOut(std::function<void(std::string)> o) { this->o = o; }
		void setErr(std::function<void(std::string)> e) { this->e = e; }
};

inline std::ostream& operator<<(std::ostream &s, const AProcess &p){
	return s << p.toString();
}

class ProcessCapture{
	private:
		std::stringstream so;
		std::stringstream se;
	protected:
		ProcessCapture(){}
		ProcessCapture(const ProcessCapture& pc) : so(pc.so.str()), se(pc.se.str()){}
		virtual void out(const std::string& s){
			so << s;
		}
		virtual void err(const std::string& s){
			se << s;
		}
	public:
		ProcessCapture(AProcess& p){
			p.setOut(std::bind(&ProcessCapture::out, std::ref(*this), std::placeholders::_1));
			p.setErr(std::bind(&ProcessCapture::err, std::ref(*this), std::placeholders::_1));
		}
		virtual ~ProcessCapture(){}
		const std::string outs() const { return so.str(); }
		const std::string errs() const { return se.str(); }
};

}
#endif /* _KUL_PROC_BASE_HPP_ */
