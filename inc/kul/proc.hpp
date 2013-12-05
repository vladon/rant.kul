/*
 * proc.hpp
 *
 *  Created on: 24 Feb 2013
 *      Author: philix
 */

#ifndef _KUL_PROC_HPP_
#define _KUL_PROC_HPP_

#include <iostream>

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
		virtual void 		preStart() 		= 0;
		virtual void 		finish()		= 0;
		virtual void 		run() throw (kul::proc::Exception) = 0; // starts the process
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
	public:
		virtual ~Process(){}
		static Process* create(const std::string& cmd, const bool wfe = true);
		static Process* create(const std::string& p, const std::string& cmd, const bool wfe = true);

		Process& addArg(const std::string& arg) { argv.push_back(arg); return *this; }
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
			for(int i = 1; i < argv.size(); i++) s += " " + argv[i];
			return s;
		}
		void			setOut(std::function<void(std::string)>& o) { this->o = o; }
		void			setErr(std::function<void(std::string)>& e) { this->e = e; }
};


};
#endif /* _KUL_PROC_HPP_ */
