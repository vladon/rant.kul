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
#include <atomic>
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

class AThread{	
	protected:
		const std::string threadID() const { return this_thread::id(); }
		virtual void run()			throw(kul::threading::Exception) 				= 0;
		virtual void join()															= 0;
		virtual void detach()														= 0;
		virtual void sleep(const ulong& millis)										= 0;
		virtual bool finished()														= 0;
		virtual void interrupt()	throw(kul::threading::InterruptionException) 	= 0;
	public:		
		virtual ~AThread(){}
};
}

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

class ALock{
	public:
		virtual ~ALock(){}
};

class ScopeLock;
class Mutex{
	private:
		std::atomic<bool> l;
		
		bool  locked()	{ return this->l.load(); }
		void  lock()	{ this->l.exchange(1); }
		void  unlock()	{ this->l.exchange(0); }
		
	public:
		Mutex() : l(0) {}
		virtual ~Mutex() {}
		friend class ScopeLock;
};

class ScopeLock : public ALock{
	private:
		Mutex& m;
	public:
		ScopeLock(Mutex& m) : m(m) {			
			while(this->m.locked()) this_thread::sleep(1);			
			this->m.lock();			
		}
		~ScopeLock(){			
			this->m.unlock();
		}
};

class Thread : public threading::AThread{
	private:
		bool s;
		std::shared_ptr<kul::osi::AThreader> th;
	protected:
		void setStarted()	{ s = true; }
		bool started()		{ return s; }
	public:
		template <class T> Thread(T t) 					: s(0), th(ThreaderService::threader(t))		{ }
		template <class T> Thread(const Ref<T>& ref) 	: s(0), th(ThreaderService::refThreader(ref))	{ }
		void run() throw(kul::threading::Exception){ 
			if(started()) KEXCEPT(threading::Exception, "Thread already started");
			setStarted();
			th->run();			
		}
		void join(){ 			
			if(started()) th->join();
		}
		void sleep(const ulong& millis){
			this_thread::sleep(millis);
		}
		void detach(){ 			
			th->detach();
		}		
		bool finished(){ 
			return th->finished();
		}		
		void interrupt() throw(kul::threading::InterruptionException){
			th->interrupt();
		}
		~Thread(){
			//LOG(INFO) << "KILLING THREAD\t\t"  << this;
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
}

class ThreadPool{
	protected:
		bool d, s;
		unsigned int m;	
		kul::Ref<ThreadPool> re;
		kul::Thread th;
		std::shared_ptr<kul::threading::APooledThreader> pT;
		std::vector<std::shared_ptr<kul::osi::AThreader> > ts;

		void setStarted()	{ s = true; }
		bool started()		{ return s; }
		virtual void start() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			for(unsigned int i = 0 ; i < m; i++){
				std::shared_ptr<kul::osi::AThreader> at = pT->threader();
				ts.push_back(at);
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
			while(true){
				int fs = 0;
				for(const std::shared_ptr<kul::osi::AThreader>& at : ts){
					if(at->finished()){
						if(at->exception() != std::exception_ptr()){
							if(!d) std::rethrow_exception(at->exception());
						}
						fs++;
					}
				}
				this_thread::sleep(1);
				if(fs == ts.size()) break;
			}
		}
		void detach(){
			d = true;
			th.detach();
		}
		void interrupt() throw(kul::threading::InterruptionException)	{ }
		const std::vector<std::exception_ptr> exceptionPointers() {
			std::vector<std::exception_ptr> ePs;
			for(const std::shared_ptr<kul::osi::AThreader>& at : ts){
				if(at->exception() != std::exception_ptr()) ePs.push_back(at->exception());
			}
			return ePs;
		}
};

template<class P>
class PredicatedThreadPool : public ThreadPool{
	private:
		P& p;
		int ps;
		std::vector<kul::osi::AThreader*> fin;
	protected:
		void start() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			while(ts.size() < ps){
				for(unsigned int i = ts.size() - fin.size(); i < m && ts.size() < ps; i++){
					std::shared_ptr<kul::osi::AThreader> at = pT->threader();
					ts.push_back(at);
					at->run();
					this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
				}
				for(const std::shared_ptr<kul::osi::AThreader>& at : ts){
					if(at->finished()){
						bool f = 0;
						for(const kul::osi::AThreader* atp : fin)
							if(atp == at.get()) f = 1;
						if(f) continue;
						if(at->exception() != std::exception_ptr()){
							if(!d) std::rethrow_exception(at->exception());
						}
						fin.push_back(at.get());
					}
				}
				this_thread::sleep(1);
			}
		}
	public:
		template <class T> PredicatedThreadPool(const T& t, P& pr) 			: ThreadPool(t) 	, p(pr), ps(p.size())	{}
		template <class T> PredicatedThreadPool(const Ref<T>& ref, P& pr) 	: ThreadPool(ref)	, p(pr), ps(p.size()){}
};
}
#endif /* _KUL_THREADS_BASE_HPP_ */


