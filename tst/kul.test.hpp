/**

./tst/kul.test.hpp

Created on: 24 Jan 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _KUL_TEST_HPP_
#define _KUL_TEST_HPP_

#include "kul/os.hpp"
#include "kul/log.hpp"
#include "kul/xml.hpp"
#include "kul/proc.hpp"
#include "kul/smart.hpp"
#include "kul/threads.hpp"

namespace kul {
class TestThreadObject{
	private:
		int i;
	public:
		TestThreadObject() : i(0){}
		void operator()(){
		   	KLOG(INFO) << "THREAD RUNNING";
			i++;
		}
		void operator()() const{
			KLOG(INFO) << "CONST THREAD RUNNING";
		}
		void print(){ KLOG(INFO) << "i = " << i;}
};

class TestThreadPoolObject{
	private:
		int i;
		Mutex& mutex;
	public:
		TestThreadPoolObject(Mutex& mutex) : i(0), mutex(mutex){}
		void operator()(){
		   	KLOG(INFO) << "THREAD RUNNING";
			kul::ScopeLock lock(mutex);
			i++;
			KLOG(INFO) << "THREAD FINISHED";
		}
		void operator()() const{
			KLOG(INFO) << "CONST THREAD RUNNING";
		}
		void print(){ KLOG(INFO) << "i = " << i;}
};

class test{ public: test(){
	kul::smart::Array<int> a;
	a.add()(new int(1)) (new int(2)) (new int(3));
	for(const int& ii : a) KLOG(INFO) << ii;
	for(int i = 0; i < a.size(); i++) KLOG(INFO) << a[i];

	KLOG(INFO);
	KLOG(INFO) << OS::dirSep();
	KLOG(INFO) << OS::pwd();
	KLOG(INFO) << OS::userDir();
	KLOG(INFO) << OS::userAppDir("maiken");
	for(std::string s : OS::dirs()){
		KLOG(INFO) << s;
	}
	for(std::string s : OS::files(OS::pwd(), true)){
		KLOG(INFO) << s;
	}

	std::shared_ptr<kul::Process> p(kul::Process::create("echo"));
	try{
		(*p).addArg("Hello").addArg("World").start();
	}catch(const kul::proc::Exception& e){ 
		KLOG(INFO) << e.debug()<< " : " << typeid(e).name();
		exit(1);
	}

	

	for(const std::string& arg : kul::cli::CmdLine::asArgs("/path/to \"words in quotes\" words\\ not\\ in\\ quotes end"))
		KLOG(INFO) << "ARG: " << arg;

	//LOG(INFO) << std::chrono::duration_cast<std::chrono::nanoseconds>(p->endTime() - p->startTime()).count();
	//LOG(INFO) << "p->startTime(): " << p->startTime().time_since_epoch().count();

	//LOG(INFO) << "p->endTime(): " << p->endTime().time_since_epoch().count();

	//LOG(INFO) << "run time endTime - startTime (microseconds): " << (p->endTime() - p->startTime()).count();
	//LOG(INFO) << "cpuTime of m: " << p->cpuTime();

	TestThreadObject tto1;
	kul::Ref<TestThreadObject> ref(tto1);
	kul::Thread th(ref);
	th.run();
	th.join();
	th.detach();
	th.interrupt();
	tto1.print();

	TestThreadObject tto2;
	kul::CRef<TestThreadObject> cref(tto2);
	kul::Thread th2(cref);
	th2.run();
	th2.join();
	th2.detach();
	th2.interrupt();
	tto2.print();

	TestThreadObject tto3;
	kul::Thread th1(tto3);
	th1.run();
	th1.join();
	th1.detach();
	th1.interrupt();
	tto3.print();
	
	kul::Mutex mutex;
	{
		{
			kul::ScopeLock lock(mutex);
		}
		kul::ScopeLock lock(mutex);
	}

	KLOG(INFO) << "CANONBALL";
	TestThreadPoolObject ttpo(mutex);
	kul::Ref<TestThreadPoolObject> ref2(ttpo);
	kul::ThreadPool<std::queue<int> > tp(ref2);
	tp.setMax(8);
	tp.runAndJoinAll();
	ttpo.print();
}};


};
#endif /* _KUL_TEST_HPP_ */
	
