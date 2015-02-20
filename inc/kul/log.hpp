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


#include <memory>
#include <string>
#include <string.h>

#include "kul/os.hpp"
#include "kul/xml.hpp"
#include "kul/defs.hpp"
#include "kul/time.hpp"
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
		virtual void log(const char* f, const int& l, const std::string& s, const mode& m) const = 0;
		const std::string line(const char* f, const int& l, const std::string& s, const mode& m) const{
			std::stringstream ss;
			ss << "[" << modeTxt(m) << "] " << kul::DateTime::NOW() << " : " << f << " : " << l << " " << s;
			return ss.str();
		}
		const std::string modeTxt(const mode& m) const{
			std::string s("NONE");
			if(m == 1)		s = "INFO";
			else if(m == 2)	s = "ERROR";
			else if(m == 3) s = "DEBUG";
			return s;
		}
};

}

class ConsoleLogger : public kul::log::ALogger{
	public:
		void log(const char* f, const int& l, const std::string& s, const log::mode& m) const{
			if(m == log::NON)
				std::cout << s << std::endl;
			else if(m == log::ERR)
				std::cerr << line(f, l, s, m) << std::endl;
			else 
				std::cout << line(f, l, s, m) << std::endl;
		}
};

class FileLogger : public kul::log::ALogger{
	private:
		const char* fi;
	public:
		FileLogger(const char* fi) : fi(fi){
			kul::File(fi, true);
		}
		void log(const char* f, const int& l, const std::string& s, const log::mode& m) const{
			//TODO finish
			line(f, l, s, m);			
		}
};

class LogMan{
	private:
		log::mode m;
		std::vector<const log::ALogger*> loggers;
		LogMan() : m(kul::log::mode::NON){
			kul::log::mode lM = kul::log::mode::NON;
			if(kul::Env::GET("KLOG")){
				std::string s(kul::Env::GET("KLOG"));
				if(s.compare("1") == 0 || s.compare("INF") == 0)      lM = log::mode::INF;
				else if(s.compare("2") == 0 || s.compare("ERR") == 0) lM = log::mode::ERR;
				else if(s.compare("3") == 0 || s.compare("DBG") == 0) lM = log::mode::DBG;
				else if(s.size() > 0) {std::cerr << "KLOG OPTION UNKNOWN" << std::endl; exit(1);}
			}
			m = lM;
		}
	public:
		static LogMan& INSTANCE(){
			static LogMan instance;
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
			LogMan::INSTANCE().log(f, l, m, msg);
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
class LogMessageToFile : public LogMessage{
	public:
		LogMessageToFile(const char* f, const int& l, const log::mode& m) : LogMessage(f, l, m){}
};

#define KLOG_NON 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::NON)
#define KLOG_INF 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG_ERR 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG_DBG 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG(sev) KLOG_ ## sev

#define KLOG2_COUT_NON		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::NON)
#define KLOG2_COUT_INF		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG2_COUT_ERR		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG2_COUT_DBG		kul::LogMessageToCOut(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG2_FILE_NON		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::NON)
#define KLOG2_FILE_INF		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG2_FILE_ERR		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG2_FILE_DBG		kul::LogMessageToFile(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG2(logger, sev) KLOG2_ ## logger ## _ ## sev

}
#endif /* _KUL_LOG_HPP_ */
