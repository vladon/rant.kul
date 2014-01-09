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

#include "kul/log.hpp" 
#include "kul/defs.hpp" 
#include "kul/except.hpp"

//osi = OS Independent
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
		virtual void sleep(const long& millis)										= 0;
		virtual bool finished()														= 0;
		virtual void interrupt()	throw(kul::threading::InterruptionException) 	= 0;
	public:		
		virtual ~AThread(){}
};

};

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
template <class PRED> class ThreadPool;

namespace osi{ namespace threading{
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
		const bool finished() 						const 	{ return f;}
		const std::exception_ptr& exceptionPointer()const	{ return eP; }		

	public:
		virtual ~AThreader(){}
		friend class kul::Thread;
		template <class PRED> friend class kul::ThreadPool;
};
};};

namespace this_thread{	
	void uSleep(const unsigned long& nanos); // linux needs a wait as the threads are loaded too fast - TODO look into - if it is resolved, it should be safely removed
	void sleep(const unsigned long& millis);	
};

namespace threading{
class ThreaderService{
	public:
		template <class T> static kul::osi::threading::AThreader* getThreader(const T& t);
		template <class T> static kul::osi::threading::AThreader* getRefThreader(const Ref<T>& ref);
		template <class T> static kul::osi::threading::AThreader* getCRefThreader(const CRef<T>& ref);
};
};
class ScopeLock;

class ALock{
	public:
		virtual ~ALock(){}
};

class Mutex{
	private:
		std::atomic<bool> l;
		
		const bool  locked(){ return this->l.load(); }
		void  lock()		{ this->l.exchange(1); }
		void  unlock()		{ this->l.exchange(0); }
		
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
		std::shared_ptr<kul::osi::threading::AThreader> th;
	protected:
		void setStarted()	{ s = true; }
		bool started()		{ return s; }
	public:
		template <class T> Thread(T t) 					: s(0), th(threading::ThreaderService::getThreader(t))			{ }
		template <class T> Thread(const Ref<T>& ref) 	: s(0), th(threading::ThreaderService::getRefThreader(ref))	{ }
		template <class T> Thread(const CRef<T>& ref) 	: s(0), th(threading::ThreaderService::getCRefThreader(ref))	{ }
		unsigned long threadID() const { return th->threadID(); }
		void run() throw(kul::threading::Exception){ 
			if(started()) throw kul::threading::Exception(__FILE__, __LINE__, "Thread is already started - THROWING");			
			setStarted();
			th->run();			
		}
		void join(){ 			
			if(started()) th->join();
		}
		void sleep(const long& millis){
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
		virtual kul::osi::threading::AThreader* getThreader() const = 0;
};
template <class T>
class PooledThreader : public APooledThreader{
	private:
		const T& t;
		PooledThreader(const T& t) : t(t){}
	public:
		kul::osi::threading::AThreader* getThreader() const { return ThreaderService::getThreader(t);}
		template <class PRED> friend class kul::ThreadPool;
};
template <class T>
class PooledRefThreader : public APooledThreader{
	private:
		const Ref<T>& r;
		PooledRefThreader(const Ref<T>& ref) : r(ref){}
	public:
		kul::osi::threading::AThreader* getThreader() const { return ThreaderService::getRefThreader(r);}
		template <class PRED> friend class kul::ThreadPool;
};
template <class T>
class PooledCRefThreader : public APooledThreader{
	private:
		const CRef<T>& r;
		PooledCRefThreader(const CRef<T>& ref) : r(ref){}
	public:		
		kul::osi::threading::AThreader* getThreader() const { return ThreaderService::getCRefThreader(r);}
		template <class PRED> friend class kul::ThreadPool;
};
};

template <class PRED>
class ThreadPool{
	private:
		bool d, s;
		unsigned int m, pM;
		PRED* p;
		std::shared_ptr<threading::APooledThreader> pT;
		std::vector<kul::osi::threading::AThreader*> ts;
		std::vector<std::exception_ptr> ePs;
	protected:
		void setStarted()	{ s = true; }
		bool started()		{ return s; }
	public:
		template <class T> ThreadPool(T t) 					: d(0), s(0), m(1), p(0), pT(new threading::PooledThreader<T>(t))		{}
		template <class T> ThreadPool(const Ref<T>& ref) 	: d(0), s(0), m(1), p(0), pT(new threading::PooledRefThreader<T>(ref))	{}
		template <class T> ThreadPool(const CRef<T>& ref) 	: d(0), s(0), m(1), p(0), pT(new threading::PooledCRefThreader<T>(ref))	{}
		~ThreadPool(){ for(const kul::osi::threading::AThreader* at : ts) delete at; }
		void setMax(const int& max) { m = max;}
		void setPredicate(PRED& predicate) { p = &predicate; pM = p->size();}

		void runAndJoinAll() throw (std::exception) {
			if(started()) throw kul::threading::Exception(__FILE__, __LINE__, "Thread is already started - THROWING");			
			setStarted();
			
			if(p){
				while(true){
					unsigned int f = 0;
					for(const kul::osi::threading::AThreader* at : ts){
						if(at->finished()) f++;						
						if(at->exceptionPointer() != std::exception_ptr()){
							if(!d)
								std::rethrow_exception(at->exceptionPointer());
							ePs.push_back(at->exceptionPointer());
						}
					}					

					if(f == pM) return;					
					for(unsigned int i = f - ts.size() ; i < m && i < p->size(); i++){
						kul::osi::threading::AThreader* at = pT->getThreader();
						ts.push_back(at);
						at->run();
						this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
					}
					this_thread::sleep(1);
				}
			}else{
				for(unsigned int i = 0 ; i < m; i++){
					kul::osi::threading::AThreader* at = pT->getThreader();
					ts.push_back(at);
					at->run();
					this_thread::uSleep(__KUL_THREAD_SPAWN_UWAIT__);
				}
			}
			while(true){
				unsigned int f = 0;
				for(const kul::osi::threading::AThreader* at : ts)
					if(at->finished()) f++;
				if(f == ts.size()) return;
				this_thread::sleep(1);
			}
		}
		void detach(){ d = true;}		
		void interrupt() throw(kul::threading::InterruptionException)	{ }
		const std::vector<std::exception_ptr>& exceptionPointers() { return ePs;}
	
};



};
#endif /* _KUL_THREADING_THREAD_HPP_ */


