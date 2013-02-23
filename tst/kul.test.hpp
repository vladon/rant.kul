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
#include "kul/alloc.hpp"
#include "kul/smart.hpp"
#include "kul/ext/google.hpp"

namespace kul {
class test{ public: test(){

	kul::smart::Array<int> a;
	a.add()(new int(1)) (new int(2)) (new int(3));
	for(const int& ii : a) LOG(INFO) << ii;

	LOG(INFO) << OS::dirSep();
	LOG(INFO) << OS::pwd();
	LOG(INFO) << OS::userDir();
	LOG(INFO) << OS::userAppDir("scaa");
	for(std::string s : OS::dirs()){
		LOG(INFO) << s;
	}
	for(std::string s : OS::files(OS::pwd(), true)){
		LOG(INFO) << s;
	}
}};
};


#endif /* _KUL_TEST_HPP_ */
