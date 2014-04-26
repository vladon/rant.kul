/**

./inc/kul/code/cpp.hpp

Created on: 15 Feb 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul (The Kommon Usage Library).

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPosE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include "kul/code/compiler.hpp"
#include "kul/log.hpp"

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
			KLOG2(COUT, INFO) << cmd;
			
			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to build executable");

			return exe; 
		}
		const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, "lib" + outFile) + ".so");			
			std::string cmd = linker + " -shared -o " + lib;
			for(const std::string& o : objects)
				cmd += " " + o;
			KLOG2(COUT, INFO) << cmd;

			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to build shared lib");

			return lib; 
		}
		const std::string buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, "lib" + outFile) + ".a"); 
			std::string cmd = archiver + " " + lib + " ";
			for(const std::string& o : objects)
				cmd += " " + o;
			KLOG2(COUT, INFO) << cmd;

			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to build static lib");

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

			if(!os::isDir(os::dirDotDot(out))) os::mkDir(os::dirDotDot(out));
			cmd += " -o " + obj + " -c " + in;
			KLOG2(COUT, INFO) << cmd;

			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to compile source");
			
			return obj;
		}
		virtual void preCompileHeader(			
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			using namespace kul;
			
			if(in.rfind(".") == std::string::npos)
				KEXCEPT(Exception, "Unknown header type");
			
			std::string cmd;// = compiler + " -x";
			std::string h = in.substr(in.rfind(".") + 1);

			if(h.compare("h") == 0)
				cmd = cc() + " -x c-header ";
			else
			if(h.compare("hpp") == 0)
				cmd = cxx() + " -x c++-header ";
			else
				KEXCEPT(Exception, "Failed to pre-compile header - uknown file type: " + h);
			cmd += in + " ";
			for(const std::string& s : args)
				cmd += s + " ";
			for(const std::string& s : incs)
				cmd += "-I" + s + " ";

			if(!os::isDir(os::dirDotDot(out))) os::mkDir(os::dirDotDot(out));
			cmd += " -o " + out;
			KLOG2(COUT, INFO) << cmd;

			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to pre-compile header");
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
			KLOG2(COUT, INFO) << cmd;

			KLOG2(COUT, INFO) << kul::os::getEnvVar("PATH");
			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to build executable");

			return exe; 
		}
		const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, outFile) + ".dll"); 
			return lib;
		}
		const std::string buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, outFile) + ".lib"); 
			std::string cmd = archiver + " /OUT:\"" + lib + "\" /NOLOGO /LTCG ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			KLOG2(COUT, INFO) << cmd;

			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to build static lib");

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

			if(!os::isDir(os::dirDotDot(os::localPath(out)))) os::mkDir(os::dirDotDot(os::localPath(out)));
			cmd += " -c \"/Fo" + obj + "\" \"" + in + "\"";			
			KLOG2(COUT, INFO) << cmd;
			
			if(kul::os::execReturn(cmd) != 0)
				KEXCEPT(Exception, "Failed to compile source");
			
			return obj;
		}
		virtual void preCompileHeader(			
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			KEXCEPT(Exception, "Method is not implemented");
		}
};




};};};
#endif /* _KUL_CODE_CPP_HPP_ */