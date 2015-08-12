/**

./src/os.cpp

Created on: 11 Aug 2015

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
#include "kul/os.hpp"

bool kul::env::WHICH(const char* c){
	for(auto& s : kul::String::split(std::string(env::GET("PATH")), kul::env::SEP()))
		for(auto& f : kul::Dir(s).files())
			if(f.name().compare(c) == 0) return 1;
	return false;
}