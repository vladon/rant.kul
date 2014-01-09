/**

./inc/kul/code/csharp.hpp

Created on: 13 Oct 2013

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
#ifndef _KUL_CODE_CSHARP_HPP_
#define _KUL_CODE_CSHARP_HPP_

#include "kul/code/compiler.hpp"
#include "kul/log.hpp"

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
			KLOG2(COUT, INFO) << cmd;
			
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