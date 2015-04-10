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
	while(true /*listen 1/num/infinit*/){

		HANDLE hHeap	  = GetProcessHeap();
		pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));

		BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if(!fConnected) continue;

		DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
		BOOL fSuccess = FALSE;

		if (pchRequest == NULL)
			KEXCEPT(kul::ipc::Exception, "Pipe Server Failure pchRequest: " + std::to_string(GetLastError()));

		// Loop until done reading
		while (1){ 
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.
			fSuccess = ReadFile( 
				hPipe,		// handle to pipe 
				pchRequest,	// buffer to receive data 
				BUFSIZE*sizeof(TCHAR), // size of buffer 
				&cbBytesRead, // number of bytes read 
				NULL);		// not overlapped I/O 

			if(!fSuccess || cbBytesRead == 0){	
				if(GetLastError() == ERROR_BROKEN_PIPE)
				// 	_tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError()); 
				// else
				// 	_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
				break;
			}
			handle(pchRequest);
		}
		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe); 
		HeapFree(hHeap, 0, pchRequest);
	}
	CloseHandle(hPipe); 
}
void kul::ipc::Server::handle(const std::string& s){
	KLOG(INF) << s;
	// respond("kul::ipc::Server::handle");
}
/** works on windows, review for others
void kul::ipc::Server::respond(const std::string& s) {
	LPTSTR str 			= _strdup(s.c_str()); 
	HANDLE hHeap 		= GetProcessHeap();
	DWORD cbReplyBytes 	= 0, cbWritten = 0; 
	pchReply 			= (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
	if (pchReply == NULL){
		if (pchRequest != NULL) HeapFree(hHeap, 0, TEXT(pchRequest));
		KEXCEPT(kul::ipc::Exception, "Pipe Server Failure pchReply: " + std::to_string(GetLastError()));
	}
	// Check the outgoing message to make sure it's not too long for the buffer.
	if (FAILED(StringCchCopy( pchReply, BUFSIZE, str))){
		HeapFree(hHeap, 0, pchRequest);
		HeapFree(hHeap, 0, pchReply);
		KEXCEPT(kul::ipc::Exception, "StringCchCopy failed, no outgoing message.");
	}
	cbReplyBytes = (lstrlen(pchReply)+1)*sizeof(TCHAR);
	// Write the reply to the pipe. 
	BOOL fSuccess = WriteFile( 
		hPipe,		// handle to pipe 
		pchReply,	 // buffer to write from 
		cbReplyBytes, // number of bytes to write 
		&cbWritten,	// number of bytes written 
		NULL);		// not overlapped I/O 
	HeapFree(hHeap, 0, pchReply);
	if (!fSuccess || cbReplyBytes != cbWritten)
		KEXCEPT(kul::ipc::Exception, "kul::ipc::Server::respond failed: " + std::to_string(GetLastError()));
}*/

void kul::ipc::Client::send(const std::string& m) const throw(kul::ipc::Exception){
	DWORD  cbToWrite, cbWritten;
	LPTSTR lpvMessage = _strdup(m.c_str()); 
	// Send a message to the pipe server. 
	cbToWrite = (lstrlen(lpvMessage)+1)*sizeof(TCHAR);
	BOOL	fSuccess = FALSE; 
	fSuccess = WriteFile(
		hPipe,		// pipe handle 
		lpvMessage,	// message 
		cbToWrite,	// message length 
		&cbWritten,	// bytes written 
		NULL);		// not overlapped 

	if(!fSuccess)
		KEXCEPT(kul::ipc::Exception, "WriteFile to pipe failed: " + std::to_string(GetLastError()));
}
void kul::ipc::Client::start() throw(kul::ipc::Exception){
	BOOL fSuccess = FALSE; 
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
 
		// Break if the pipe handle is valid. 
		if (hPipe != INVALID_HANDLE_VALUE) break; 
 
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
			KEXCEPT(kul::ipc::Exception, "Could not open pipe: " + std::to_string(GetLastError()));
 
		// All pipe instances are busy, so wait for 5 seconds. DEFINE WAIT TIME
		if (!WaitNamedPipe(lpszPipename, 5000))
			KEXCEPT(kul::ipc::Exception, "Could not open pipe: 5 second wait timed out.");
	} 
 
	// The pipe connected; change to message-read mode. 
	dwMode = PIPE_READMODE_MESSAGE; 
	fSuccess = SetNamedPipeHandleState( 
		hPipe,	 // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,	  // don't set maximum bytes 
		NULL);	 // don't set maximum time 
	if(!fSuccess)
		KEXCEPT(kul::ipc::Exception, "SetNamedPipeHandleState failed: " + std::to_string(GetLastError()));

}
void kul::ipc::Client::stop() const throw(kul::ipc::Exception){
	CloseHandle(hPipe); 
}

/** works on windows, review for others
const std::string* kul::ipc::Client::receive() throw(kul::ipc::Exception){
	KLOG(INF);
	BOOL	fSuccess = FALSE; 
	DWORD  cbRead;
	TCHAR  chBuf[BUFSIZE]; 
	str.reset();
	// Read from the pipe. 
	if(m){
		fSuccess = ReadFile( 
			hPipe,
			chBuf,	 // buffer to receive reply 
			BUFSIZE*sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);	 // not overlapped 
		m = GetLastError() == ERROR_MORE_DATA;
		if(!fSuccess && !m)
			KEXCEPT(kul::ipc::Exception, "ReadFile from pipe failed: " + std::to_string(GetLastError()));
		if(fSuccess)
			str = std::make_unique<std::string>(chBuf);
	}
	return str.get();
}*/
