/**

./inc/kul/code/cpp.hpp

Created on: 15 Feb 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include "kul/code/compiler.hpp"

namespace kul{ namespace code{ namespace cpp{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class CCompiler : public Compiler{
	protected:
		CCompiler(const int& v) : Compiler(v){}
	public:
		virtual ~CCompiler(){}
		virtual const std::string cc() 	const = 0;
		virtual const std::string cxx() const = 0;
		virtual const std::string getSharedLib(const std::string& lib) const = 0;
		virtual const std::string getStaticLib(const std::string& lib) const = 0;
		bool sourceIsBin() const{ return false; }
};

// Supports all known gcc versions - i.e. known to me
class GCCompiler : public CCompiler{
	public:
		GCCompiler(const int& v = 0) : CCompiler(v){}
		const std::string getSharedLib(const std::string& lib) const {
			return "lib" + lib + ".so";
		}
		const std::string getStaticLib(const std::string& lib) const {
			return "lib" + lib + ".a";
		}
		const std::string buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const hash::set::String& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception) {

			std::string exe = out;
			std::string cmd = linker + " ";
			
			for(const std::string& path : libPaths)
				cmd += "-L" + path + " ";
			
			if(mode == Mode::SHAR)
				cmd += " -shared ";
			else if(mode == Mode::STAT)
				cmd += " -static ";		

			cmd += " -o " + exe;
			for(const std::string& o : objects)
				cmd += " " + o;
			for(const std::string& lib : libs)
				cmd += " -l" + lib + " ";
			cmd += linkerEnd;
			LOG(INFO) << cmd;
			
			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build executable");

			return exe; 
		}
		const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".so");			
			std::string cmd = linker + " -shared -o " + lib;
			for(const std::string& o : objects)
				cmd += " " + o;
			LOG(INFO) << cmd;

			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build shared lib");

			return lib; 
		}
		const std::string buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".a"); 
			std::string cmd = archiver + " " + lib + " ";
			for(const std::string& o : objects)
				cmd += " " + o;
			LOG(INFO) << cmd;

			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build static lib");

			return lib; 
		}
		const std::string compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const hash::set::String& incs, 
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			using namespace kul;
			std::string cmd = compiler + " ";
			std::string obj = out + ".o";

			for(const std::string& s : incs)
				cmd += "-I" + s + " ";
			for(const std::string& s : args)
				cmd += s + " ";

			if(!OS::isDir(OS::dirDotDot(out))) OS::mkDir(OS::dirDotDot(out));
			cmd += " -o " + obj + " -c " + in;
			LOG(INFO) << cmd;

			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to compile source");
			
			return obj;
		}
		virtual void preCompileHeader(			
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			using namespace kul;
			
			if(in.rfind(".") == std::string::npos)
				throw Exception(__FILE__, __LINE__, "Unknown header type");
			
			std::string cmd;// = compiler + " -x";
			std::string h = in.substr(in.rfind(".") + 1);

			if(h.compare("h") == 0)
				cmd = cc() + " -x c-header ";
			else
			if(h.compare("hpp") == 0)
				cmd = cxx() + " -x c++-header ";
			else
				throw Exception(__FILE__, __LINE__, "Failed to pre-compile header - uknown file type: " + h);
			cmd += in + " ";
			for(const std::string& s : args)
				cmd += s + " ";
			for(const std::string& s : incs)
				cmd += "-I" + s + " ";

			if(!OS::isDir(OS::dirDotDot(out))) OS::mkDir(OS::dirDotDot(out));
			cmd += " -o " + out;
			LOG(INFO) << cmd;

			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to pre-compile header");
		}
		virtual const std::string cc() const {
			return "gcc";
		}
		virtual const std::string cxx() const {
			return "g++";
		}
};

class ClangCompiler : public GCCompiler{
	public:
		ClangCompiler(const int& v = 0) : GCCompiler(v){}
		virtual const std::string cc() const {
			return "clang";
		}
		virtual const std::string cxx() const {
			return "clang++";
		}
};


class WINCompiler : public CCompiler{
	public:
		WINCompiler(const int& v = 0) : CCompiler(v){}
		virtual const std::string cc() const {
			return "cl";
		}
		virtual const std::string cxx() const {
			return "cl";
		}
		const std::string getSharedLib(const std::string& lib) const {
			return lib + ".dll";
		}
		const std::string getStaticLib(const std::string& lib) const {
			return lib + ".lib";
		}
		const std::string buildExecutable(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects,
			const std::vector<std::string>& libs,
			const hash::set::String& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception){ 

			std::string exe = out + ".exe"; 
			std::string cmd = linker + " /NOLOGO /LTCG ";
			
			for(const std::string& path : libPaths)
				cmd += " /LIBPATH:\"" + path + "\" ";
			/*
			if(mode == Mode::SHAR)
				cmd += " /MD ";
			else if(mode == Mode::STAT)
				cmd += " /MT ";		*/

			cmd += " /OUT:\"" + exe + "\" ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";

			if(mode == Mode::SHAR)
				for(const std::string& lib : libs)
					cmd += " \"" + getSharedLib(lib) + "\" ";
			else if(mode == Mode::STAT)
				for(const std::string& lib : libs)
					cmd += " \"" + getStaticLib(lib) + "\" ";

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

			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".lib"); 
			std::string cmd = archiver + " /OUT:\"" + lib + "\" /NOLOGO /LTCG ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			LOG(INFO) << cmd;

			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build static lib");

			return lib;
		}
		const std::string compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const hash::set::String& incs, 
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			using namespace kul;
			std::string cmd = compiler + " ";
			std::string obj = out + ".o";

			for(const std::string& s : incs)
				cmd += "\"/I" + s + "\" ";
			for(const std::string& s : args)
				cmd += s + " ";

			if(!OS::isDir(OS::dirDotDot(OS::localPath(out)))) OS::mkDir(OS::dirDotDot(OS::localPath(out)));
			cmd += " -c \"/Fo" + obj + "\" \"" + in + "\"";			
			LOG(INFO) << cmd;
			
			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to compile source");
			
			return obj;
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
#endif /* _KUL_CODE_CPP_HPP_ */