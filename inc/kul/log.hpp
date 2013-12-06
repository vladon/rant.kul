/**

./inc/kul/log.hpp

Created on: 6 Dec 2013

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

#ifndef _KUL_LOG_HPP_
#define _KUL_LOG_HPP_

#include <string>
#include <string.h>
#include <memory>

#include "kul/os.hpp"
#include "kul/xml.hpp"
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

class FileLogger : public ALogger{
	public:
		void log() const{}
		const logger type(){ return FILE; }
};

class ConsoleLogger  : public ALogger{
	public:
		void log() const{}
		const logger type(){ return CONSOLE; }
};

class LogNodeUser  : public kul::xml::NodeUser<LogNodeUser>{
	public:
		LogNodeUser(const std::string& f) : NodeUser(f)	{ get();}
		virtual LogNodeUser*					get();
		virtual const kul::xml::NodeValidator	validator();
};

};

class LogMan{
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
			for(const log::logger& t : types)
				for(const log::ALogger& l : loggers)
					l.log();
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