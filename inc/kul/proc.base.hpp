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

#include <functional>

#include "kul/except.hpp"

namespace kul { namespace proc {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ExitException : public kul::proc::Exception{
	public:
		ExitException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};
}

class AProcess{
	private:
		const bool wfe;
		bool s;
		unsigned int pi;
		const std::string d;
		std::function<void(std::string)> e;
		std::function<void(std::string)> o;
		std::vector<std::string> argv;
		std::vector<std::pair<const std::string, const std::string> > evs;

	protected:
		AProcess(const std::string& cmd, const bool& wfe) : wfe(wfe), s(0), pi(0), d(){ argv.push_back(cmd); }
		AProcess(const std::string& cmd, const std::string& d, const bool& wfe) : wfe(wfe), s(0), d(d){ argv.push_back(cmd); }
		virtual ~AProcess(){}

		const std::string&	directory()const { return d; }
		virtual void preStart() {}
		virtual void finish()	{}
		virtual void tearDown()	{}
		virtual void run() throw (kul::Exception) = 0;
		bool waitForExit()	const { return wfe; }
		void pid(const unsigned int& pi )  { this->pi = pi; }

		const std::vector<std::string>&		 									arguments()				const { return argv; };
		const std::vector<std::pair<const std::string, const std::string> >& 	environmentVariables()	const { return evs; }
		virtual void out(const std::string& s){
			if(this->o) this->o(s);
			else 		std::cout << s << std::endl;
		}
		virtual void err(const std::string& s){
			if(this->e) this->e(s);
			else 		std::cerr << s << std::endl;
		}
		void error(const int line, std::string s) throw (kul::Exception){
			tearDown();
			KEXCEPT(kul::proc::Exception, s);
		}
	public:
		AProcess& addArg(const std::string& arg) { if(arg.size()) argv.push_back(arg); return *this; }
		AProcess& addEnvVar(const std::string& n, const std::string& v) { evs.push_back(std::pair<const std::string, const std::string>(n, v)); return *this;}
		virtual void start() throw(kul::Exception){
			if(this->s) KEXCEPT(kul::proc::Exception, "Process is already started");
			this->s = true;
			this->run();
		}
		const unsigned int& pid() 	const { return pi; }
		const bool started() 		const { return pi > 0; }
		virtual std::string	toString(){
			std::string s(argv[0]);
			if(d.size()){
				s = kul::Dir(d.c_str()).join(argv[0]);
			}
			for(unsigned int i = 1; i < argv.size(); i++) s += " " + argv[i];
			return s;
		}
		void setOut(std::function<void(std::string)> o) { this->o = o; }
		void setErr(std::function<void(std::string)> e) { this->e = e; }
};

class ProcessCapture{
	private:
		std::vector<std::string> oV;
		std::vector<std::string> eV;
	protected:
		ProcessCapture(){}
		ProcessCapture(const ProcessCapture& pc) : oV(pc.oV), eV(pc.eV){}
		virtual void out(const std::string& s){
			oV.push_back(s);
		}
		virtual void err(const std::string& s){
			eV.push_back(s);
		}
	public:
		ProcessCapture(AProcess& p){
			p.setOut(std::bind(&ProcessCapture::out, std::ref(*this), std::placeholders::_1));
			p.setErr(std::bind(&ProcessCapture::err, std::ref(*this), std::placeholders::_1));
		}
		virtual ~ProcessCapture(){}
		const std::vector<std::string> outs() const { return oV; }
		const std::vector<std::string> errs() const { return eV; }
};

}
#endif /* _KUL_PROC_BASE_HPP_ */
