/**

./inc/kul/log.hpp

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
#ifndef _KUL_LOG_HPP_
#define _KUL_LOG_HPP_

#include <string>
#include <string.h>
#include <memory>

#include "kul/os.hpp"
#include "kul/xml.hpp"
#include "kul/defs.hpp"
#include "kul/smart.hpp"
#include "kul/except.hpp"

namespace kul{ namespace log{

enum logger { CONSOLE = 0, FILE, NONE};

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ALogger{
	public:
		virtual ~ALogger(){}
		virtual void log() const{}
		virtual const logger type(){ return NONE; }

};

class NullLogger  : public ALogger{
	public:
		void log() const{}
		const logger type(){ return NONE; }
};

class ConsoleLogger  : public ALogger{
	public:
		void log() const{}
		const logger type(){ return CONSOLE; }
};

class FileLogger : public ALogger{
	public:
		void log() const{}
		const logger type(){ return FILE; }
};

class LogNodeUser  : public kul::xml::NodeUser<LogNodeUser>{
	public:
		LogNodeUser(const std::string& f) : NodeUser(f)	{ get();}
		virtual LogNodeUser*					get();
		virtual const kul::xml::NodeValidator	validator();
};

};

class LogMan{
	//KLOG(INFO) << kul::OS::getEnvVar("KLOG");
	private:
		static LogMan* instance;
		kul::smart::Array<log::ALogger> loggers;
		std::shared_ptr<log::LogNodeUser> xml;
		LogMan(){}
	public:
		static LogMan* INIT(const std::string& d){
			if(!instance) instance = new LogMan();
			instance->xml.reset(new log::LogNodeUser(d));
			return instance;
		};
		static LogMan* INSTANCE(){
			if(!instance) instance = new LogMan();
			return instance;
		};

		virtual ~LogMan(){}

		void addLogger(log::ALogger* l){
			if(l)  loggers.add()(l);
		}
		void log(const std::vector<log::logger> types){
			// for(const log::logger& t : types)
				// for(const log::ALogger& l : loggers)
					// l.log();
		}
};

class LogMessage{
	private:
		int l;
		const char* f;
		const char* t;
		std::string m;
	public:		
		~LogMessage(){
			if(!l || !f || !t) std::cout << m << std::endl;
			else 
				std::cout << f << " " << t << " " << l << " " << m << std::endl;
		}
		LogMessage(const char* f, int l, const char*t) :  l(l), f(f), t(t) {}
		template<class T> LogMessage& operator<<(const T& s){
			std::stringstream ss;
			ss << s;			
			m += ss.str();
			return *this;
		}
};

class LogMessageToCOut : public LogMessage{
	public:
		LogMessageToCOut() : LogMessage(0, 0, 0){}
		LogMessageToCOut(const char* f, int l, const char*t) : LogMessage(f, l, t){}
};
class LogMessageToCErr : public LogMessage{
	public:
		LogMessageToCErr(const char* f, int l, const char*t) : LogMessage(f, l, t){}
};

class LogMessageToFile : public LogMessage{
	public:
		LogMessageToFile(const char* f, int l, const char*t) : LogMessage(f, l, t){}
};


#define KLOG_INFO 	kul::LogMessage(__FILE__, __LINE__, "INFO")
#define KLOG_ERROR 	kul::LogMessage(__FILE__, __LINE__, "ERROR")
#define KLOG_DEBUG 	kul::LogMessage(__FILE__, __LINE__, "DEBUG")
#define KLOG(sev) KLOG_ ## sev

#define KLOG2_COUT_NONE		kul::LogMessageToCOut()
#define KLOG2_COUT_INFO 	kul::LogMessageToCOut(__FILE__, __LINE__, "INFO")
#define KLOG2_COUT_ERROR 	kul::LogMessageToCOut(__FILE__, __LINE__, "ERROR")
#define KLOG2_COUT_DEBUG 	kul::LogMessageToCOut(__FILE__, __LINE__, "DEBUG")
#define KLOG2_CERR_INFO 	kul::LogMessageToCErr(__FILE__, __LINE__, "INFO")
#define KLOG2_CERR_ERROR 	kul::LogMessageToCErr(__FILE__, __LINE__, "ERROR")
#define KLOG2_CERR_DEBUG 	kul::LogMessageToCErr(__FILE__, __LINE__, "DEBUG")
#define KLOG2_FILE_INFO 	kul::LogMessageToFile(__FILE__, __LINE__, "INFO")
#define KLOG2_FILE_ERROR 	kul::LogMessageToFile(__FILE__, __LINE__, "ERROR")
#define KLOG2_FILE_DEBUG 	kul::LogMessageToFile(__FILE__, __LINE__, "DEBUG")
#define KLOG2(logger, sev) KLOG2_ ## logger ## _ ## sev

};

#endif /* _KUL_LOG_HPP_ */