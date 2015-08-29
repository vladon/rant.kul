/**

./inc/kul/threads.hpp

Created on: 1 Jan 2013

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
#ifndef _KUL_THREADS_OS_HPP_
#define _KUL_THREADS_OS_HPP_

#include "kul/threads.base.hpp"

#include <Windows.h>

namespace kul{ 
namespace this_thread{
inline const std::string id(){
	std::ostringstream os;
	os << std::hex << std::hash<std::thread::id>()(std::this_thread::get_id());
	return os.str();
}
} // END NAMESPACE this_thread

class Mutex{
	private:
		CRITICAL_SECTION critSec;
	public:
		Mutex(){
			InitializeCriticalSection(&critSec);
		}
		~Mutex() {
			DeleteCriticalSection(&critSec);
		}
		void lock() {
			EnterCriticalSection(&critSec); 
		}
		void unlock() {
			LeaveCriticalSection(&critSec);
		}
};

namespace threading{
DWORD WINAPI threadFunction(LPVOID th);
}

class Thread : public threading::AThread{
	private:
		HANDLE h;
		friend DWORD WINAPI threading::threadFunction(LPVOID);
		void act(){ to->act(); }
		void exp(const std::exception_ptr& e){ ep = e; }
		void fin(){ f = 1; }
	public:
		Thread(const std::shared_ptr<threading::ThreadObject>& t) : AThread(t){}
		template <class T> Thread(const T& t) : AThread(t){}
		template <class T> Thread(const Ref<T>& t) : AThread(t){}
		virtual ~Thread(){}
		void join(){
			while(!finished()) this_thread::sleep(1);
		}
		bool detach(){ return CloseHandle(h); }
		void interrupt() throw(kul::threading::InterruptionException){}
		void run() throw(kul::threading::Exception){ 
			h = CreateThread(0, 5120000, threading::threadFunction, this, 0, 0);
		}
};

namespace threading{
inline DWORD WINAPI threadFunction(LPVOID th){
	try{
		reinterpret_cast<Thread*>(th)->act(); 
	}catch(const std::exception& e){ 
		reinterpret_cast<Thread*>(th)->exp(std::current_exception());
	}
	reinterpret_cast<Thread*>(th)->fin();
	return 0;
}
}


}// END NAMESPACE kul
#endif /* _KUL_THREADS_OS_HPP_ */
