/**

./tst/kul.test.hpp

Created on: 24 Jan 2013

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
#ifndef _KUL_TEST_HPP_
#define _KUL_TEST_HPP_

#include "kul/os.hpp"
#include "kul/cli.hpp"
#include "kul/log.hpp"
#include "kul/xml.hpp"
#include "kul/time.hpp"
#include "kul/proc.hpp"
#include "kul/threads.hpp"

namespace kul {
class TestThreadObject{
	private:
		int i;
	public:
		TestThreadObject() : i(0){}
		void operator()(){
		   	KLOG(INF) << "THREAD RUNNING";
			i++;
		}
		void operator()() const{
			KLOG(INF) << "CONST THREAD RUNNING";
		}
		void print(){ KLOG(INF) << "i = " << i;}
};

class TestThreadPoolObject{
	private:
		int i;
		Mutex& mutex;
	public:
		TestThreadPoolObject(Mutex& mutex) : i(0), mutex(mutex){}
		void operator()(){
		   	kul::ScopeLock lock(mutex);
		   	KLOG(INF) << "THREAD RUNNING";			
			i++;
			KLOG(INF) << "THREAD FINISHED";
		}
		void operator()() const{
			KLOG(INF) << "CONST THREAD RUNNING";
		}
		void print(){ KLOG(INF) << "i = " << i;}
};

class test{ public: test(){
	KLOG(INF);
	KLOG(ERR);
	KLOG(DBG);
	KLOG(INF) << Dir::SEP();
	KLOG(INF) << Env::CWD();
	KLOG(INF) << os::userDir().real();
	KLOG(INF) << os::userAppDir("maiken").real();
	for(kul::Dir d : kul::Dir(Env::CWD()).dirs()) for(kul::File f : d.files()) KLOG(INF) << f.real();

	std::unique_ptr<kul::Process> p(kul::Process::create("echo"));
	try{
		(*p).addArg("Hello").addArg("World").start();
	}catch(const kul::proc::Exception& e){ 
		KLOG(INF) << e.debug()<< " : " << typeid(e).name();
		KLOG(INF) << "Error expected on windows without echo on path"; //it's a built in command in cmd
	}

	for(const std::string& arg : kul::cli::CmdLine::asArgs("/path/to \"words in quotes\" words\\ not\\ in\\ quotes end"))
		KLOG(INF) << "ARG: " << arg;

	KLOG(INF) << "kul::Now::MILLIS(); " << kul::Now::MILLIS();
	KLOG(INF) << "kul::Now::MICROS(); " << kul::Now::MICROS();
	KLOG(INF) << "kul::Now::NANOS();  " << kul::Now::NANOS();

	KLOG(INF) << "kul::DateTime::NOW();  " << kul::DateTime::NOW();

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

	KLOG(INF) << "LAUNCHING THREAD POOL";
	TestThreadPoolObject ttpo(mutex);
	kul::Ref<TestThreadPoolObject> ref2(ttpo);
	kul::ThreadPool<std::queue<int> > tp(ref2);
	tp.setMax(8);
	tp.runAndJoinAll();
	ttpo.print();

	KLOG(INF) << "CPU CORES: " << kul::cpu::cores();
	KLOG(INF) << "MAX THREADS: " << kul::cpu::threads();
}};


}
#endif /* _KUL_TEST_HPP_ */
	
