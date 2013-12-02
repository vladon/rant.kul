/*
 * compiler.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_CSHARP_HPP_
#define _KUL_CODE_CSHARP_HPP_

#include "kul/code/compiler.hpp"

namespace kul{ namespace code{ namespace csharp{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class WINCompiler : public Compiler{
	public:
		WINCompiler(const int& v = 0) : Compiler(v){}
		bool sourceIsBin()	const{ return true; }
		const std::string buildExecutable(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects,
			const std::vector<std::string>& libs,
			const hash::set::String& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception){
			
			std::string exe = out + ".exe"; 
			std::string cmd = linker + " /NOLOGO ";
			
			for(const std::string& path : libPaths)
				cmd += " /LIBPATH:\"" + path + "\" ";

			cmd += " /out:\"" + exe + "\" ";
			for(const std::string& o : objects)
				cmd += " " + o + " ";

			for(const std::string& lib : libs)
				cmd += " /reference:" + lib + ".dll ";			

			cmd += linkerEnd;
			LOG(INFO) << cmd;

			LOG(INFO) << kul::OS::getEnvVar("PATH");
			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build executable");

			return exe; 
		}
		const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".dll"); 
			return lib;
		}
		const std::string buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			throw Exception(__FILE__, __LINE__, "No static libraries in C Sharp");
		}
		const std::string compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const hash::set::String& incs, 
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){

			return in;
		}
		virtual void preCompileHeader(			
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			throw Exception(__FILE__, __LINE__, "Method is not implemented");
		}
};

};};};
#endif /* _KUL_CODE_CSHARP_HPP_ */