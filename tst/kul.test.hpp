/*
 * test.hpp
 *
 *  Created on: 24 Jan 2013
 *      Author: philix
 */

#ifndef _KUL_TEST_HPP_
#define _KUL_TEST_HPP_

#include "glog/logging.h"

#include "kul/os.hpp"
#include "kul/xml.hpp"
#include "kul/proc.hpp"
#include "kul/alloc.hpp"
#include "kul/smart.hpp"
#include "kul/threading.hpp"


namespace kul {
class TestThreadObject{
	private:
		int i;
	public:
		TestThreadObject() : i(0){}
		void operator()(){
			i++;
			LOG(INFO) << "THREAD RUNNING";
		}
		void operator()() const{			
			LOG(INFO) << "CONST THREAD RUNNING";
		}
		void print(){ LOG(INFO) << "i = " << i;}
};

class test{ public: test(){
	kul::smart::Array<int> a;
	a.add()(new int(1)) (new int(2)) (new int(3));
	for(const int& ii : a) LOG(INFO) << ii;
	for(int i = 0; i < a.size(); i++) LOG(INFO) << a[i];	

	LOG(INFO) << OS::dirSep();
	LOG(INFO) << OS::pwd();
	LOG(INFO) << OS::userDir();
	LOG(INFO) << OS::userAppDir("maiken");
	for(std::string s : OS::dirs()){
		LOG(INFO) << s;
	}
	for(std::string s : OS::files(OS::pwd(), true)){
		LOG(INFO) << s;
	}

	std::shared_ptr<kul::proc::CPUMonitoredProcess> p(kul::proc::CPUMonitoredProcess::create("echo"));
	try{
		(*p).addArg("Hello").addArg("World").start();
	}catch(const kul::proc::Exception& e){ 
		LOG(INFO) << e.debug()<< " : " << typeid(e).name();
		exit(1);
	}

	

	for(const std::string& arg : kul::cli::CmdLine::asArgs("/path/to \"words in quotes\" words\\ not\\ in\\ quotes end"))
		LOG(INFO) << "ARG: " << arg;

    //LOG(INFO) << std::chrono::duration_cast<std::chrono::nanoseconds>(p->endTime() - p->startTime()).count();
    //LOG(INFO) << "p->startTime(): " << p->startTime().time_since_epoch().count();

    //LOG(INFO) << "p->endTime(): " << p->endTime().time_since_epoch().count();

    //LOG(INFO) << "run time endTime - startTime (microseconds): " << (p->endTime() - p->startTime()).count();
    //LOG(INFO) << "cpuTime of m: " << p->cpuTime();

	TestThreadObject tto1;
	kul::threading::Ref<TestThreadObject> ref(tto1);
    kul::threading::Thread th(ref);
    th.run();
    th.join();
    th.detach();
    th.interrupt();
    tto1.print();

	TestThreadObject tto2;
    kul::threading::CRef<TestThreadObject> cref(tto2);
    kul::threading::Thread th2(cref);
    th2.run();
    th2.join();
    th2.detach();
    th2.interrupt();
    tto2.print();

	TestThreadObject tto3;
    kul::threading::Thread th1(tto3);
    th1.run();
    th1.join();
    th1.detach();
    th1.interrupt();
    tto3.print();

    kul::threading::Mutex mutex;
    {
    	kul::threading::ScopeLock lock(mutex);
    }
    kul::threading::ScopeLock lock(mutex);
}};


};
#endif /* _KUL_TEST_HPP_ */
	