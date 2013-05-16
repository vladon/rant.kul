/*
 * cli.hpp
 *
 *  Created on: 13 Feb 2013
 *      Author: philix
 */

#ifndef _CLI_HPP_
#define _CLI_HPP_

#include "glog/logging.h"

#include "kul/ext/google.hpp"

#include <string>
#include <vector>

#include "kul/except.hpp"

namespace kul{  namespace cli {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ArgNotFoundException : public Exception{
	public:
		ArgNotFoundException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};

class Cmd{
	private:
		const char* c;
	public:
		Cmd(const char* c) : c(c){}
		const char* command() const { return c;}
};

enum ArgType{ FLAG = 0, STRING, INT};

class Arg : public Cmd{
	private:
		const char* d;
		ArgType t;
	public:		
		Arg(const char* d, const char* dd, ArgType t) : Cmd(dd), d(d), t(t){}
		Arg(const char* d, const char* dd) : Cmd(dd), d(d), t(ArgType::FLAG){}
		const char* dash() const { return d;}
		const char* dashdash() const { return command();}
		const bool valueExpected() const { return t != ArgType::FLAG;}
		const ArgType& type() const { return t; }
};

class Args{
	private:
		const std::vector<Cmd>& cmds;
		const std::vector<Arg>& args;
		kul::ext::goo_gle::StringToStringHashMap vals;
		Args();
	public:
		Args(const std::vector<Cmd>& cmds, const std::vector<Arg>& args) : cmds(cmds), args(args){}
		void process(int argc, char* argv[]);
		const Cmd& commands(const char* c){
			for(const Cmd& cmd : cmds) if(strcmp(cmd.command(), c) == 0) return cmd;
			throw ArgNotFoundException(__FILE__, __LINE__, "No command " + std::string(c) + " found");
		}
		const Arg& dashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dash(), c) == 0) return a;
			throw ArgNotFoundException(__FILE__, __LINE__, "No argument " + std::string(c) + " found");
		}
		const Arg& doubleDashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dashdash(), c) == 0) return a;
			throw ArgNotFoundException(__FILE__, __LINE__, "No argument " + std::string(c) + " found");
		}
		const std::vector<Cmd>& commands()	 						const { return cmds;}
		const std::vector<Arg>& arguments() 						const { return args;}
		const kul::ext::goo_gle::StringToStringHashMap& values() 	const { return vals; }
};




};};
#endif /* _CLI_HPP_ */
