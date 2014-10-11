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
		virtual const std::string sharedLib(const std::string& lib) const = 0;
		virtual const std::string staticLib(const std::string& lib) const = 0;
		bool sourceIsBin() const{ return false; }
};

// Supports all known gcc versions - i.e. known to me
class GCCompiler : public CCompiler{
	public:
		GCCompiler(const int& v = 0) : CCompiler(v){}
		const std::string sharedLib(const std::string& lib) const {
			return "lib" + lib + ".so";
		}
		const std::string staticLib(const std::string& lib) const {
			return "lib" + lib + ".a";
		}
		const CompilerProcessCapture buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const std::vector<std::string>& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception) {

			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			CompilerProcessCapture pc(*p, out);
			for(const std::string& path : libPaths)	(*p).addArg("-L").addArg(path);
			if(mode == Mode::SHAR)		(*p).addArg("-shared");
			else if(mode == Mode::STAT) (*p).addArg("-static");
			(*p).addArg("-o").addArg(out);
			for(const std::string& o : objects)	(*p).addArg(o);
			for(const std::string& lib : libs)	(*p).addArg("-l").addArg(lib);
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) (*p).addArg(s);
			
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc; 
		}
		const CompilerProcessCapture buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, "lib" + outFile) + ".so");

			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			CompilerProcessCapture pc(*p, lib);
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			(*p).addArg("-shared").addArg("-o").addArg(lib);
			for(const std::string& o : objects)	(*p).addArg(o);	
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc; 
		}
		const CompilerProcessCapture buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, "lib" + outFile) + ".a");
			std::string cmd = archiver;
			std::vector<std::string> bits;
			if(archiver.find(" ") != std::string::npos){
				bits = kul::String::split(archiver, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			CompilerProcessCapture pc(*p, lib);
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			(*p).addArg(lib).addArg("-c");
			for(const std::string& o : objects)	(*p).addArg(o);	
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc; 
		}
		const CompilerProcessCapture compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const std::vector<std::string>& incs,
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			using namespace kul;
			std::string obj = out + ".o";
			
			std::string cmd = compiler;
			std::vector<std::string> bits;
			if(compiler.find(" ") != std::string::npos){
				bits = kul::String::split(compiler, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			CompilerProcessCapture pc(*p, obj);
			
			for(const std::string& s : incs)	(*p).addArg("-I").addArg(s);
			for(const std::string& s : args)	(*p).addArg(s);
			(*p).addArg("-o").addArg(obj).addArg("-c").addArg(in);
			try{
				(*p).start();			
			}catch(const kul::proc::Exception& e){
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc;
		}
		virtual void preCompileHeader(			
			const std::vector<std::string>& incs,
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
			KLOG(INF) << cmd;

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

class IntelCompiler : public GCCompiler{
	public:
		IntelCompiler(const int& v = 0) : GCCompiler(v){}
		virtual const std::string cc() const {
			return "icc";
		}
		virtual const std::string cxx() const {
			return "icpc";
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
		const std::string sharedLib(const std::string& lib) const {
			return lib + ".dll";
		}
		const std::string staticLib(const std::string& lib) const {
			return lib + ".lib";
		}
		const CompilerProcessCapture buildExecutable(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects,
			const std::vector<std::string>& libs,
			const std::vector<std::string>& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception){ 

			std::string exe = out + ".exe";

			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			CompilerProcessCapture pc(*p, exe);
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			(*p).addArg("/NOLOGO").addArg("/LTCG");			
			for(const std::string& path : libPaths)	(*p).addArg("/LIBPATH:").addArg(path);
			//if(mode == Mode::SHAR)		(*p).addArg("-shared");
			//else if(mode == Mode::STAT)	(*p).addArg("-static");
			(*p).addArg("/OUT:").addArg(exe);
			for(const std::string& o : objects)	(*p).addArg(o);
			for(const std::string& lib : libs)
				if(mode == Mode::SHAR) (*p).addArg(sharedLib(lib));
				else
				if(mode == Mode::STAT) (*p).addArg(staticLib(lib));
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) (*p).addArg(s);

			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc; 
		}
		const CompilerProcessCapture buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, outFile) + ".dll"); 

			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			CompilerProcessCapture pc(*p, lib);
			(*p).addArg("/OUT:").addArg(lib).addArg("/NOLOGO").addArg("/LTCG");
			for(const std::string& o : objects)	(*p).addArg(o);	
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc;
		}
		const CompilerProcessCapture buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::os::dirJoin(outDir, outFile) + ".lib"); 
			std::string cmd = archiver;
			std::vector<std::string> bits;
			if(archiver.find(" ") != std::string::npos){
				bits = kul::String::split(archiver, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			CompilerProcessCapture pc(*p, lib);
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			(*p).addArg("/OUT:").addArg(lib).addArg("/NOLOGO").addArg("/LTCG");
			for(const std::string& o : objects)	(*p).addArg(o);	
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}
			return pc;
		}
		const CompilerProcessCapture compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const std::vector<std::string>& incs,
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			using namespace kul;
			
			std::string obj = out + ".o";
			if(!os::isDir(os::dirDotDot(os::localPath(out)))) os::mkDir(os::dirDotDot(os::localPath(out)));

			std::string cmd = compiler;
			std::vector<std::string> bits;
			if(compiler.find(" ") != std::string::npos){
				bits = kul::String::split(compiler, ' ');
				cmd = bits[0];
			}
			std::shared_ptr<kul::Process> p(kul::Process::create(cmd));
			for(uint i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			CompilerProcessCapture pc(*p, obj);
			(*p).addArg("-c").addArg("/Fo").addArg(obj).addArg(in);
			for(const std::string& s : incs)	(*p).addArg("/I").addArg(s);
			for(const std::string& s : args)	(*p).addArg(s);
			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc;
		}
		virtual void preCompileHeader(			
			const std::vector<std::string>& incs,
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			KEXCEPT(Exception, "Method is not implemented");
		}
};

};};};
#endif /* _KUL_CODE_CPP_HPP_ */
