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
#include "kul/ext/google.hpp"

namespace kul {
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
	(*p).addArg("Hello").addArg("World").start();

    LOG(INFO) << std::chrono::duration_cast<std::chrono::nanoseconds>(p->endTime() - p->startTime()).count();
    LOG(INFO) << "p->startTime(): " << p->startTime().time_since_epoch().count();

    LOG(INFO) << "p->endTime(): " << p->endTime().time_since_epoch().count();

    LOG(INFO) << "run time endTime - startTime (microseconds): " << (p->endTime() - p->startTime()).count();
    LOG(INFO) << "cpuTime of m: " << p->cpuTime();
}};


};
#endif /* _KUL_TEST_HPP_ */
