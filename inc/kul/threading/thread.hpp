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
#ifndef _KUL_THREADING_THREAD_HPP_
#define _KUL_THREADING_THREAD_HPP_

#include <queue>
#include <atomic>
#include <chrono>
#include <thread>

#include "kul/defs.hpp" 
#include "kul/except.hpp"

namespace kul{ namespace threading{

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
		virtual unsigned long threadID() 									const	= 0;
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

template <class T>
class CRef{
	private:
		const T& t;
	public:
		CRef(const T& t) : t(t){}
		const T& get() const{ return t;}
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

		virtual unsigned long threadID() 							const 		= 0;
		virtual void act() 														= 0;		
		virtual void join() 													= 0;
		virtual void detach() 													= 0;
		virtual void interrupt() throw(kul::threading::InterruptionException) 	= 0;
		virtual void run() throw(kul::threading::Exception) 					= 0;
		void setFinished()									{ f = true;}
		bool finished(	)	 						const 	{ return f;}
		const std::exception_ptr& exception() 		const	{ return eP;}
		const void exception(const std::exception_ptr& e)	{ eP = e;}

	public:
		virtual ~AThreader(){}
		friend class kul::Thread;
		friend class kul::ThreadPool;
		template<class P> friend class kul::PredicatedThreadPool;
};
}// END NAMESPACE OSI OS Independent

namespace this_thread{	// linux needs a wait as the threads are loaded too fast - TODO look into - if it is resolved, it should be safely removed
	const size_t id();
	void uSleep(const unsigned long& nanos);
	void sleep(const unsigned long& millis);
}
namespace threading{
class ThreaderService{
	public:
		template <class T> static std::shared_ptr<kul::osi::AThreader> getThreader(const T& t);
		template <class T> static std::shared_ptr<kul::osi::AThreader> getRefThreader(const Ref<T>& ref);
		template <class T> static std::shared_ptr<kul::osi::AThreader> getCRefThreader(const CRef<T>& ref);
};
}
class ScopeLock;

class ALock{
	public:
		virtual ~ALock(){}
};

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
		template <class T> Thread(T t) 					: s(0), th(threading::ThreaderService::getThreader(t))			{ }
		template <class T> Thread(const Ref<T>& ref) 	: s(0), th(threading::ThreaderService::getRefThreader(ref))	{ }
		template <class T> Thread(const CRef<T>& ref) 	: s(0), th(threading::ThreaderService::getCRefThreader(ref))	{ }
		unsigned long threadID() const { return th->threadID(); }
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

class ThreadGroup{
	private:
		std::vector<Thread> threads;
	public:
		void addThread(Thread& thread){
			threads.push_back(thread);
		}
		void runAll(){
			for(Thread& t: threads){
				t.run();
				this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
			}
		}
		void joinAll(){
			if(threads.size() == 0) return;
			while(true){
				bool f = true;
				for(Thread& t: threads) 
					if(!t.finished()){ f = false; break; }
				if(f) break;
				this_thread::sleep(1);
			}
		}
};

namespace threading{
class APooledThreader{
	public:
		virtual ~APooledThreader(){}
		virtual std::shared_ptr<kul::osi::AThreader> getThreader() const = 0;
};
template <class T>
class PooledThreader : public APooledThreader{
	private:
		const T t;
	public:
		PooledThreader(const T& t) : t(t){}
		std::shared_ptr<kul::osi::AThreader> getThreader() const { return ThreaderService::getThreader(t);}
};
template <class T>
class PooledRefThreader : public APooledThreader{
	private:
		const Ref<T>& r;
	public:
		PooledRefThreader(const Ref<T>& ref) : r(ref){}
		std::shared_ptr<kul::osi::AThreader> getThreader() const { return ThreaderService::getRefThreader(r);}
};
template <class T>
class PooledCRefThreader : public APooledThreader{
	private:
		const CRef<T>& r;
	public:		
		PooledCRefThreader(const CRef<T>& ref) : r(ref){}
		std::shared_ptr<kul::osi::AThreader> getThreader() const { return ThreaderService::getCRefThreader(r);}
};
}

class ThreadPool{
	protected:
		bool d, s;
		unsigned int m;
		std::shared_ptr<kul::threading::APooledThreader> pT;
		std::queue<std::shared_ptr<kul::osi::AThreader> > ts;
		std::vector<std::exception_ptr> ePs;

		void setStarted()	{ s = true; }
		bool started()		{ return s; }
	public:
		template <class T> ThreadPool(const T& t) 			: d(0), s(0), m(1), pT(std::make_shared<kul::threading::PooledThreader<T> >(t)){}
		template <class T> ThreadPool(const Ref<T>& ref) 	: d(0), s(0), m(1), pT(std::make_shared<kul::threading::PooledRefThreader<T> >(ref)){}
		template <class T> ThreadPool(const CRef<T>& ref) 	: d(0), s(0), m(1), pT(std::make_shared<kul::threading::PooledCRefThreader<T> >(ref)){}
		~ThreadPool(){}
		void setMax(const int& max) { m = max;}

		virtual void run() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();

			for(unsigned int i = 0 ; i < m; i++){
				std::shared_ptr<kul::osi::AThreader> at = pT->getThreader();
				ts.push(at);
				at->run();
				this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
			}
		}
		void join() throw (std::exception) {
			while(true){
				for(int i = 0; i < ts.size(); i++){
					std::shared_ptr<kul::osi::AThreader>& at = ts.front();
					if(at->finished()){
						if(at->exception() != std::exception_ptr()){
							if(!d) std::rethrow_exception(at->exception());
							ePs.push_back(at->exception());
						}
						ts.pop();
					}else break;
				}
				if(ts.size() == 0) return;
				this_thread::sleep(1);
			}
		}
		void detach(){ d = true;}		
		void interrupt() throw(kul::threading::InterruptionException)	{ }
		const std::vector<std::exception_ptr>& exceptionPointers() { return ePs;}
	
};

template<class P>
class PredicatedThreadPool : public ThreadPool{
	private:
		P& p;
	public:
		template <class T> PredicatedThreadPool(const T& t, P& pr) 			: ThreadPool(t), p(pr)	{}
		template <class T> PredicatedThreadPool(const Ref<T>& ref, P& pr) 	: ThreadPool(ref), p(pr){}
		template <class T> PredicatedThreadPool(const CRef<T>& ref, P& pr) 	: ThreadPool(ref), p(pr){}
		void run() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			while(p.size() > 0){
				for(int i = 0; i < ts.size(); i++){
					std::shared_ptr<kul::osi::AThreader>& at = ts.front();
					if(at->finished()){
						if(at->exception() != std::exception_ptr()){
							if(!d) std::rethrow_exception(at->exception());
							ePs.push_back(at->exception());
						}
						ts.pop();
					}
				}

				for(unsigned int i = ts.size(); i < m && i < p.size(); i++){
					std::shared_ptr<kul::osi::AThreader> at = pT->getThreader();
					ts.push(at);
					at->run();
					this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
				}
				this_thread::sleep(1);
			}
		}
};
}
#endif /* _KUL_THREADING_THREAD_HPP_ */


