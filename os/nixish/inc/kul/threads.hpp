/**

./inc/kul/threads.hpp

Created on: 1 Jan 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul.nix (The Kommon Usage Library for Linux Distros).

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

#include <time.h>
#include <pthread.h>

#include "kul/threads.base.hpp"

namespace kul{ 

namespace threading{

class AThreader : public kul::osi::AThreader{
	private:
		pthread_t thread;
		static void* threadFunction(void* aThreader){
			reinterpret_cast<AThreader*>(aThreader)->act(); 
			reinterpret_cast<AThreader*>(aThreader)->setFinished(); 
			return 0;
		}
	protected:
		AThreader() : kul::osi::AThreader(), thread(){}
		virtual ~AThreader(){}
		void join()			{
			while(!finished()) this_thread::sleep(1);
		}
		void detach()		{}
		void interrupt() throw(kul::threading::InterruptionException){ 
			pthread_cancel(thread);
		}
		void run() throw(kul::threading::Exception){
			pthread_create(&thread, NULL, AThreader::threadFunction, this);
		}
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
		const Ref<T> r;
		RefThreader(const Ref<T>& r) : r(r) {}
	protected:
		void act(){ 
			try{
				r.get()();
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