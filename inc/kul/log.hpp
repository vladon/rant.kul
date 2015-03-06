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
#include "kul/def.hpp"
#include "kul/time.hpp"
#include "kul/except.hpp"
#include "kul/string.hpp"
#include "kul/threads.base.hpp"

namespace kul{ namespace log{

enum mode { NON = 0, INF, ERR, DBG};

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

}
class Logger{
	private:
		static unsigned short tid;
	public:
		void log(const char* f, const int& l, const std::string& s, const log::mode& m) const{
			if(m == log::NON)
				std::cout << s << std::endl;
			else if(m == log::ERR)
				std::cerr << line(f, l, s, m) << std::endl;
			else 
				std::cout << line(f, l, s, m) << std::endl;
		}
		const std::string line(const char* f, const int& l, const std::string& s, const log::mode& m) const{
			std::stringstream ss;
			std::string mode("[" + modeTxt(m) + "]");
			kul::String::pad(mode, 7);
			std::string tr(kul::this_thread::id());
			tid = tr.size() > tid ? tr.size() : tid;
			kul::String::pad(tr, tid);
			ss << mode << " : " << tr << " - " << kul::DateTime::NOW().substr(4) << " : " << f << " : " << l << " " << s;
			return ss.str();
		}
		const std::string modeTxt(const log::mode& m) const{
			std::string s("NONE");
			if(m == 1)		s = "INFO";
			else if(m == 2)	s = "ERROR";
			else if(m == 3) s = "DEBUG";
			return s;
		}
};

class LogMan{
	private:
		log::mode m;
		const Logger logger;
		LogMan() : m(kul::log::mode::NON), logger(){
			kul::log::mode lM = kul::log::mode::NON;
			const char* klog = kul::Env::GET("KLOG");
			if(klog){
				std::string s(klog);
				if(s.compare("1") == 0 || s.compare("INF") == 0)      lM = log::mode::INF;
				else if(s.compare("2") == 0 || s.compare("ERR") == 0) lM = log::mode::ERR;
				else if(s.compare("3") == 0 || s.compare("DBG") == 0) lM = log::mode::DBG;
				else KEXCEPT(Exception, "KLOG OPTION UNKNOWN");
			}
			m = lM;
		}
	public:
		static LogMan& INSTANCE(){
			static LogMan instance;
			return instance;
		};
		bool inf(){ return m >= log::INF;}
		bool err(){ return m >= log::ERR;}
		bool dbg(){ return m >= log::DBG;}
		void log(const char* f, const int& l, const log::mode& m, const std::string& s){
			if(this->m >= m) logger.log(f, l, s, m);
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

#define KLOG_NON 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::NON)
#define KLOG_INF 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG_ERR 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG_DBG 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG(sev) KLOG_ ## sev

}
#endif /* _KUL_LOG_HPP_ */
