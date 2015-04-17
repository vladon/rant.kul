/** 

./src/kul/ipc.cpp

Created on: 10 Sept 2014

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
#include "kul/ipc.hpp"

void kul::ipc::Server::start() throw(kul::ipc::Exception){
	uuid.dir().mk();
	mkfifo(uuid.full().c_str(), 0666);
}
void kul::ipc::Server::listen() throw(kul::ipc::Exception){
	char buff[BUFSIZE];
	while(lp){
		memset(buff, 0, BUFSIZE);
		fd = open(uuid.full().c_str(), O_RDONLY);
		if (fd == -1) KEXCEPT(kul::ipc::Exception, "Cannot open FIFO for read");
		// uncomment to check for PID 
		// read(fd, buff, 9);
		// std::istringstream sspid(buff);
		// int pid;
		// sspid >> pid;
		// if(pid == kul::this_proc::id())
		// 	KEXCEPT(kul::ipc::Exception, "wut");
		// memset(buff, 0, BUFSIZE);

		int l;
		read(fd, buff, 3);
		std::istringstream ssl(buff);
		ssl >> l;
		memset(buff, 0, BUFSIZE);
		read(fd, buff, l);
		handle(buff);
		close(fd);
		if(lp != -1) lp--;
	}
}

void kul::ipc::Client::send(const std::string& m) const throw(kul::ipc::Exception){
	// uncomment to check for PID 
	// writePID();
	writeLength(m);
	write(fd, m.c_str(), m.size());
}

void kul::ipc::Client::start() throw(kul::ipc::Exception){
	fd = open(uuid.full().c_str(), O_WRONLY);
	if (fd == -1) KEXCEPT(kul::ipc::Exception, "Cannot contact server");
}
void kul::ipc::Client::stop() const throw(kul::ipc::Exception){
	close(fd);
}
