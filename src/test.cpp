/*
 * test.cpp
 *
 *  Created on: 23 Jan 2013
 *      Author: philix
 */

#include "kul/smart.hpp"

void kul::smart::test::Test(){

	int* in = new int(1);
	in[1] = 2;
	in[2] = 3;

	kul::smart::Array<int> a(in);

	for(const int& ii : a){}

}


