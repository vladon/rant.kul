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
#include "kul/ipc.hpp"
#include "kul/log.hpp"
#include "kul/math.hpp"
#include "kul/proc.hpp"
#include "kul/time.hpp"
#include "kul/threads.hpp"

#include <iomanip>

namespace kul {
class TestThreadObject{
	private:
		int i;
	public:
		TestThreadObject() : i(0){}
		void print(){ KLOG(INF) << "i = " << i;}
		friend class kul::ThreadCopy<TestThreadObject>;
		friend class kul::ThreadRef<TestThreadObject>;
	protected:
		void operator()(){
			KLOG(INF) << "THREAD RUNNING";
			i++;
			KLOG(INF) << "THREAD FINISHED";
		}
	public:
		void operator()() const {
			KLOG(INF) << "CONST THREAD RUNNING";
			KLOG(INF) << "CONST THREAD FINISHED";
		}
};

class TestThreadPoolObject{
	protected:
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
		void print(){ KLOG(INF) << "i = " << i;}
};

class TestThreadPoolQObject : public TestThreadPoolObject{
	private:
		std::queue<int>& q;
	public:
		TestThreadPoolQObject(Mutex& mutex, std::queue<int>& q) : TestThreadPoolObject(mutex), q(q){}
		void operator()(){
		   	kul::ScopeLock lock(mutex);
		   	KLOG(INF) << "THREAD RUNNING";
			i++;
			q.pop();
			KLOG(INF) << "THREAD FINISHED";
		}
		void print(){ KLOG(INF) << "i = " << i;}
};

class TestIPCServer : public kul::ipc::Server{
	public:
		TestIPCServer() : kul::ipc::Server("uuid", 1){} // UUID 	CHECKS ONCE
		void handle(const std::string& s){
			KLOG(INF) << "TestIPCServer " << s;
		}
};
class TestIPCThread{
	public:
		void operator()(){
			TestIPCServer().listen();
		}
};
class TestIPC{
	public:
		TestIPC(){
			TestIPCThread ipc;
			kul::Ref<TestIPCThread> ref(ipc);
			kul::Thread t(ref);
			t.run();
			kul::this_thread::sleep(1000);
			kul::ipc::Client("uuid").send("TestIPCClient");
			t.join();
		}
};

class Test{
	public:
		Test(){
			KERR << "KERR";
			KOUT(NON) << "KOUT(NON)";
			KOUT(INF) << "KOUT(INF)";
			KOUT(ERR) << "KOUT(ERR)";
			KOUT(DBG) << "KOUT(DBG)";
			KLOG(INF) << "KLOG(INF)";
			KLOG(ERR) << "KLOG(ERR)";
			KLOG(DBG) << "KLOG(DBG)";
			KOUT(NON) << kul::Dir::SEP();
			KOUT(NON) << kul::env::SEP();
			KOUT(NON) << kul::env::CWD();
			KOUT(NON) << kul::os::userDir().path();
			KLOG(INF) << kul::os::userAppDir("maiken").path();
			for(const kul::Dir& d : kul::Dir(kul::env::CWD()).dirs())
				for(const kul::File& f : d.files())
					KOUT(NON) << d.join(f.name());
			try{
				kul::Process("echo").arg("Hello").arg("World").start();
			}catch(const kul::proc::Exception& e){
				KERR << e.debug()<< " : " << typeid(e).name();
				KERR << "Error expected on windows without echo on path";
			}

			for(const std::string& arg : kul::cli::CmdLine::asArgs("/path/to \"words in quotes\" words\\ not\\ in\\ quotes end"))
				KOUT(NON) << "ARG: " << arg;

			KOUT(NON) << "kul::Now::MILLIS(); " << kul::Now::MILLIS();
			KOUT(NON) << "kul::Now::MICROS(); " << kul::Now::MICROS();
			KOUT(NON) << "kul::Now::NANOS();  " << kul::Now::NANOS();

			KOUT(NON) << "kul::DateTime::NOW();  " << kul::DateTime::NOW();

			KOUT(NON) << "CPU CORES: " << kul::cpu::cores();
			KOUT(NON) << "MAX THREADS: " << kul::cpu::threads();

			TestThreadObject tto1;
			kul::Ref<TestThreadObject> ref(tto1);
			kul::Thread th(ref);
			th.detach();
			th.run();
			th.join();
			tto1.print();

			TestThreadObject tto2;
			kul::Ref<const TestThreadObject> cref(tto2);
			kul::Thread th2(cref);
			th2.detach();
			th2.run();
			th2.join();
			tto2.print();

			TestThreadObject tto3;
			kul::Thread th1(tto3);
			th1.detach();
			th1.run();
			th1.join();
			tto3.print();

			kul::Mutex mutex;
			{
				{
					kul::ScopeLock lock(mutex);
				}
				kul::ScopeLock lock(mutex);
			}

			KOUT(NON) << "LAUNCHING THREAD POOL";
			TestThreadPoolObject ttpo1(mutex);
			kul::Ref<TestThreadPoolObject> ref2(ttpo1);
			kul::ThreadPool tp1(ref2);
			tp1.setMax(4);
			tp1.detach();
			tp1.run();
			tp1.join();
			ttpo1.print();

			std::queue<int> q;
			for(int i = 0; i < 10; i++) q.push(i);
			KOUT(NON) << "LAUNCHING PREDICATED THREAD POOL";
			TestThreadPoolQObject ttpo2(mutex, q);
			kul::Ref<TestThreadPoolQObject> ref3(ttpo2);
			kul::PredicatedThreadPool<std::queue<int> > tp2(ref3, q);
			tp2.setMax(kul::cpu::threads());
			tp2.detach();
			tp2.run();
			tp2.join();
			ttpo2.print();

			TestIPC ipc;

			KOUT(NON) << kul::math::abs(-1);

			KOUT(NON) << kul::math::root(16);
			KOUT(NON) << kul::math::root(64, 3);
			KOUT(NON) << std::setprecision(16) << kul::math::root<double>(64, 6);
			KOUT(NON) << kul::math::root(64, 6, 8);
			KOUT(NON) << kul::math::root(64, 6, 3, 3);
			// kul::math::root(root of, nth root, iterations, starting guess);

			KOUT(NON) << std::setprecision(6) << kul::math::root(2, 2);
			KOUT(NON) << std::setprecision(16) << kul::math::root<double>(2, 3);

			KOUT(NON) << kul::math::pow(-2, 0);
			KOUT(NON) << kul::math::pow(2, 5);
			KOUT(NON) << kul::math::pow(2, -5);
			KOUT(NON) << kul::math::pow(2.2974, 5);
			KOUT(NON) << kul::math::pow(2, 6);
		}
};


}
#endif /* _KUL_TEST_HPP_ */

