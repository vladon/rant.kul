/**

./inc/kul/threading/thread.base.hpp

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
#ifndef _KUL_THREADS_BASE_HPP_
#define _KUL_THREADS_BASE_HPP_

#include <queue>
#include <chrono>
#include <memory>
#include <thread>

#include "kul/defs.hpp" 
#include "kul/type.hpp" 
#include "kul/except.hpp"

namespace kul{ 
namespace this_thread{	
inline void sleep(const unsigned long& millis) { std::this_thread::sleep_for(std::chrono::milliseconds(millis));}
inline void uSleep(const unsigned long& micros){ std::this_thread::sleep_for(std::chrono::microseconds(micros));}
inline void nSleep(const unsigned long& nanos) { std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));}
}// END NAMESPACE this_thread

class AThread;
class ThreadPool;
template<class P> class PredicatedThreadPool;

namespace threading{
class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};
class InterruptionException : public Exception{
	public:
		InterruptionException(const char*f, const int l, const std::string& s) : Exception(f, l, s){}
};

class ThreadObject{
	private:		
		virtual void act() = 0;
		friend class AThread;
	public:
		virtual ~ThreadObject(){}
};
}
template <class T>
class ThreadCopy : public threading::ThreadObject{
	private:
		T t;
		void act(){ t(); } 
	public:
		ThreadCopy(T t) : t(t){}
};
template <class T>
class ThreadRef : public threading::ThreadObject{
	private:
		const Ref<T>& t;
		void act(){ t.get()(); } 
	public:
		ThreadRef(const Ref<T>& t) : t(t){}
};
namespace threading{
class AThread{
	protected:
		AThread(const std::shared_ptr<ThreadObject>& t) : to(t), f(0), s(0){ /*to.reset(t);*/ }
		template <class T> AThread(const T& t) : to(std::make_shared<ThreadCopy<T>>(t)), f(0), s(0){}
		template <class T> AThread(const Ref<T>& t) : to(std::make_shared<ThreadRef<T>>(t)), f(0), s(0){}
		AThread() : f(0), s(0){}
		bool f, s;
		std::exception_ptr ep;
		std::shared_ptr<threading::ThreadObject> to;
		void act(){
			try{
				to->act(); 
			}catch(const std::exception& e){ 
				ep = std::current_exception();
			}
			f = 1;
			s = 0;
		}
	public:
		virtual ~AThread(){}
		void join(){
			while(!finished()) this_thread::sleep(11);
		}
		bool started() { return s; }
		bool finished(){ return f; }
		const std::exception_ptr& exception(){ return ep;}
		void rethrow(){ if(ep) std::rethrow_exception(ep);}
};

} // END NAMESPACE threading
} // END NAMESPACE kul
#endif /* _KUL_THREADS_BASE_HPP_ */
