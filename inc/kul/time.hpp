/**

./inc/kul/time.hpp

Created on: 04 Jan 2014

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
#ifndef _KUL_TIME_HPP_
#define _KUL_TIME_HPP_

#include <ctime>
#include <chrono>
#include <sstream>

#include "kul/string.hpp"

namespace kul {
namespace time{
class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};
}

class Now{
	public:
		static int64_t MILLIS()	{ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t MICROS()	{ return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t NANOS()	{ return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
};

class DateTime{
	private:
		static const std::string MILLIS(){
			std::string s(std::to_string(Now::MILLIS()));
			return s.substr(s.length() - 3);
		}
	public:
		static const std::string AS(const std::time_t t, const std::string& f = "%Y-%m-%d-%H:%M:%S"){
			char buffer [80];
			std::strftime(buffer,80, f.c_str(), std::localtime(&t));
			std::string s(buffer);
			if(f.find("%i") != std::string::npos)
				kul::String::replace(s, "%i", MILLIS());
			return s;
		}
		static const std::string AS(const std::string& epoch, const std::string& f = "%Y-%m-%d-%H:%M:%S"){
			ulong e = 0;
			std::stringstream ss(epoch);
			ss >> e;
			if(!e) KEXCEPT(time::Exception, "Invalid time used :" + epoch);
			return AS(e, f);
		}
		static const std::string NOW(const std::string& f = "%Y-%m-%d-%H:%M:%S"){
			return AS(std::time(NULL), f);
		}
};
}
#endif /* _KUL_TIME_HPP_ */
