/*
 * compiler.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_COMPILER_HPP_
#define _KUL_CODE_COMPILER_HPP_

#include "glog/logging.h"

#include "kul/os.hpp"
#include "kul/cli.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"
#include "kul/except.hpp"

#include "kul/ext/google.hpp"

namespace kul{ namespace code{ 

enum Mode { NONE = 0, STAT, SHAR};

class Compiler{	
	protected:
		Compiler(const int& v) : version(v){}
		const int version;
		kul::Process* setupProcess(const std::string& c, const std::string& cmdline, const std::vector<kul::cli::EnvVar>& evs) const {
			
			std::vector<std::string> bits = kul::cli::CmdLine::asArgs(cmdline);

			std::string cmd = c;
			std::string path;
			if(c.find(" ") != std::string::npos)
				for(const std::string& s : kul::String::split(c, ' ')){
					cmd = s;
					break;	
				}
			if(kul::OS::localPath(cmd).find(kul::OS::dirSep()) != std::string::npos){
				path = cmd.substr(0, cmd.rfind(kul::OS::dirSep()));
				cmd = cmd.substr(cmd.rfind(kul::OS::dirSep()) + 1);
			}
			
			kul::Process* p;
			if(path.empty()) 	p = kul::Process::create(cmd);
			else 				p = kul::Process::create(path, cmd);

			bits.erase(bits.begin());			
			for(const std::string& s : bits)
				p->addArg(s);			
			for(const kul::cli::EnvVar& ev : evs)
				p->addEnvVar(ev.name(), ev.toString());

			return p;
		}
	public:
		virtual ~Compiler(){}		
		virtual bool sourceIsBin()		const = 0;
		virtual const std::string buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) 			const throw (kul::Exception) = 0;
		virtual const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const throw (kul::Exception) = 0;
		virtual const std::string buildStaticLibrary(
			const std::string& archiver,
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const throw (kul::Exception) = 0;
		virtual const std::string compileSource	(
			const std::string& compiler,
			const std::vector<std::string>& args, 		
			const kul::StringHashSet& incs, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
		virtual void preCompileHeader(
			const kul::StringHashSet& incs, 
			const kul::StringHashSet& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
};

};};
#endif /* _KUL_CODE_COMPILER_HPP_ */