/**

./src/kul/http.hpp

Created on: 31 Jan 2015

Copyright (c) 2015, Philip Deegan

This file is part of kul.win (The Kommon Usage Library for Windows Environments).

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
//
// MAY REQUIRE: netsh http add urlacl url=http://localhost:666/ user=EVERYONE listen=yes delegate=no
//
#ifndef _KUL_WIN_HTTP_HPP_
#define _KUL_WIN_HTTP_HPP_

#include "kul/http.base.hpp"

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <http.h>
#include <stdio.h>

#pragma comment(lib, "httpapi.lib")

namespace kul{ namespace http{ 

class Server : public kul::http::AServer{
	private:
		HANDLE 	q = NULL;
		const PCWSTR LOCALHOST = L"http://localhost:666/";

		bool doGet(IN PHTTP_REQUEST pRequest);
		bool doPost(IN PHTTP_REQUEST pRequest);

		static const LPVOID wAlloc(ULONG& u){ return HeapAlloc(GetProcessHeap(), 0, u); }
		static const bool wFreeM(LPVOID a){ return HeapFree (GetProcessHeap(), 0, a); }
	public:
		Server(const short& p) : AServer(p){}
		~Server(){
			HttpRemoveUrl(this->q, LOCALHOST);
			if(q) CloseHandle(q);
			HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
		}
		void start() throw (kul::http::Exception);
};

class Client : public kul::http::AClient{
	private:
		int sockfd;
	public:
		Client() : AClient(){}
		~Client(){}
		void start(const std::string& u, const short& p);
		void send(const kul::http::ARequest& r) const;
};

}}

#endif /* _KUL_WIN_HTTP_HPP_ */