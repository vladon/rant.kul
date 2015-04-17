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
#include "kul/http.hpp"
#include "kul/proc.hpp"
#include "kul/time.hpp"
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
			KLOG(INF) << "THREAD FINISHED";
		}
		void operator()() const{
			KLOG(INF) << "CONST THREAD RUNNING";
			KLOG(INF) << "CONST THREAD FINISHED";
		}
		void print(){ KLOG(INF) << "i = " << i;}
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
		TestIPCServer(const std::string& uuid) : kul::ipc::Server(uuid, 1){} // UUID 	CHECKS ONCE
		void handle(const std::string& s){
			KLOG(INF) << "TestIPCServer " << s;
		}
};
class TestIPCThread{
	public: 
		void operator()(){
			TestIPCServer("uuid").listen();
		}
};
class TestIPC{ 
	public: 
		TestIPC(){
			TestIPCThread ipc;
			kul::Thread t(ipc);
			t.run();
			kul::this_thread::sleep(500);
			kul::ipc::Client("uuid").send("TestIPCClient");
		}
};

class TestHTTPServer : public kul::http::Server{
	public:
		TestHTTPServer() : kul::http::Server(666){}
		const std::string handle(const std::string& res, const std::string& atts){
			return "TestHTTPServer " + res + " : " + atts; 
		}
};
class TestGetRequest : public kul::http::_1_1GetRequest{
	public:
		void handle(const std::string& s){
			KOUT(NON) << s;
		}
};
class TestHTTPThread{
	private:
		TestHTTPServer& s;
	public:
		TestHTTPThread(TestHTTPServer& serv) : s(serv){}
		void operator()(){
			s.listen();
		}
};
class TestHTTP{ 
	public: 
		TestHTTP(){
			TestHTTPServer serv;
			TestHTTPThread http(serv);
			kul::Thread t(http);
			t.run();
			kul::this_thread::sleep(500);
			TestGetRequest().send("localhost", "index.html", 666);
			serv.stop();
			t.join();
		}
};

class test{ 
	public: 
		test(){
			KLOG(INF);
			KLOG(ERR);
			KLOG(DBG);
			KLOG(INF) << kul::Dir::SEP();
			KLOG(INF) << kul::Env::SEP();
			KLOG(INF) << kul::Env::CWD();
			KLOG(INF) << kul::os::userDir().path();
			KLOG(INF) << kul::os::userAppDir("maiken").path();
			for(const kul::Dir& d : kul::Dir(kul::Env::CWD()).dirs()) 
				for(const kul::File& f : d.files()) KLOG(INF) << d.join(f.name());

			kul::Process p("echo");
			try{
				p.addArg("Hello").addArg("World").start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				KLOG(INF) << "Error expected on windows without echo on path";
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

			KLOG(INF) << "LAUNCHING THREAD POOL";
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
			KLOG(INF) << "LAUNCHING PREDICATED THREAD POOL";
			TestThreadPoolQObject ttpo2(mutex, q);
			kul::Ref<TestThreadPoolQObject> ref3(ttpo2);
			kul::PredicatedThreadPool<std::queue<int> > tp2(ref3, q);
			tp2.setMax(kul::cpu::threads());
			tp2.detach();
			tp2.run();
			tp2.join();
			ttpo2.print();

			KLOG(INF) << "CPU CORES: " << kul::cpu::cores();
			KLOG(INF) << "MAX THREADS: " << kul::cpu::threads();

			try{
				TestIPC ipc;
				TestHTTP http;
			}
			catch(const kul::Exception& e){ KOUT(NON) << e.what(); }
			catch(const std::exception& e){ KOUT(NON) << e.what(); }
		}
};


}
#endif /* _KUL_TEST_HPP_ */
	
