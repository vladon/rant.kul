/**

./src/Args.cpp

Created on: 14 Feb 2013

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
#include <string.h>

#include "kul/io.hpp"
#include "kul/cli.hpp"
#include "kul/string.hpp"

void kul::cli::Args::process(int argc, char* argv[]) throw(ArgNotFoundException) {
	std::string cliArg = "";
	for(int i = 1; i < argc; i++)
		cliArg += std::string(argv[i]) + " ";

	for(const Arg& a1 : arguments())
		for(const Arg& a2 : arguments()){
			if(&a1 == &a2) continue;
			if((a1.dash() != ' ') && (a1.dash() == a2.dash()
					|| strcmp(a1.dashdash(), a2.dashdash()) == 0))
				KEXCEPT(Exception, "Duplicate argument detected");
		}
	for(const Cmd& c1 : commands())
		for(const Cmd& c2 : commands()){
			if(&c1 == &c2) continue;
			if(strcmp(c1.command(), c2.command()) == 0)
				KEXCEPT(Exception, "Duplicate argument detected");
		}

	Arg* arg = 0;
	int valExpected = 0;
	std::string valExpectedFor, c, t;

	for(int i = 1; i < argc; i++){
		c = argv[i];
		t = c;

		if(c.compare("---") == 0)	KEXCEPT(Exception, "Illegal argument ---");
		if(c.compare("--") == 0)	KEXCEPT(Exception, "Illegal argument --");
		if(c.compare("-") == 0) 	KEXCEPT(Exception, "Illegal argument -");
		if(valExpected == 1 || (valExpected == 2 && c.find("-") != 0)){
			valExpected = 0;
			vals[arg->dashdash()] = c;
			arg = 0;
			continue;
		}else if(valExpected == 2) valExpected = 0;

		if(c.find("--") == 0){
			c = c.substr(c.find("--") + 2);
			valExpectedFor = c;
			if(c.find("=") == std::string::npos){
				arg = const_cast<Arg*>(&doubleDashes(c.c_str()));
				valExpected = arg->type();
				if(!valExpected) vals[arg->dashdash()] = "";
				continue;
			}
			valExpectedFor = c.substr(0, c.find("="));
			arg = const_cast<Arg*>(&doubleDashes(valExpectedFor.c_str()));
			valExpected = arg->type();
			if(valExpected == 0) 
				KEXCEPT(Exception, "Found = when no value is expected for arg " + valExpectedFor);
			c = c.substr(c.find("=") + 1);
			vals[arg->dashdash()] = c;
		}
		else if(c.find("-") == 0){
			valExpectedFor = c;
			c = c.substr(c.find("-") + 1);
			if(c.find("=") != std::string::npos){
				if(c.substr(0, c.find("=")).size() > 1)
					KEXCEPT(Exception, "Cannot mix flag and non-flag arguments");
				arg = const_cast<Arg*>(&dashes(c.at(0)));
				vals[arg->dashdash()] = c.substr(c.find("=") + 1);
				valExpected = 0;
			}else if(c.length() > 1){
				std::string a = c;
				for(unsigned int i = 0; i < c.length(); i++){
					arg = const_cast<Arg*>(&dashes(a.at(0)));
					if(i + 1 == c.length())
						valExpected = arg->type();
					else if(arg->type() == ArgType::STRING)
						KEXCEPT(Exception, "Cannot mix flag and non-flag arguments");
					vals[arg->dashdash()] = "";
					if(a.length() > 1) a = a.substr(1);
				}
			}else{
				arg = const_cast<Arg*>(&dashes(c.at(0)));
				valExpected = arg->type();
				vals[arg->dashdash()] = "";
			}
			continue;
		}
		if(valExpected == 1){
			valExpected = 0;
			continue;
		}
		commands(c.c_str());
		vals[c] = "";
	}
	if(valExpected == 1) KEXCEPT(Exception, "Value expected for argument: \""+ valExpectedFor + "\"");
}
