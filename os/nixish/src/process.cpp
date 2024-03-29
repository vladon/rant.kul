/**

./src/kul/Process.cpp

Created on: 24 Feb 2013

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
#include "kul/log.hpp"
#include "kul/proc.hpp"

int kul::this_proc::id(){
	return getpid();
}
void kul::this_proc::kill(const int& e){
	::kill(kul::this_proc::id(), e);
}

void kul::Process::waitForStatus(){
	int ret = 0;
	ret = recall(waitpid(pid(), &cStat, 0));
	assert(ret);
}
void kul::Process::waitExit() throw (kul::proc::ExitException){
	tearDown();
	cec = WEXITSTATUS(cStat);
	if(cec != 0)
		kul::LogMan::INSTANCE().err()
		? KEXCEPT(kul::proc::ExitException, "Process exit code: " + std::to_string(cec) + kul::os::EOL() + toString())
		: KEXCEPT(kul::proc::ExitException, "Process exit code: " + std::to_string(cec));
	finish();
	setFinished();
}

void kul::Process::tearDown(){
	recall(close(popPip[0]));
	recall(close(popPip[1]));
	recall(close(popPip[2]));
	recall(close(errFd[1]));
	recall(close(errFd[0]));
	recall(close(outFd[1]));
	recall(close(outFd[0]));
	recall(close(inFd[1]));
	recall(close(inFd[0]));
}

void kul::Process::run() throw (kul::proc::Exception){
	int ret = 0;

	if((ret = pipe(inFd)) < 0) 	error(__LINE__, "Failed to pipe in");
	if((ret = pipe(outFd)) < 0)	error(__LINE__, "Failed to pipe out");
	if((ret = pipe(errFd)) < 0)	error(__LINE__, "Failed to pipe err");

	this->preStart();
	pid(fork());
	if(pid() > 0){
		if(this->waitForExit()){ // parent
			popPip[0] = inFd[1];
			popPip[1] = outFd[0];
			popPip[2] = errFd[0];

#ifdef __KUL_PROC_BLOCK_ERR__
			if((ret = fcntl(popPip[1], F_SETFL, O_NONBLOCK)) < 0) error(__LINE__, "Failed nonblocking for popPip[1]");
			if((ret = fcntl(popPip[2], F_SETFL, O_NONBLOCK)) < 0) error(__LINE__, "Failed nonblocking for popPip[2]");
#else
			fcntl(popPip[1], F_SETFL, O_NONBLOCK);
			fcntl(popPip[2], F_SETFL, O_NONBLOCK);
#endif
			fd_set childOutFds;
			FD_ZERO(&childOutFds);
			FD_SET(popPip[1], &childOutFds);
			FD_SET(popPip[2], &childOutFds);
			close(inFd[1]);
			bool alive = true;

			do {
				alive = ::kill(pid(), 0) == 0;
				if(FD_ISSET(popPip[1], &childOutFds)) {
					bool b = 0;
					do {
						char cOut[1024] = {'\0'};
						int ret = recall(read(popPip[1], cOut, sizeof(cOut)));
						cOut[ret > 0 ? ret : 0] = 0;
						if (ret < 0){
							if(b && ((errno != EAGAIN) || (errno != EWOULDBLOCK)))
								error(__LINE__, "read on childout failed");
							if(((errno != EAGAIN) || (errno != EWOULDBLOCK))) b = 1;
						}
						else if (ret) out(cOut);
						else waitForStatus();
					} while(ret > 0);
				}
				if(FD_ISSET(popPip[2], &childOutFds)) {
					bool b = 0;
					do {
						char cErr[1024] = {'\0'};
						int ret = recall(read(popPip[2], cErr, sizeof(cErr)));
						cErr[ret > 0 ? ret : 0] = 0;
						if (ret < 0){
							if(b && ((errno != EAGAIN) || (errno != EWOULDBLOCK)))
								error(__LINE__, "read on childout failed");
							if(((errno != EAGAIN) || (errno != EWOULDBLOCK))) b = 1;
						}
						else if (ret) err(cErr);
						else waitForStatus();
					} while(ret > 0);
				}
				recall(waitpid(pid(), &cStat, WNOHANG));
			}while(alive);

			waitExit();
		}
	}else if(pid() == 0){ // child
		close(inFd[1]);
		close(outFd[0]);
		close(errFd[0]);

		int ret = 0; //check rets
		close(0);
		if((ret = dup(inFd[0])) < 0) 	error(__LINE__, "dup in call failed");
		close(1);
		if((ret = dup(outFd[1])) < 0) 	error(__LINE__, "dup out call failed");
		close(2);
		if((ret = dup(errFd[1])) < 0) 	error(__LINE__, "dup err call failed");

		/* SETUP EnvVars */ // SET ENV, it's a forked process so it doesn't matter - it'll die soon, like you.
		for(const std::pair<const std::string, const std::string>& ev : vars())
			env::SET(ev.first.c_str(), ev.second.c_str());

		if(!this->directory().empty()) kul::env::CWD(this->directory());
		exit(this->child());
	}else error(__LINE__, "Unhandled process id for child: " + std::to_string(pid()));
}



