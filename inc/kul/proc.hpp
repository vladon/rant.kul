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

class AbstractExecCall{
	protected:
		virtual ~AbstractExecCall(){}
		virtual void 				run()		throw (kul::proc::Exception) = 0; // starts the process
		virtual void 				preStart()  = 0;
		virtual void 				finish()	= 0;
		virtual const int& 			pid()		= 0;
		virtual const std::string& 	directory()	= 0;
		virtual void out(const std::string& s){
			std::cout << s << std::endl;
		}
		virtual void err(const std::string& s){
			std::cerr << s << std::endl;
		}
	public:
		virtual void start() throw (kul::proc::Exception)= 0; // allows for setup before calling run - must call run manually.

};

class Process : public AbstractExecCall{
	private:
		bool s;
		std::string d;
		const std::string p;
		std::vector<std::string> argv;
		std::vector<std::pair<const std::string, const std::string> > evs;

	protected:
		Process(const std::string& cmd) : AbstractExecCall(), s(0), d(), p(){ argv.push_back(cmd); }
		Process(const std::string& p, const std::string& cmd) : AbstractExecCall(), s(0), d(), p(p){ argv.push_back(cmd); }

		const bool& 		started() 		{ return s; }
		const std::string&	directory()		{ return d; }
		const std::string&	path()			{ return p; }
		void 				setStarted() 	{ this->s = true; }
		virtual void 		preStart() 		= 0;
		virtual void 		finish()		= 0;

		const std::vector<std::string>&		 									arguments()				const { return argv; };
		const std::vector<std::pair<const std::string, const std::string> >& 	environmentVariables()	const { return evs; }
	public:
		virtual ~Process(){}
		static Process* create(const std::string& cmd);
		static Process* create(const std::string& p, const std::string& cmd);

		Process& addArg(const std::string& arg) { argv.push_back(arg); return *this; }
		Process& setDir(const std::string& dir) { this->d = dir; return *this; }
		Process& addEnvVar(const std::string& n, const std::string& v) { evs.push_back(std::pair<const std::string, const std::string>(n, v)); return *this;}
		virtual void start() throw(kul::proc::Exception){
			if(started()) throw kul::proc::Exception(__FILE__, __LINE__, "Process is already started");
			setStarted();
			this->run();
		}
};

class CPUMonitoredProcess : public Process{
	/*
	#include <chrono>
	#include <time.h>
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::system_clock::time_point timePoint;
	private:
		timePoint sTime;
		timePoint eTime;
		clockid_t clockid;
		struct timespec ts;*/
	protected:
		CPUMonitoredProcess(const std::string& cmd) 							: Process(cmd)		{}//, clockid(){}
		CPUMonitoredProcess(const std::string& path, const std::string& cmd)	: Process(path, cmd){}//, clockid(){}

		virtual void preStart() = 0;//    { sTime = Clock::now(); }
		virtual void finish() = 0;//      { eTime = Clock::now(); }
		/*virtual void tick() = 0;{ // really only needs to be done once before the process exits.
			if (clock_getcpuclockid(pid(), &clockid) != 0)
				throw ExitException(__FILE__, __LINE__, "getting clock_getcpuclockid");
			if (clock_gettime(clockid, &ts) == -1)
				throw ExitException(__FILE__, __LINE__, "getting clock_gettime");
		}*/
	public:
		virtual ~CPUMonitoredProcess(){}
		static CPUMonitoredProcess* create(const std::string& cmd);
		static CPUMonitoredProcess* create(const std::string& path, const std::string& cmd);

		void start() throw(kul::proc::Exception){
			if(started()) throw kul::proc::Exception(__FILE__, __LINE__, "Process is already started");
			setStarted();
			this->run();
		}
		/*const timePoint& 	startTime()	{ return sTime;}
		const timePoint& 	endTime()	{ return eTime;}
		const long int& 	cpuTime()	{ return ts.tv_nsec; }*/
};



};};
#endif /* _KUL_PROC_HPP_ */
