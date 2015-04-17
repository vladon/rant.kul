/**

./src/kul/ipc/ipc.cpp

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
	DWORD  dwThreadId = 0; 
	hPipe = INVALID_HANDLE_VALUE;
	LPTSTR lpszPipename = _strdup(uuid.c_str()); 
	hPipe = CreateNamedPipe( 
		lpszPipename,			 // pipe name 
		PIPE_ACCESS_DUPLEX,		// read/write access 
		PIPE_TYPE_MESSAGE |		// message type pipe 
		PIPE_READMODE_MESSAGE |	// message-read mode 
		PIPE_WAIT,				// blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFSIZE,				  // output buffer size 
		BUFSIZE,				  // input buffer size 
		0,						// client time-out 
		NULL);					// default security attribute 
	if (hPipe == INVALID_HANDLE_VALUE)
		KEXCEPT(kul::ipc::Exception, "CreateNamedPipe failed: " + std::to_string(GetLastError()));
}

void kul::ipc::Server::listen() throw(kul::ipc::Exception){
	while(lp){
		HANDLE hHeap 	= GetProcessHeap();
		pchRequest 		= (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
		bool fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if(!fConnected) continue;
		DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
		if (pchRequest == NULL)
			KEXCEPT(kul::ipc::Exception, "Pipe Server Failure pchRequest: " + std::to_string(GetLastError()));
		bool fSuccess = FALSE;
		while(1){
			fSuccess = ReadFile(hPipe, pchRequest, BUFSIZE*sizeof(TCHAR), &cbBytesRead, NULL); 
			if(!fSuccess || cbBytesRead == 0 && (GetLastError() == ERROR_BROKEN_PIPE))
				break;
			handle(pchRequest);
		}
		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe); 
		HeapFree(hHeap, 0, pchRequest);
		if(lp != -1) lp--;
	}
	CloseHandle(hPipe); 
}
void kul::ipc::Server::handle(const std::string& s){
	KLOG(INF) << s;
}

void kul::ipc::Client::send(const std::string& m) const throw(kul::ipc::Exception){
	DWORD  cbToWrite, cbWritten;
	LPTSTR lpvMessage = _strdup(m.c_str()); 
	cbToWrite = (lstrlen(lpvMessage)+1)*sizeof(TCHAR);
	if(!WriteFile(hPipe, lpvMessage, cbToWrite, &cbWritten, NULL))
		KEXCEPT(kul::ipc::Exception, "WriteFile to pipe failed: " + std::to_string(GetLastError()));
}

void kul::ipc::Client::start() throw(kul::ipc::Exception){
	bool fSuccess = FALSE; 
	DWORD  dwMode; 
	LPTSTR lpszPipename = _strdup(uuid.c_str());
	while(1){ 
		hPipe = CreateFile( 
			lpszPipename,	// pipe name 
			GENERIC_READ | GENERIC_WRITE, // read and write access 
			0,				// no sharing 
			NULL,			// default security attributes
			OPEN_EXISTING,	// opens existing pipe 
			0,				// default attributes 
			NULL);			// no template file 
 
		if (hPipe != INVALID_HANDLE_VALUE) break; 
 
		if (GetLastError() != ERROR_PIPE_BUSY)
			KEXCEPT(kul::ipc::Exception, "Could not open pipe: " + std::to_string(GetLastError()));
 
		if (!WaitNamedPipe(lpszPipename, 5000))
			KEXCEPT(kul::ipc::Exception, "Could not open pipe: 5 second wait timed out.");
	} 
	dwMode = PIPE_READMODE_MESSAGE; 
	fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
	if(!fSuccess)
		KEXCEPT(kul::ipc::Exception, "SetNamedPipeHandleState failed: " + std::to_string(GetLastError()));

}
void kul::ipc::Client::stop() const throw(kul::ipc::Exception){
	CloseHandle(hPipe); 
}
