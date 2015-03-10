/**

./inc/kul/threading/thread.hpp

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
#include <iostream>

#include "kul/defs.hpp" 
#include "kul/except.hpp"

namespace kul{ 

namespace this_thread{	
	const std::string id();
	void uSleep(const unsigned long& nanos);
	void sleep(const unsigned long& millis);
}// END NAMESPACE this_thread

namespace threading{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class InterruptionException : public Exception{
	public:
		InterruptionException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};
}// END NAMESPACE threading

template <class T>
class Ref{
	private:
		T& t;
	public:
		Ref(T& t) : t(t){}
		T& get() const{ return t;}
};

class Thread;
class ThreadPool;
template<class P> class PredicatedThreadPool;

namespace osi{
class AThreader{
	private:
		bool f;
	protected:
		AThreader() : f(0){}
		std::exception_ptr eP;

		virtual void act() 														= 0;		
		virtual void join() 													= 0;
		virtual void detach() 													= 0;
		virtual void interrupt() throw(kul::threading::InterruptionException) 	= 0;
		virtual void run() throw(kul::threading::Exception) 					= 0;
		void setFinished()									{ f = true;}
		bool finished(	)	 						const 	{ return f;}
		const std::exception_ptr& exception() 		const	{ return eP;}
		void exception(const std::exception_ptr& e)	{ eP = e;}

	public:
		virtual ~AThreader(){}
		friend class kul::Thread;
		friend class kul::ThreadPool;
		template<class P> friend class kul::PredicatedThreadPool;
};
}// END NAMESPACE OSI OS Independent

class ThreaderService{
	public:
		template <class T> static std::shared_ptr<kul::osi::AThreader> threader(const T& t);
		template <class T> static std::shared_ptr<kul::osi::AThreader> refThreader(const Ref<T>& ref);
};

class Thread{
	private:
		bool s;
		std::shared_ptr<kul::osi::AThreader> th;
	protected:
	public:
		template <class T> Thread(T t) 					: s(0), th(ThreaderService::threader(t))		{ }
		template <class T> Thread(const Ref<T>& ref) 	: s(0), th(ThreaderService::refThreader(ref))	{ }
		void run() throw(kul::threading::Exception){ 
			if(s) KEXCEPT(threading::Exception, "Thread already started");
			s = true;
			th->run();			
		}
		void join()						{ if(s) th->join();	}
		void sleep(const ulong& mil)	{ if(s) this_thread::sleep(mil); }
		void detach()					{ if(s) th->detach(); }
		bool finished()					{ return s && th->finished(); }
		void interrupt() throw(kul::threading::InterruptionException){
			th->interrupt();
		}
};

namespace threading{
class APooledThreader{
	public:
		virtual ~APooledThreader(){}
		virtual std::shared_ptr<kul::osi::AThreader> threader() const = 0;
};
template <class T>
class PooledThreader : public APooledThreader{
	private:
		const T t;
	public:
		PooledThreader(const T& t) : t(t){}
		std::shared_ptr<kul::osi::AThreader> threader() const { return ThreaderService::threader(t);}
};
template <class T>
class PooledRefThreader : public APooledThreader{
	private:
		const Ref<T>& r;
	public:
		PooledRefThreader(const Ref<T>& ref) : r(ref){}
		std::shared_ptr<kul::osi::AThreader> threader() const { return ThreaderService::refThreader(r);}
};
}// END NAMESPACE threading

class ThreadPool{
	protected:
		bool d, s;
		unsigned int m;	
		kul::Ref<ThreadPool> re;
		kul::Thread th;
		std::shared_ptr<kul::threading::APooledThreader> pT;
		std::queue<std::shared_ptr<kul::osi::AThreader> > ts;
		std::vector<std::exception_ptr> ePs;
		void setStarted()	{ s = true; }
		bool started()		{ return s; }
		virtual void start() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			for(unsigned int i = 0 ; i < m; i++){
				std::shared_ptr<kul::osi::AThreader> at = pT->threader();
				ts.push(at);
				at->run();
				this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
			}
		}
	public:
		template <class T> ThreadPool(const T& t) 			: d(0), s(0), m(1), re(*this), th(re), pT(std::make_shared<kul::threading::PooledThreader<T> >(t)){}
		template <class T> ThreadPool(const Ref<T>& ref) 	: d(0), s(0), m(1), re(*this), th(re), pT(std::make_shared<kul::threading::PooledRefThreader<T> >(ref)){}
		void setMax(const int& max) { m = max;}
		void run(){
			th.run();
		}
		void operator()(){
			start();
		}
		virtual void join() throw (std::exception){
			th.join();
			while(ts.size()){
				const std::shared_ptr<kul::osi::AThreader>& at = ts.front();
				if(at->finished()){
					if(at->exception() != std::exception_ptr()){
						if(!d) std::rethrow_exception(at->exception());
						ePs.push_back(at->exception());
					}
					ts.pop();
				}
				this_thread::sleep(1);
			}
		}
		void detach(){
			d = true;
			th.detach();
		}
		void interrupt() throw(kul::threading::InterruptionException)	{ }
		const std::vector<std::exception_ptr> exceptionPointers() {
			return ePs;
		}
};

template<class P>
class PredicatedThreadPool : public ThreadPool{
	private:
		P& p;
		unsigned int ps;
	protected:
		void start() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			unsigned int c = 0;
			while(c < ps){
				for(unsigned int i = ts.size(); i < m && c < ps; i++){
					c++;
					std::shared_ptr<kul::osi::AThreader> at = pT->threader();
					ts.push(at);
					at->run();
					this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
				}
				const std::shared_ptr<kul::osi::AThreader>* at = &ts.front();
				while(at && (*at)->finished()){
					if((*at)->exception() != std::exception_ptr()){
						if(!d) std::rethrow_exception((*at)->exception());
						ePs.push_back((*at)->exception());
					}
					ts.pop();
					at = 0;
					if(ts.size()) at = &ts.front();
					this_thread::sleep(1);
				}
			}
		}
	public:
		template <class T> PredicatedThreadPool(const T& t, P& pr) 			: ThreadPool(t) 	, p(pr), ps(p.size())	{}
		template <class T> PredicatedThreadPool(const Ref<T>& ref, P& pr) 	: ThreadPool(ref)	, p(pr), ps(p.size()){}
};
}// END NAMESPACE kul

#endif /* _KUL_THREADS_BASE_HPP_ */
