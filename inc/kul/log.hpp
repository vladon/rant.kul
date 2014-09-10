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
#include "kul/time.hpp"
#include "kul/array.hpp"
#include "kul/except.hpp"

namespace kul{ namespace log{

enum mode { NON = 0, INF, ERR, DBG};

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ALogger{
	public:
		virtual ~ALogger(){}
		virtual void log(const char* f, const int& l, const std::string& s, const mode& m) const{}
		const std::string modeTxt(const mode& m) const{
			std::string s("NONE");
			if(m == 1)		s = "INFO";
			else if(m == 2)	s = "ERROR";
			else if(m == 3) s = "DEBUG";
			return s;
		}
};

class NullLogger : public ALogger{
	public:
		void log(const char* f, const int& l, const std::string& s, const mode& m) const{}
};

class ConsoleLogger : public ALogger{
	public:
		void log(const char* f, const int& l, const std::string& s, const mode& m) const{
			if(m == ERR) 	std::cerr << kul::DateTime::NOW() << " " << f << " : " << l << " " << modeTxt(m) << " " << s << std::endl;
			else 			std::cout << kul::DateTime::NOW() << " " << f << " : " << l << " " << modeTxt(m) << " " << s << std::endl;
		}
};

class FileLogger : public ALogger{
	public:
		void log(const char* f, const int& l, const std::string& s, const mode& m) const{}
};

};

class LogMan{
	private:
		const log::mode m;
		static LogMan* instance;
		std::vector<const log::ALogger*> loggers;
		LogMan(const log::mode& m) : m(m){}
	public:
		static LogMan* INSTANCE(){
			if(!instance) {
				kul::log::mode lM = kul::log::mode::NON;
				if(kul::os::getEnvVar("KLOG")){
					std::string s(kul::os::getEnvVar("KLOG"));
					if(s.compare("1") == 0 || s.compare("INF") == 0)      lM = log::mode::INF;
					else if(s.compare("2") == 0 || s.compare("ERR") == 0) lM = log::mode::ERR;
					else if(s.compare("3") == 0 || s.compare("DBG") == 0) lM = log::mode::DBG;
					else {std::cerr << "KLOG OPTION UNKNOWN" << std::endl; exit(1);}
				}
				instance = new LogMan(lM);
			}
			return instance;
		};
		virtual ~LogMan(){}
		void addLogger(const log::ALogger& l){
			loggers.push_back(&l);
		}
		void log(const char* f, const int& l, const log::mode& m, const std::string& s){
			if(loggers.size() == 0 && m == log::mode::ERR)
				std::cerr << kul::DateTime::NOW() << " " << f << " : " << l << " - " << s << std::endl;
			for(const log::ALogger* logger : loggers)
				if(this->m >= m) logger->log(f, l, s, m);			
		}
};

class LogMessage{
	private:
		const char* f;
		const int& l;
		const log::mode& m;
		std::string msg;
	public:		
		~LogMessage(){
			LogMan::INSTANCE()->log(f, l, m, msg);
		}
		LogMessage(const char* f, const int& l, const log::mode& m) : f(f), l(l), m(m){}
		template<class T> LogMessage& operator<<(const T& s){			
			std::stringstream ss;
			ss << s;			
			msg += ss.str();
			return *this;
		}
};

class LogMessageToCOut : public LogMessage{
	public:
		LogMessageToCOut(const char* f, const int& l, const log::mode& m) : LogMessage(f, l, m){}
};
class LogMessageToCErr : public LogMessage{
	public:
		LogMessageToCErr(const char* f, const int& l, const log::mode& m) : LogMessage(f, l, m){}
};

class LogMessageToFile : public LogMessage{
	public:
		LogMessageToFile(const char* f, const int& l, const log::mode& m) : LogMessage(f, l, m){}
};


#define KLOG_INF 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG_ERR 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG_DBG 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG(sev) KLOG_ ## sev

#define KLOG2_COUT_NON		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::NON)
#define KLOG2_COUT_INF		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG2_COUT_ERR		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG2_COUT_DBG		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG2_CERR_INF		kul::LogMessageToCErr(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG2_CERR_ERR		kul::LogMessageToCErr(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG2_CERR_DBG		kul::LogMessageToCErr(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG2_FILE_INF		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG2_FILE_ERR		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG2_FILE_DBG		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG2(logger, sev) KLOG2_ ## logger ## _ ## sev

};

#endif /* _KUL_LOG_HPP_ */