/*
 * cli.hpp
 *
 *  Created on: 13 Feb 2013
 *      Author: philix
 */

#ifndef _CLI_HPP_
#define _CLI_HPP_

#include "glog/logging.h"

#include <string>
#include <vector>

#include "kul/os.hpp"
#include "kul/except.hpp"
#include "kul/string.hpp"
#include "kul/ext/google.hpp"

namespace kul{  namespace cli {

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ArgNotFoundException : public Exception{
	public:
		ArgNotFoundException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};

class ArgParsingException : public Exception{
	public:
		ArgParsingException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};

class CmdLine{
	public:
		static std::vector<std::string> asArgs(const std::string& cmd) throw(ArgParsingException);
};

class Cmd{
	private:
		const char* c;
		const char* h;
	public:
		Cmd(const char* c, const char* h = "") : c(c), h(h){}
		const char* command() 	const { return c;}
		const char* help() 		const { return h;}
};

enum EnvVarMode{ APPE = 0, PREP, REPL};

class EnvVar{
	private:
		const std::string n;
		const std::string v;
		const EnvVarMode m;
	public:
		EnvVar(const std::string n, const std::string v, const EnvVarMode m) : n(n), v(v), m(m){}
		const char* 		name() 		const { return n.c_str(); }
		const char* 		value() 	const { return v.c_str(); }
		const EnvVarMode 	mode() 		const { return m; }
		const std::string 	toString() 	const {
			std::string var(value());
			kul::String::replaceAll(var, ";", kul::OS::pathSep());
			kul::String::replaceAll(var, ":", kul::OS::pathSep());
			kul::String::replaceAll(var, kul::OS::newLine(), "");
			kul::String::trim(var);
			std::string ev(OS::getEnvVar(name()));			
			if(ev.compare("") != 0){
				if 		(mode() == EnvVarMode::PREP)
					var = var + std::string(kul::OS::pathSep()) + ev;	
				else if (mode() == EnvVarMode::APPE)				
					var = ev + std::string(kul::OS::pathSep()) + var;	
			}
			return var;
		}
};

enum ArgType{ FLAG = 0, STRING, INT};

class Arg : public Cmd{
	private:
		const char* d;
		ArgType t;
	public:		
		Arg(const char* d, const char* dd, ArgType t, const char* h = "") : Cmd(dd, h), d(d), t(t){}
		Arg(const char* d, const char* dd, const char* h = "") : Cmd(dd, h), d(d), t(ArgType::FLAG){}
		const char* dash() const { return d;}
		const char* dashdash() const { return command();}
		const bool valueExpected() const { return t != ArgType::FLAG;}
		const ArgType& type() const { return t; }
};

class Args{
	private:
		const std::vector<Cmd>& cmds;
		const std::vector<Arg>& args;
		kul::StringToStringHashMap vals;
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
		const kul::StringToStringHashMap& values() 	const { return vals; }
};

class AWritable{
	public:
		virtual const char* msg() const = 0;
};

class AWriter{
	protected:
		virtual void error(const AWritable& a) const = 0;
};


};};
#endif /* _CLI_HPP_ */
