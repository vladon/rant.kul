/*
 * threading.hpp
 *
 *  Created on: 1 Jan 2013
 *      Author: philix
 */

#ifndef _KUL_THREADING_THREAD_HPP_
#define _KUL_THREADING_THREAD_HPP_

#include "glog/logging.h"

#include <queue>
 
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
		virtual bool finished()														= 0;
		virtual void interrupt()	throw(kul::threading::InterruptionException) 	= 0;
	public:		
		virtual ~AThread(){}
};

template <class T>
class Object{
	void* o;
	public:
		Object(void* o){}
};

class Function{
	void* f;
	public:
		Function(void* f){}
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

class ScopeLock;

class ALock{
	public:
		virtual ~ALock(){}
};

class AMutex{
	private:
		bool l;
		std::queue<const ALock*> locks;
		void lock() 	{ l = 1;}
		void unlock() 	{ l = 0;}
		virtual void mute() const = 0;
		void addLock(const ALock& lock) { locks.push(&lock);}
		void popLock() { locks.pop();}
		const ALock* front() { return locks.front();}
		const int size() { return locks.size();}

	public:
		AMutex() : l(0){}
		const bool& locked(){ return l;}
		friend class ScopeLock;
};

class ScopeLock : public ALock{
	private:
		AMutex& m;
	public:
		ScopeLock(AMutex& m) : m(m){			
			m.addLock(*this);
			if(m.size() > 1)
				while(m.locked() && m.size() > 1 && !(this == m.front())){}			
			m.lock();
		}
		~ScopeLock(){			
			m.popLock();			
			m.unlock(); 
		}
};

};};
#endif /* _KUL_THREADING_THREAD_HPP_ */


