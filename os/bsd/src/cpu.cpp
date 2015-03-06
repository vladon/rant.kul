/**

./src/os.cpp

Created on: 1 Jan 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul.bsd (The Kommon Usage Library for BSD).

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
#include <pwd.h>
#include <thread>
#include <fstream>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include "kul/os.hpp"

unsigned int kul::cpu::cores() {
	unsigned int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
	int mib[4];
	size_t len = sizeof(numCPU);

	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;   // alternatively, try HW_NCPU;

	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if(numCPU < 1){
		mib[1] = HW_NCPU;
		sysctl( mib, 2, &numCPU, &len, NULL, 0 );
		if(numCPU < 1) numCPU = 1;
	}
	return numCPU;
}
unsigned int kul::cpu::threads() {
	return std::thread::hardware_concurrency();
}
