/**

./inc/kul/threads.hpp

Created on: 1 Jan 2013

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
#ifndef _KUL_THREADS_HPP_
#define _KUL_THREADS_HPP_

#include <windows.h>

#include "kul/threads.base.hpp"

namespace kul{ 

namespace threading{

DWORD WINAPI thread(LPVOID lpParameter);

class AThreader : public kul::osi::AThreader{
	private:
		HANDLE h;
	protected:
		AThreader() : kul::osi::AThreader(){}
		
		void join(){
			while(!finished()) this_thread::sleep(1);
		}
		void detach(){}
		void interrupt() throw(kul::threading::InterruptionException){}
		void run() throw(kul::threading::Exception){ 
			h = CreateThread(0, 5120000, thread, this, 0, 0);
		}
		friend DWORD WINAPI thread(LPVOID);
	public:
		virtual ~AThreader(){ CloseHandle(h); }
};

template <class T>
class Threader : public AThreader{
	private:
		T t;
		Threader(const T& t) : t(t){}
	protected:
		void act(){ 
			try{
				t();
			}catch(const std::exception& e){ 
				this->eP = std::current_exception();
			}
		}
	public:
		friend class kul::ThreaderService;
};

template <class T>
class RefThreader : public AThreader{
	private:
		const Ref<T>& ref;
		RefThreader(const Ref<T>& ref) : ref(ref){}
	protected:
		void act(){ 
			try{
				ref.get()();
			}catch(const std::exception& e){ 
				this->eP = std::current_exception();
			}
		}
	public:
		friend class kul::ThreaderService;
};

}

template <class T> std::shared_ptr<kul::osi::AThreader> kul::ThreaderService::threader(const T& t){
	return std::make_shared<kul::threading::Threader<T> >(kul::threading::Threader<T>(t));
}
template <class T> std::shared_ptr<kul::osi::AThreader> kul::ThreaderService::refThreader(const Ref<T>& ref){
	return std::make_shared<kul::threading::RefThreader<T> >(kul::threading::RefThreader<T>(ref));
}

}
#endif /* _KUL_THREADS_HPP_ */
