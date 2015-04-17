/**

./src/kul/ipc.hpp

Created on: 31 Jan 2015

Copyright (c) 2015, Philip Deegan

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
#ifndef _KUL_IPC_HPP_
#define _KUL_IPC_HPP_

#ifndef _KUL_IPC_UUID_PREFIX_
#define _KUL_IPC_UUID_PREFIX_ "\\\\.\\pipe\\"
#endif

#include "kul/proc.hpp"

#include <memory>
#include <conio.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h> 

#define BUFSIZE 512

namespace kul{ namespace ipc{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class Server{
	private:
		int lp;
		const std::string uuid;
		HANDLE hPipe;
		TCHAR *pchRequest, *pchReply;

		void start() throw(Exception);
	protected:
		virtual void handle(const std::string& s);
	public:
		virtual ~Server(){
			if (pchRequest != NULL) HeapFree(GetProcessHeap(), 0, pchRequest);
			if (pchReply != NULL) HeapFree(GetProcessHeap(), 0, pchReply);
		}
		void listen() throw(Exception);
		Server(const int& lp = -1) throw(Exception) : lp(lp), uuid(_KUL_IPC_UUID_PREFIX_ + std::string("pid\\") + std::to_string(kul::this_proc::id())){ start();}
		Server(const std::string& ui, const int& lp = -1) throw(Exception) : uuid(_KUL_IPC_UUID_PREFIX_ + ui), lp(lp){ start();}
};

class Client{
	private:
		const std::string uuid;

		HANDLE hPipe; 
		void start() throw(Exception);
		void stop() const throw(Exception);
	public:
		virtual ~Client(){
			stop();
		}
		Client(const std::string& ui) throw(Exception) : uuid(_KUL_IPC_UUID_PREFIX_ + ui) { start(); }
		Client(const int& pid) throw(Exception) : uuid(_KUL_IPC_UUID_PREFIX_ + std::string("pid\\") + std::to_string(pid)) { start(); }
		virtual void send(const std::string& m) const throw(Exception);
};

}// END NAMESPACE ipc
}// END NAMESPACE kul

#endif /* _KUL_IPC_HPP_ */
