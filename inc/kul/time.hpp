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

namespace kul {
namespace date{
namespace time{
class Now{
	private:

	public:
		static const std::string GET(const std::string f = "") {
			return "";
		}
};
};
};

namespace time{
class Now{
	public:
		static int64_t millis()		{ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t micros()		{ return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
		static int64_t nanos()		{ return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
};


};};
#endif /* _KUL_TIME_HPP_ */
