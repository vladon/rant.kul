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
#include "kul/def.hpp"
#include "kul/time.hpp"
#include "kul/except.hpp"
#include "kul/string.hpp"
#include "kul/threads.os.hpp"

#ifndef __KUL_LOG_TIME_FRMT__
#define __KUL_LOG_TIME_FRMT__ "%Y-%m-%d-%H:%M:%S:%i"
#endif

#ifndef __KUL_LOG_FRMT__
#define __KUL_LOG_FRMT__ "[%M] : %T - %D : %F : %L - %S"
#endif

namespace kul{ namespace log{

enum mode { NON = 0, INF, ERR, DBG};

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

}
class Logger{
	private:
		static unsigned short tid;
	public:
		void out(const std::string& s, const log::mode& m) const{
			if(m != log::ERR) 
				printf("%s", s.c_str());
			else
				fprintf(stderr, "%s", s.c_str());
		}
		void log(const char* f, const int& l, const std::string& s, const log::mode& m) const{
			std::string mode(modeTxt(m));
			std::string tr(kul::this_thread::id());
			tid = tr.size() > tid ? tr.size() : tid;
			kul::String::pad(tr, tid);
			std::string str(__KUL_LOG_FRMT__);
			kul::String::replace(str, "%M", mode);
			kul::String::replace(str, "%T", tr);
			kul::String::replace(str, "%D", kul::DateTime::NOW(__KUL_LOG_TIME_FRMT__));
			kul::String::replace(str, "%F", f);
			kul::String::replace(str, "%L", std::to_string(l));
			kul::String::replace(str, "%S", s);
			str += kul::os::EOL();
			out(str, m);
		}
		const std::string modeTxt(const log::mode& m) const{
			std::string s("NON");
			if(m == 1)		s = "INF";
			else if(m == 2)	s = "ERR";
			else if(m == 3) s = "DBG";
			return s;
		}
};

class LogMan{
	private:
		log::mode m;
		const Logger logger;
		LogMan() : m(kul::log::mode::NON), logger(){
			const char* klog = kul::env::GET("KLOG");
			if(klog){
				bool e = 0;
				std::string s(klog);
				if(s.empty()) e = 1;
				else{
					kul::String::trim(s);
					if(s.compare("0") == 0 || s.compare("NON") == 0)      m = log::mode::NON;
					else if(s.compare("1") == 0 || s.compare("INF") == 0) m = log::mode::INF;
					else if(s.compare("2") == 0 || s.compare("ERR") == 0) m = log::mode::ERR;
					else if(s.compare("3") == 0 || s.compare("DBG") == 0) m = log::mode::DBG;
					else e = 1;
				} 
				if(e){
					m = log::mode::ERR;
					out(m, "ERROR DISCERNING LOG LEVEL, ERROR LEVEL IN USE");
				}
			}
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
		void out(const log::mode& m, const std::string& s){
			if(this->m >= m) logger.out(s + kul::os::EOL(), m);
		}
		void err(const log::mode& m, const std::string& s){
			logger.out(s + kul::os::EOL(), m);
		}
};

class Message{
	protected:
		std::stringstream ss;
		const log::mode& m;

		Message(const log::mode& m) : m(m){}
	public:
		template<class T> Message& operator<<(const T& s){
			ss << s;
			return *this;
		}
};
class LogMessage : public Message{
	private:
		const char* f;
		const int& l;
	public:		
		~LogMessage(){
			LogMan::INSTANCE().log(f, l, m, ss.str());
		}
		LogMessage(const char* f, const int& l, const log::mode& m) : Message(m), f(f), l(l){}
};
class OutMessage : public Message{
	public:
		~OutMessage(){
			LogMan::INSTANCE().out(m, ss.str());
		}
		OutMessage(const log::mode& m = kul::log::mode::NON) : Message(m){}
};
class ErrMessage : public Message{
	public:
		~ErrMessage(){
			LogMan::INSTANCE().err(m, ss.str());
		}
		ErrMessage() : Message(log::mode::ERR){}
};

#define KLOG_INF 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::INF)
#define KLOG_ERR 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::ERR)
#define KLOG_DBG 	kul::LogMessage(__FILE__, __LINE__, kul::log::mode::DBG)
#define KLOG(sev) KLOG_ ## sev

#define KOUT_NON 	kul::OutMessage()
#define KOUT_INF 	kul::OutMessage(kul::log::mode::INF)
#define KOUT_ERR 	kul::OutMessage(kul::log::mode::ERR)
#define KOUT_DBG 	kul::OutMessage(kul::log::mode::DBG)
#define KOUT(sev) KOUT_ ## sev

#define KERR 		kul::ErrMessage()

} // END NAMESPACE kul
#endif /* _KUL_LOG_HPP_ */
