/**

./src/kul/Threading.cpp

Created on: 20 May 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul.nix (The Kommon Usage Library for Linux Distros).

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
#include "kul/threads.hpp"

#include <thread>
#include <chrono>
#include <iomanip>

const std::string kul::this_thread::id(){
	std::ostringstream os;
	os << std::hex << pthread_self();
	return os.str();
}
void kul::this_thread::uSleep(const unsigned long& nanos){
	std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));
}
void kul::this_thread::sleep(const unsigned long& millis){
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}
