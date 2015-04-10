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
#include "kul/log.hpp"

void kul::ipc::Server::start() throw(kul::ipc::Exception){
	KLOG(INF) << kul::this_proc::id();
	mkfifo(uuid.full().c_str(), 0666);
}
void kul::ipc::Server::listen() throw(kul::ipc::Exception){
	char buff[BUFSIZE];
	while(1){
		memset(buff, 0, BUFSIZE);
		fd = open(uuid.full().c_str(), O_RDWR);//, O_RDONLY);
		if (fd == -1) KEXCEPT(kul::ipc::Exception, "Cannot open FIFO for read");
		read(fd, buff, 9);
		std::istringstream sspid(buff);
		KLOG(INF) << buff;
		int pid;
		sspid >> pid;
		KLOG(INF) << pid;
		if(pid == kul::this_proc::id())
			KEXCEPT(kul::ipc::Exception, "wut");

		kul::File mutex(uuid.name() + "_" + std::to_string(pid), uuid.dir());
		while(mutex.is()) kul::this_thread::sleep(10);
		KLOG(INF);
		memset(buff, 0, BUFSIZE);
		int l;
		read(fd, buff, 3);
		std::istringstream ssl(buff);
		ssl >> l;
		KLOG(INF) << l;
		memset(buff, 0, BUFSIZE);
		read(fd, buff, l);
		KLOG(INF) << buff;
		handle(buff);
		close(fd);
	}
}

void kul::ipc::Server::handle(const std::string& s){
	respond("kul::ipc::Server::handle");
}
void kul::ipc::Server::respond(const std::string& s){
	kul::File mutex(uuid.name() + "_" + std::to_string(kul::this_proc::id()), uuid.dir());
	KLOG(INF) << uuid.dir().path();
	KLOG(INF) << mutex.full();
	if(!mutex.mk()) KEXCEPT(kul::ipc::Exception, "Cannot create mutex for response");
	if(!mutex.is()) KEXCEPT(kul::ipc::Exception, "is not mute");
	writeLength(s);
	write(fd, s.c_str(), s.size());
	mutex.rm();
}


const std::string kul::ipc::Client::receive() throw(kul::ipc::Exception){
	char buff[BUFSIZE];
	memset(buff, 0, BUFSIZE);
	read(fd, buff, 9);
	std::istringstream ss(buff);
	int pid;
	ss >> pid;
	KLOG(INF) << pid;
	if(pid == kul::this_proc::id())
		KEXCEPT(kul::ipc::Exception, "No server listening");

	kul::File mutex(uuid.name() + "_" + std::to_string(pid), uuid.dir());
	while(mutex.is()) { kul::this_thread::sleep(10); KLOG(INF);}
	int l;
	std::string s;
	memset(buff, 0, BUFSIZE);
	if(read(fd, &l, 3)){
		KLOG(INF) << l;
		read(fd, buff, l);
		if(buff[0] != 0) s = buff;
	}
	return s;
}

void kul::ipc::Client::send(const std::string& m) const throw(kul::ipc::Exception){
	kul::File mutex(uuid.name() + "_" + std::to_string(kul::this_proc::id()), uuid.dir());
	if(!mutex.mk()) KEXCEPT(kul::ipc::Exception, "Cannot create mutex for request");
	if(!mutex.is()) KEXCEPT(kul::ipc::Exception, "is not mute");
	writePID();
	writeLength(m);
	write(fd, m.c_str(), m.size());
	mutex.rm();
	KLOG(INF);
}

void kul::ipc::Client::start() throw(kul::ipc::Exception){
	KLOG(INF) << kul::this_proc::id();
	fd = open(uuid.full().c_str(), O_RDWR);//O_WRONLY);
	if (fd == -1) KEXCEPT(kul::ipc::Exception, "Cannot contact server");
}
void kul::ipc::Client::stop() const throw(kul::ipc::Exception){
	close(fd);
}
