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

#include <chrono>
#include <ctime>

#include "kul/string.hpp"

namespace kul {

class DateTime{
	private:
		static const long int millis(){
			std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
			time_t tnow = std::chrono::system_clock::to_time_t(now);
			tm *date = std::localtime(&tnow);
			date->tm_hour = 0;
			date->tm_min = 0;
			date->tm_sec = 0;
			auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - midnight).count();
		}
	public:
		static const std::string NOW(const std::string f = "") {
			std::time_t result = std::time(NULL);
			std::string s(std::asctime(std::localtime(&result)));
			s = s.substr(0, 19) + ":" 
			+ kul::String::toString(millis()) 
			+ s.substr(20);
			return s;
		}
};

namespace time{ 
class Now{
	public:
		static int64_t millis()		{ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t micros()		{ return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t nanos()		{ return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
};};

};
#endif /* _KUL_TIME_HPP_ */
