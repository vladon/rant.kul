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
			kul::Process p(cmd);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			CompilerProcessCapture pc(p);
			for(const std::string& path : libPaths)	p.arg("-L" + path);
			if(mode == Mode::SHAR)		p.arg("-shared");
			else if(mode == Mode::STAT) p.arg("-static");
			p.arg("-o").arg(out);
			for(const std::string& o : objects)	p.arg(o);
			for(const std::string& lib : libs)	p.arg("-l" + lib);
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) p.arg(s);
			
			try{
				p.start();
			}catch(const kul::proc::Exception& e){
				pc.exception(std::current_exception());
			}
			pc.tmp(out);
			pc.cmd(p.toString());
			return pc; 
		}
		const CompilerProcessCapture buildLibrary(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 
			const kul::File& out, 
			const Mode& mode) const throw (kul::Exception) {

			std::string lib = out.dir().join(sharedLib(out.name()));
			if(mode == Mode::STAT) lib = out.dir().join(staticLib(out.name()));
			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			kul::Process p(cmd);
			CompilerProcessCapture pc(p);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			if(mode == Mode::SHAR)		p.arg("-shared");
			p.arg("-o").arg(lib);
			for(const std::string& o : objects)	p.arg(o);
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) p.arg(s);
			try{
				p.start();
			}catch(const kul::proc::Exception& e){
				pc.exception(std::current_exception());
			}
			pc.tmp(lib);
			pc.cmd(p.toString());
			return pc; 
		}
		const CompilerProcessCapture compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const std::vector<std::string>& incs,
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			std::string cmd = compiler;
			std::vector<std::string> bits;
			if(compiler.find(" ") != std::string::npos){
				bits = kul::String::split(compiler, ' ');
				cmd = bits[0];
			}
			kul::Process p(cmd);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			CompilerProcessCapture pc(p);
			
			for(const std::string& s : incs)	p.arg("-I"+s);
			for(const std::string& s : args)	p.arg(s);
			p.arg("-o").arg(out).arg("-c").arg(in);
			try{
				p.start();
			}catch(const kul::proc::Exception& e){
				pc.exception(std::current_exception());
			}

			pc.cmd(p.toString());
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

			cmd += " -o " + out;
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
			kul::Process p(cmd);
			CompilerProcessCapture pc(p);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			for(const std::string& path : libPaths)	p.arg("/LIBPATH:\"" + path + "\"");
			p.arg("/OUT:\"" + exe + "\"").arg("/nologo");	
			for(const std::string& o : objects)	p.arg(o);
			for(const std::string& lib : libs)
				if(mode == Mode::SHAR) p.arg(sharedLib(lib));
				else
				if(mode == Mode::STAT) p.arg(staticLib(lib));
				else
					for(const std::string libp : libPaths){
						bool f = 0;
						for(const kul::File fi : kul::Dir(libp).files()){
							if(fi.name().compare(sharedLib(lib)) == 0){
								p.arg(sharedLib(lib));
								f = 1;
							}else
							if(fi.name().compare(staticLib(lib)) == 0){
								p.arg(staticLib(lib));
								f = 1;
							}
							if(f) break;
						}
						if(f) break;
					}
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) p.arg(s);

			try{
				p.start();
			}catch(const kul::proc::Exception& e){
				pc.exception(std::current_exception());
			}
			pc.tmp(exe);
			pc.cmd(p.toString());
			return pc; 
		}
		const CompilerProcessCapture buildLibrary(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 
			const kul::File& out, 
			const Mode& mode) const throw (kul::Exception) {

			std::string lib = out.dir().join(sharedLib(out.name()));
			if(mode == Mode::STAT) lib = out.dir().join(staticLib(out.name()));
			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			kul::Process p(cmd);
			CompilerProcessCapture pc(p);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			p.arg("/OUT:\"" + lib + "\"").arg("/nologo").arg("/LTCG");
			for(const std::string& o : objects)	p.arg(o);
			for(const std::string& s: kul::String::split(linkerEnd, ' ')) p.arg(s);
			try{
				p.start();
			}catch(const kul::proc::Exception& e){
				pc.exception(std::current_exception());
			}
			pc.tmp(lib);
			pc.cmd(p.toString());
			return pc; 
		}
		const CompilerProcessCapture compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const std::vector<std::string>& incs,
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){ 

			std::string cmd = compiler;
			std::vector<std::string> bits;
			if(compiler.find(" ") != std::string::npos){
				bits = kul::String::split(compiler, ' ');
				cmd = bits[0];
			}
			kul::Process p(cmd);
			for(unsigned int i = 1; i < bits.size(); i++) p.arg(bits[i]);
			p.arg("/nologo");
			CompilerProcessCapture pc(p);
			for(const std::string& s : incs)	p.arg("/I\"" + s + "\"");
			for(const std::string& s : args)	p.arg(s);
			p.arg("/c").arg("/Fo\"" + out + "\"").arg("\"" + in + "\"");
			try{
				p.start();
			}catch(const kul::Exception& e){
				pc.exception(std::current_exception());
			}
			pc.cmd(p.toString());
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

}}}
#endif /* _KUL_CODE_CPP_HPP_ */
