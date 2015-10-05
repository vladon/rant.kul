/**

./inc/kul/threads.os.hpp

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

#include "kul/type.hpp"
#include "kul/threads.base.hpp"

#include <pthread.h>

namespace kul{ 
namespace this_thread{
inline const std::string id(){
	std::ostringstream os;
	os << std::hex << pthread_self();
	return os.str();
}
} // END NAMESPACE this_thread

class Mutex{
	private:
		pthread_mutex_t mute;
	public:
		Mutex(){
			pthread_mutexattr_t att;
			pthread_mutexattr_init(&att);
			pthread_mutexattr_settype(&att, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mute, &att);
			pthread_mutexattr_destroy(&att);
		}
		~Mutex() {
			pthread_mutex_destroy(&mute);
		}
		void lock() {
			pthread_mutex_lock(&mute); 
		}
		void unlock() {
			pthread_mutex_unlock(&mute);
		}
};

class Thread : public threading::AThread{
	private:
		pthread_t thr;
		static void* threadFunction(void* th){
			((Thread*)th)->act();
			return 0;
		}
	public:
		Thread(const std::shared_ptr<threading::ThreadObject>& t) : AThread(t){}
		template <class T> Thread(const T& t) : AThread(t){}
		template <class T> Thread(const Ref<T>& t) : AThread(t){}
		virtual ~Thread(){}
		bool detach(){
			return pthread_detach(thr);
		}
		void interrupt() throw(kul::threading::InterruptionException){ 
			pthread_cancel(thr);
			f = 1;
		}
		void run() throw(kul::threading::Exception){
			if(s) KEXCEPTION("Thread running");
			f = 0;
			pthread_create(&thr, NULL, Thread::threadFunction, this);
			s = 1;
		}
};

}// END NAMESPACE kul
#endif /* _KUL_THREADS_OS_HPP_ */
