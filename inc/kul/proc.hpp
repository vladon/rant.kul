/**

./inc/kul/proc.hpp

Created on: 24 Feb 2013

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
#ifndef _KUL_PROC_HPP_
#define _KUL_PROC_HPP_

#include <iostream>

//#include "kul/log.hpp"
#include "kul/except.hpp"

namespace kul { namespace proc {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ExitException : public Exception{
	public:
		ExitException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};
};

class Process{
	private:
		const bool wfe;
		bool s;
		std::string d;
		const std::string p;
		std::function<void(std::string)> e;
		std::function<void(std::string)> o;
		std::vector<std::string> argv;
		std::vector<std::pair<const std::string, const std::string> > evs;

	protected:
		Process(const std::string& cmd, const bool& wfe) : wfe(wfe), s(0), d(), p(){ argv.push_back(cmd); }
		Process(const std::string& p, const std::string& cmd, const bool& wfe) : wfe(wfe), s(0), d(), p(p){ argv.push_back(cmd); }

		const bool& 		started() 		const { return s; }
		const std::string&	directory()		const { return d; }
		const std::string&	path()			const { return p; }
		void 				setStarted() 	{ this->s = true; }
		virtual void 		preStart() 		{}
		virtual void 		finish()		{}
		virtual void 		tearDown()		{}
		virtual void 		run() throw (kul::proc::Exception) = 0;
		const bool			waitForExit()	const { return wfe; }

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
		void error(const int line, std::string s) throw (kul::proc::Exception){
			tearDown();
			throw kul::proc::Exception(__FILE__, line,  s);
		}
	public:
		virtual ~Process(){}
		static Process* create(const std::string& cmd, const bool wfe = true);
		static Process* create(const std::string& p, const std::string& cmd, const bool wfe = true);

		Process& addArg(const std::string& arg) { if(arg.size()) argv.push_back(arg); return *this; }
		Process& setDir(const std::string& dir) { this->d = dir; return *this; }
		Process& addEnvVar(const std::string& n, const std::string& v) { evs.push_back(std::pair<const std::string, const std::string>(n, v)); return *this;}
		virtual void start() throw(kul::proc::Exception){
			if(started()) throw kul::proc::Exception(__FILE__, __LINE__, "Process is already started");
			setStarted();
			this->run();
		}
		virtual std::string	toString(){
			std::string s(argv[0]);
			if(d.size())
				s = kul::OS::dirJoin(d, argv[0]);
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
		virtual void out(const std::string& s){
			oV.push_back(s);
		}
		virtual void err(const std::string& s){
			eV.push_back(s);
		}
	public:
		ProcessCapture(Process& p){
			p.setOut(std::bind(&ProcessCapture::out, std::ref(*this), std::placeholders::_1));
			p.setErr(std::bind(&ProcessCapture::err, std::ref(*this), std::placeholders::_1));
		}
		virtual ~ProcessCapture(){}
		const std::vector<std::string> outs(){return oV;}
		const std::vector<std::string> errs(){return eV;}
};



};
#endif /* _KUL_PROC_HPP_ */
