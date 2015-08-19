/**

./inc/kul/win/proc.hpp

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
#ifndef _KUL_PROC_HPP_
#define _KUL_PROC_HPP_

#include <string>
#include <tchar.h>
#include <stdio.h>
#include <sstream>
#include <strsafe.h>
#include <windows.h> 

#include "kul/os.hpp"
#include "kul/def.hpp"
#include "kul/proc.base.hpp"

// extern char **environ;

namespace kul{ 

class Process : public kul::AProcess{
	private:
		HANDLE g_hChildStd_OUT_Rd;
		HANDLE g_hChildStd_OUT_Wr;
		HANDLE g_hChildStd_ERR_Rd;
		HANDLE g_hChildStd_ERR_Wr;
	protected:
		void tearDown();
		void run() throw (kul::Exception);
	public:
		Process(const std::string& cmd, const bool& wfe = true) 						: kul::AProcess(cmd, wfe), g_hChildStd_OUT_Rd(NULL), g_hChildStd_OUT_Wr(NULL), g_hChildStd_ERR_Rd(NULL), g_hChildStd_ERR_Wr(NULL){ }
		Process(const std::string& cmd, const std::string& path, const bool& wfe = true): kul::AProcess(cmd, path, wfe), g_hChildStd_OUT_Rd(NULL), g_hChildStd_OUT_Wr(NULL), g_hChildStd_ERR_Rd(NULL), g_hChildStd_ERR_Wr(NULL){}
		bool kill(int k = 6){
			if(!started()) return 0;
			DWORD dwDesiredAccess = PROCESS_TERMINATE;
			bool  bInheritHandle  = 0;
			HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, pid());
			if (hProcess == NULL) return 0;
			bool r = TerminateProcess(hProcess, 128+k);
			CloseHandle(hProcess);
			setFinished();
			return r;
		}
};


}
#endif /* _KUL_PROC_HPP_ */
