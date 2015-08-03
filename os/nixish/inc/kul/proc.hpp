/**

./inc/kul/nix/proc.hpp

Created on:  24 Feb 2013

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
#ifndef _KUL_PROC_HPP_
#define _KUL_PROC_HPP_

#include <queue>
#include <memory>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/wait.h>

#include "kul/os.hpp"
#include "kul/proc.base.hpp"

namespace kul {

class Process : public kul::AProcess{
	private:
		int inFd[2];
		int outFd[2];
		int errFd[2];
		int popPip[3];
		int cStat; //child status
		int cec; //child exit code

		void waitForStatus();
		void waitExit() throw (kul::proc::ExitException);
		inline int recall(const int& s){
			int ret; 
			while((ret = (s)) < 0x0 && (errno == EINTR)){}
			return ret;
		}
		void run() throw (kul::proc::Exception);
		
	protected:
		int	child(){
			std::string s(args()[0]);
			char** as = new char*[args().size() + 1]; // does need deleting - process exists after method
			int i = 0;
			for(const std::string& c : args()){ as[i] = const_cast<char*>(c.c_str()); i++; }
			as[i] = NULL;
			return execvp(s.c_str(), as);
		}
		const std::string command(){
			std::string s;
			for(const std::string& a : args()) s += " " + a;
			return s;
		}
		void tearDown();
		void finish()	{ }
		void preStart()	{ }
	public:
		Process(const std::string& cmd, const bool& wfe = true)							: kul::AProcess(cmd, wfe){}
		Process(const std::string& path, const std::string& cmd, const bool& wfe = true): kul::AProcess(path, cmd, wfe){}
};


}
#endif /* _KUL_PROC_HPP_ */
