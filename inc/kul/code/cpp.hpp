/*
 * cpp.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include "glog/logging.h"

#include "kul/os.hpp"
#include "kul/cli.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"
#include "kul/except.hpp"

#include "kul/ext/google.hpp"

namespace kul{ namespace code{ namespace cpp{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class CompilerNotFoundException : public kul::Exception{
	public:
		CompilerNotFoundException(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

enum Mode { NONE = 0, STAT, SHAR};

class Compiler{	
	protected:
		Compiler(const int& v) : version(v){}
		const int version;
		kul::proc::Process* setupProcess(const std::string& c, const std::string& cmdline, const std::vector<kul::cli::EnvVar>& evs) const {

			std::vector<std::string> bits = kul::cli::CmdLine::asArgs(cmdline);

			std::string cmd = c;
			std::string path;
			if(c.find(" ") != std::string::npos)
				for(const std::string& s : kul::st_d::String::split(c, " ")){
					cmd = s;
					break;	
				}
			if(kul::OS::localPath(cmd).find(kul::OS::dirSep()) != std::string::npos){
				path = cmd.substr(0, cmd.rfind(kul::OS::dirSep()));
				cmd = cmd.substr(cmd.rfind(kul::OS::dirSep()) + 1);
			}

			kul::proc::Process* p;
			if(path.empty()) 	p = kul::proc::Process::create(cmd);
			else 				p = kul::proc::Process::create(path, cmd);

			bits.erase(bits.begin());			
			for(const std::string& s : bits)
				p->addArg(s.c_str());			
			for(const kul::cli::EnvVar& ev : evs)
				p->addEnvVar(ev.name(), ev.toString());

			return p;
		}
	public:
		virtual ~Compiler(){}
		virtual const std::string buildExecutable(
			const std::string& linker, 	
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::ext::goo_gle::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) const = 0;
		virtual const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const = 0;
		virtual const std::string buildStaticLibrary(
			const std::string& archiver, 	
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const = 0;
		virtual const std::string compileSource	(
			const std::string& compiler,	
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& args, 		
			const kul::ext::goo_gle::StringHashSet& incs, 
			const std::string& in, 
			const std::string& out) const = 0;
		
};

// Supports all known gcc versions - i.e. known to me
class GCCompiler : public Compiler{
	public:
		GCCompiler(const int& v = 0) : Compiler(v){}
		const std::string buildExecutable(
			const std::string& linker, 	
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::ext::goo_gle::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) const {

			std::string exe = out;
			std::string cmd = linker + " ";
			
			for(const std::string& path : libPaths)
				cmd += "-L" + path + " ";
			
			if(mode == Mode::SHAR)
				cmd += " -shared ";
			else if(mode == Mode::STAT)
				cmd += " -static ";		

			cmd += " -o \"" + exe + "\" ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			for(const std::string& lib : libs)
				cmd += "-l" + lib + " ";
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(linker, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return exe; 
		}
		const std::string buildSharedLibrary(const std::string& linker, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".so");			
			std::string cmd = linker + " -shared -o " + " \"" + lib + "\" ";;
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(linker, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return lib; 
		}
		const std::string buildStaticLibrary(const std::string& archiver, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".a"); 
			std::string cmd = archiver + " \"" + lib + "\" ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(archiver, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return lib; 
		}
		const std::string compileSource(const std::string& compiler, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& args, const kul::ext::goo_gle::StringHashSet& incs, const std::string& in, const std::string& out) const {
			using namespace kul;
			std::string cmd = compiler + " ";
			std::string obj = out + ".o";

			for(const std::string& s : incs)
				cmd += "-I" + s + " ";
			for(const std::string& s : args)
				cmd += s + " ";

			if(!OS::isDir(OS::dirDotDot(out))) OS::mkDir(OS::dirDotDot(out));
			cmd += " -o \"" + obj + "\" -c \"" + in + "\"";			
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(compiler, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return obj;
		}
};

class WINCompiler : public Compiler{
	public:
		WINCompiler(const int& v = 0) : Compiler(v){}
		const std::string buildExecutable(
			const std::string& linker, 	
			const std::vector<kul::cli::EnvVar>& evs,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::ext::goo_gle::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) const {

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
			for(const std::string& lib : libs)
				cmd += " " + lib + " ";
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(linker, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return exe; 
		}
		const std::string buildSharedLibrary(const std::string& linker, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".dll"); 
			return lib;
		}
		const std::string buildStaticLibrary(const std::string& archiver, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".lib"); 
			std::string cmd = archiver + " /OUT:\"" + lib + "\" /NOLOGO /LTCG ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			LOG(INFO) << cmd;
			
			std::shared_ptr<kul::proc::Process> p(setupProcess(archiver, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}

			return lib;
		}
		const std::string compileSource(const std::string& compiler, const std::vector<kul::cli::EnvVar>& evs, const std::vector<std::string>& args, const kul::ext::goo_gle::StringHashSet& incs, const std::string& in, const std::string& out) const {
			using namespace kul;
			std::string cmd = compiler + " ";
			std::string obj = out + ".obj";

			for(const std::string& s : incs)
				cmd += "\"/I" + s + "\" ";
			for(const std::string& s : args)
				cmd += s + " ";

			if(!OS::isDir(OS::dirDotDot(OS::localPath(out)))) OS::mkDir(OS::dirDotDot(OS::localPath(out)));
			cmd += " -c \"/Fo" + obj + "\" \"" + in + "\"";			
			LOG(INFO) << cmd;

			std::shared_ptr<kul::proc::Process> p(setupProcess(compiler, cmd, evs));
			try{
				p->start();
			}
			catch(const kul::proc::Exception& e){ 
				LOG(INFO) << e.debug()<< " : " << typeid(e).name();
				exit(1);
			}
			return obj;
		}
};

class Compilers{
	private:
		Compilers(){
			// add compilers to map
			GCCompiler* 	gcc	= new GCCompiler();
			WINCompiler* 	win	= new WINCompiler();
			compilers.insert(std::pair<std::string, Compiler*>("gcc"		, gcc));			
			compilers.insert(std::pair<std::string, Compiler*>("g++"		, gcc));			
			compilers.insert(std::pair<std::string, Compiler*>("nvcc"		, gcc));			
			compilers.insert(std::pair<std::string, Compiler*>("clang"		, gcc));			
			compilers.insert(std::pair<std::string, Compiler*>("cl"			, win));			
		}
		static Compilers* instance;
		kul::ext::goo_gle::StringToTGMap<Compiler*> compilers;
	public:
		static Compilers* INSTANCE(){ if(instance == 0) instance = new Compilers(); return instance;}
		const Compiler* get(const std::string& compiler) throw(CompilerNotFoundException){
			std::string needle = compiler;
			kul::st_d::String::replaceAll(needle, ".exe", "");

			if(Compilers::compilers.count(compiler) > 0)
				return (*Compilers::compilers.find(compiler)).second;

			if(needle.find(" ") != std::string::npos)
				for(const std::string& s :kul::st_d::String::split(compiler, ' ')){
					if(Compilers::compilers.count(s) > 0)
						return (*Compilers::compilers.find(s)).second;

					if(kul::OS::localPath(s).find(kul::OS::dirSep()) != std::string::npos)
						if(Compilers::compilers.count(s.substr(s.rfind(kul::OS::dirSep()) + 1)) > 0)
							return (*Compilers::compilers.find(s.substr(s.rfind(kul::OS::dirSep()) + 1))).second;
				}
			
			if(kul::OS::localPath(compiler).find(kul::OS::dirSep()) != std::string::npos)
				if(Compilers::compilers.count(compiler.substr(compiler.rfind(kul::OS::dirSep()) + 1)) > 0)
					return (*Compilers::compilers.find(compiler.substr(compiler.rfind(kul::OS::dirSep()) + 1))).second;
			
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + compiler + " is not implemented");
		}
};


};};};
#endif /* _KUL_CODE_CPP_HPP_ */
