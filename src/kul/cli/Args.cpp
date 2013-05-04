/*
 * Args.cpp
 *
 * Created on: Feb 14, 2013
 * Author: philip
 */

#include "glog/logging.h"

#include <string.h>

#include "kul/string.hpp"
#include "kul/cli.hpp"


void kul::cli::Args::process(int argc, char* argv[]){
	std::string cliArg = "";
	for(int i = 1; i < argc; i++)
		cliArg += std::string(argv[i]) + " ";

	for(const Arg& a : arguments())
		if(strlen(a.dash()) > 1)
			throw Exception(__FILE__, __LINE__, "Short param too long, only one character allowed");

	for(const Arg& a1 : arguments())
		for(const Arg& a2 : arguments()){
			if(&a1 == &a2) continue;
			if(strcmp(a1.dash(), a2.dash()) == 0
					|| strcmp(a1.dashdash(), a2.dashdash()) == 0)
				throw Exception(__FILE__, __LINE__, "Duplicate argument detected");
		}
	for(const Cmd& c1 : commands())
		for(const Cmd& c2 : commands()){
			if(&c1 == &c2) continue;
			if(strcmp(c1.command(), c2.command()) == 0)
				throw Exception(__FILE__, __LINE__, "Duplicate argument detected");
		}

	Arg* arg = 0;
	bool valExpected = false;
	std::string valExpectedFor = "";
	std::string c= "";
	std::string t = "";
	for(int i = 1; i < argc; i++){
		c = argv[i];
		t = c;

		kul::st_d::String::replaceAll(t, "----", "---");
		if(t.find("---") != std::string::npos) throw Exception(__FILE__, __LINE__, "Illegal argument " + c);
		if(c.compare("---") == 0)	throw Exception(__FILE__, __LINE__, "Illegal argument ---");
		if(c.compare("--") == 0)	throw Exception(__FILE__, __LINE__, "Illegal argument --");
		if(c.compare("-") == 0) throw Exception(__FILE__, __LINE__, "Illegal argument -");
		if((c.find("--") == 0 || c.find("-") == 0) && valExpected)
			throw Exception(__FILE__, __LINE__, "Value expected for argument: \""+ valExpectedFor + "\"");

		if(valExpected){
			valExpected = !valExpected;
			vals[arg->dashdash()] = c;
			arg = 0;
			continue;
		}

		if(c.find("--") == 0){
			valExpectedFor = c;
			c = c.substr(c.find("--"));
			arg = const_cast<Arg*>(&doubleDashes(c.c_str()));
			valExpected = arg->valueExpected();
			if(c.find("=") == std::string::npos)
				continue;
			c = c.substr(c.find("="));
		}
		if(c.find("-") == 0){
			valExpectedFor = c;
			c = c.substr(c.find("-") + 1);
			if(c.find("=") != std::string::npos){
				if(c.substr(0, c.find("=")).size() > 1)
					throw Exception(__FILE__, __LINE__, "Cannot mix flag and non-flag arguments");
				arg = const_cast<Arg*>(&dashes(c.substr(0, c.find("=")).c_str()));
				vals[arg->dashdash()] = c.substr(c.find("=") + 1);
				valExpected = 0;
			}else if(c.length() > 1){
				for(unsigned int i = 0; i < c.length(); i++){
					char ch = c[i];
					arg = const_cast<Arg*>(&dashes(&ch));
					if(arg->valueExpected())
						throw Exception(__FILE__, __LINE__, "Cannot mix flag and non-flag arguments");
					vals[arg->dashdash()] = "";
				}
			}else{
				arg = const_cast<Arg*>(&dashes(c.c_str()));

				valExpected = arg->valueExpected();
				if(!valExpected)
					vals[arg->dashdash()] = "";
			}
			continue;
		}
		if(valExpected){
			valExpected = !valExpected;
			continue;
		}
		commands(c.c_str());
		vals[c] = "";
	}
	if(valExpected)
		throw Exception(__FILE__, __LINE__, "Value expected for argument: \""+ valExpectedFor + "\"");
}
