/**

./src/kul/Process.cpp

Created on: 24 Feb 2013

Copyright (c) 2013, Philip Deegan

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
#include "kul/log.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"

void kul::Process::tearDown(){
	CloseHandle(g_hChildStd_OUT_Rd);
	CloseHandle(g_hChildStd_ERR_Rd);
}

void kul::Process::run() throw (kul::Exception){
	SECURITY_ATTRIBUTES sa;
	// Set the bInheritHandle flag so pipe handles are inherited. 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.bInheritHandle = TRUE; 
	sa.lpSecurityDescriptor = NULL; 
	// Create a pipe for the child process's STDERR. 
	if ( ! CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0) ) {
		error(__LINE__, "CreatePipe failed");
	}
	// Ensure the read handle to the pipe for STDERR is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0) ){
		error(__LINE__, "SetHandleInformation failed");
	}
	// Create a pipe for the child process's STDOUT. 
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0) ) {
		error(__LINE__, "CreatePipe failed");
	}
	// Ensure the read handle to the pipe for STDOUT is not inherited
	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ){
		error(__LINE__, "SetHandleInformation failed");
	}

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;

	bool bSuccess = FALSE; 

	ZeroMemory(&piProcInfo,  sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = g_hChildStd_ERR_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;

	preStart();

	// Record the current env vars before changing to new ones
	std::vector<std::pair<const char*, std::string> > oldEvs;
	for(std::pair<const std::string , const std::string > evs : environmentVariables()){
		oldEvs.push_back(std::pair<const char*, std::string>(evs.first.c_str(), kul::Env::GET(evs.first.c_str())));
		kul::Env::SET(evs.first.c_str(), evs.second.c_str());
	}

	const char* dir = directory().empty() ? 0 : directory().c_str();
	LPSTR szCmdline = _strdup(command().c_str());
	// Create the child process. 
	bSuccess = CreateProcess(NULL, 
		szCmdline,		// command line 
		NULL,			// process security attributes 
		NULL,			// primary thread security attributes 
		TRUE,			// handles are inherited 
		0,				// creation flags		 
		NULL,			// use parent's environment 
		dir,
		&siStartInfo,	// STARTUPINFO pointer 
		&piProcInfo);	// receives PROCESS_INFORMATION

	pid(piProcInfo.dwProcessId);

	CloseHandle(g_hChildStd_ERR_Wr);
	CloseHandle(g_hChildStd_OUT_Wr);
	
	// Reset changed env vars back to what previously was
	for(const std::pair<const char*, std::string>& oldEv : oldEvs)
		kul::Env::SET(oldEv.first, oldEv.second.c_str());

	if(!bSuccess)
		error(__LINE__, "CreateProcess failed with last error: " + GetLastError());

	if(this->waitForExit()){
		DWORD dwRead; 
		CHAR chBuf[PROCESS_BUFFER_SIZE];
		bSuccess = FALSE;

		while(WaitForSingleObject(piProcInfo.hProcess, 1) == WAIT_TIMEOUT){
			for (;;) { 
				bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, PROCESS_BUFFER_SIZE, &dwRead, NULL);
				if(!bSuccess || dwRead == 0 ) break;
				for(const std::string& l : kul::String::lines(std::string(chBuf, dwRead).c_str())) out(l);
				dwRead = 0;
			} 
			dwRead = 0;
			for (;;) { 
				bSuccess=ReadFile( g_hChildStd_ERR_Rd, chBuf, PROCESS_BUFFER_SIZE, &dwRead, NULL);
				if(!bSuccess || dwRead == 0 ) break; 
				for(const std::string& l : kul::String::lines(std::string(chBuf, dwRead).c_str())) err(l);
				dwRead = 0;
			} 
		}
	}

	tearDown();
	
	if(this->waitForExit()){
		DWORD ec = 0;
		if (FALSE == GetExitCodeProcess(piProcInfo.hProcess, &ec))
			KEXCEPT(kul::proc::Exception, "GetExitCodeProcess failure");
		if(ec != 0)
			kul::LogMan::INSTANCE().err()
			? KEXCEPT(kul::proc::ExitException, "Process exit code: " + std::to_string(ec) + kul::os::newLine() + command())
			: KEXCEPT(kul::proc::ExitException, "Process exit code: " + std::to_string(ec));
	}
	finish();
}