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
#ifndef _KUL_THREADS_HPP_
#define _KUL_THREADS_HPP_

#include "kul/threads.os.hpp"

namespace kul{

class ScopeLock{
	private:
		Mutex& m;
	public:
		ScopeLock(Mutex& m) : m(m) {
			this->m.lock();
		}
		~ScopeLock(){
			this->m.unlock();
		}
};

class ThreadPool{
	protected:
		bool d, s;
		unsigned int m;	
		kul::Ref<ThreadPool> re;
		kul::Thread th;
		std::shared_ptr<kul::threading::ThreadObject> to;
		std::queue<std::shared_ptr<kul::Thread> > ts;
		std::vector<std::exception_ptr> ePs;
		void setStarted()	{ s = true; }
		bool started()		{ return s; }
		virtual void start() throw (std::exception) {
			if(started()) KEXCEPT(Exception, "ThreadPool is already started");
			setStarted();
			for(unsigned int i = 0 ; i < m; i++){
				std::shared_ptr<kul::Thread> at = std::make_shared<kul::Thread>(to);
				at->run();
				ts.push(at);
				this_thread::nSleep(__KUL_THREAD_SPAWN_WAIT__);
			}
		}
	public:
		template <class T> ThreadPool(const T& t) 			: d(0), s(0), m(1), re(*this), th(re), to(std::make_shared<kul::ThreadCopy<T> >(t)){}
		template <class T> ThreadPool(const Ref<T>& ref) 	: d(0), s(0), m(1), re(*this), th(re), to(std::make_shared<kul::ThreadRef<T> >(ref)){}
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
				const std::shared_ptr<kul::Thread>& at = ts.front();
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
					std::shared_ptr<kul::Thread> at = std::make_shared<kul::Thread>(to);
					at->run();
					ts.push(at);
					this_thread::nSleep(__KUL_THREAD_SPAWN_WAIT__);
				}
				const std::shared_ptr<kul::Thread>* at = &ts.front();
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
		template <class T> PredicatedThreadPool(const T& t, P& pr) 			: ThreadPool(t) 	, p(pr), ps(p.size()){}
		template <class T> PredicatedThreadPool(const Ref<T>& ref, P& pr) 	: ThreadPool(ref)	, p(pr), ps(p.size()){}
};
}// END NAMESPACE kul

#endif /* _KUL_THREADS_HPP_ */
