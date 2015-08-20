/**

./src/kul/http.hpp

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
// 
// MAY REQUIRE: runas admin - netsh http add urlacl url=http://localhost:666/ user=EVERYONE listen=yes delegate=no
//
#ifndef _KUL_HTTP_HPP_
#define _KUL_HTTP_HPP_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "kul/log.hpp"
#include "kul/threads.hpp"
#include "kul/http.base.hpp"

#ifndef UNICODE
#define UNICODE
#endif

#ifndef MAX_ULONG_STR
#define MAX_ULONG_STR ((ULONG) sizeof("4294967295"))
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <http.h>
#include <stdio.h>
#include <Windows.h>
#include <winsock2.h>

#pragma comment(lib, "httpapi.lib")
#pragma comment(lib,"ws2_32.lib")

namespace kul{ namespace http{ 

class Server : public kul::http::AServer{
	private:
		const std::string url;
		HANDLE q;

		bool get(PHTTP_REQUEST pRequest);
		bool post(PHTTP_REQUEST pRequest);

		void initialiseReponse(HTTP_RESPONSE& response, int status, PSTR reason);
		void addKnownHeader(HTTP_RESPONSE& response, _HTTP_HEADER_ID headerID, PSTR rawValue);
		void postClean(PUCHAR rstr);
		static const LPVOID wAlloc(ULONG& u){ return HeapAlloc(GetProcessHeap(), 0, u); }
		static const bool wFreeM(LPVOID a){ return HeapFree(GetProcessHeap(), 0, a); }
	public:
		Server(const short& p, const std::string& s = "localhost") : AServer(p), q(NULL), url("http://" + s + ":" + std::to_string(p) + "/"){
			ULONG r = 0;
			r = HttpInitialize(HTTPAPI_VERSION_1, HTTP_INITIALIZE_SERVER, NULL);
			if(r != NO_ERROR) KEXCEPT(Exception, "HttpInitialize failed: " + std::to_string(r));
			r = HttpCreateHttpHandle(&this->q, 0);
			if(r != NO_ERROR) KEXCEPT(Exception, "HttpCreateHttpHandle failed: " + std::to_string(r));
			std::wstring ws(url.begin(), url.end());
			r = HttpAddUrl(this->q, ws.c_str(), NULL);
			if(r != NO_ERROR) KEXCEPT(Exception, "HttpAddUrl failed: " + std::to_string(r));
		}
		~Server(){
			HttpRemoveUrl(this->q, std::wstring(url.begin(), url.end()).c_str());
			if(q) CloseHandle(q);
			HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
		}
		void listen() throw(kul::http::Exception);
		void stop();

		virtual const std::pair<kul::hash::set::String, std::string> handle(const std::string& res, kul::hash::map::S2S atts){
			using namespace std;
			stringstream ss;
			ss << res << " : ";	
			for(const pair<string, string> p : atts) ss << p.first << "=" << p.second << " ";
			return pair<kul::hash::set::String, string>(
				kul::hash::set::String(), res + " : " + ss.str());
		}
};

}// END NAMESPACE ipc
}// END NAMESPACE http

#endif /* _KUL_HTTP_HPP_ */
