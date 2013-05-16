/*
 * code.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include "glog/logging.h"

#include "kul/os.hpp"
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
		kul::proc::Process* setupCompilationProcess(const std::string cmd) const{
			kul::proc::Process* p;
			std::string n = cmd.substr(cmd.find(" "));
			for(const std::string& s : kul::st_d::String::split(cmd, ' ')){
				LOG(INFO) << s;
				p = kul::proc::Process::create(s.c_str());
				break;
			}
			for(const std::string& s : kul::st_d::String::split(n, ' ')){
				LOG(INFO) << s;
				p->addArg(s.c_str());
			}
			return p;
		}
	public:
		virtual ~Compiler(){}
		virtual const std::string buildExecutable(
			const std::string& linker, 	
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::ext::goo_gle::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) const = 0;
		virtual const std::string buildSharedLibrary(
			const std::string& linker, 	
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const = 0;
		virtual const std::string buildStaticLibrary(
			const std::string& archiver, 	
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const = 0;
		virtual const std::string compileSource	(
			const std::string& compiler,	
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
			if(OS::execReturn(cmd) != 0) exit(-1);
			return exe; 
		}
		const std::string buildSharedLibrary(const std::string& linker, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".so");			
			std::string cmd = linker + " -shared -o " + " \"" + lib + "\" ";;
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			//if(stat && data().count(LIBRARIES) > 0)
			//	for(const std::string& lib : (*data().find(LIBRARIES)).second)
			//		cmd += "-l\"" + lib + "\" ";
			LOG(INFO) << cmd;
			if(OS::execReturn(cmd) != 0) exit(1);
			return lib; 
		}
		const std::string buildStaticLibrary(const std::string& archiver, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, "lib" + outFile) + ".a"); 
			std::string cmd = archiver + " \"" + lib + "\" ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			//if(stat && data().count(LIBRARIES) > 0)
			//	for(const std::string& lib : (*data().find(LIBRARIES)).second)
			//		cmd += "-l\"" + lib + "\" ";
			LOG(INFO) << cmd;
			if(OS::execReturn(cmd) != 0) exit(1);
			return lib; 
		}
		const std::string compileSource(const std::string& compiler, const std::vector<std::string>& args, const kul::ext::goo_gle::StringHashSet& incs, const std::string& in, const std::string& out) const {
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
			if(OS::execReturn(cmd) != 0) exit(-1);
			return obj;
		}
};

class WINCompiler : public Compiler{
	public:
		WINCompiler(const int& v = 0) : Compiler(v){}
		const std::string buildExecutable(
			const std::string& linker, 	
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const kul::ext::goo_gle::StringHashSet& libPaths,
			const std::string& out, 
			const Mode& mode) const {
/*
link.exe 
/ERRORREPORT:PROMPT 
/OUT:"E:\sand\box\cpp\kul\master\kul\Release\maiken.exe" 
/NOLOGO 
/LIBPATH:E:\sand\box\cpp\kul\master\kul\Release pugixml.lib
 kul.lib kul.win.lib libglog.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib 
 /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" 
 /manifest:embed /DEBUG /PDB:"E:\sand\box\cpp\kul\master\kul\Release\maiken.pdb" 
 /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE /NXCOMPAT /IMPLIB:"E:\sand\box\cpp\kul\master\kul\Release\maiken.lib" /MACHINE:X86 /SAFESEH Release\debug.obj
         Release\ValgrindErrorParser.obj
         Release\ValgrindError.obj
         Release\ValgrindErrors.obj
         Release\Valgrinder.obj
         Release\Project.obj
         Release\Settings.obj
         Release\Application.obj
*/
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
			if(OS::execReturn(cmd) != 0) exit(-1);
			return exe; 
		}
		const std::string buildSharedLibrary(const std::string& linker, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".dll"); 
			return lib;
		}
		const std::string buildStaticLibrary(const std::string& archiver, const std::vector<std::string>& objects, const std::string& outDir, const std::string& outFile) const { 
			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".lib"); 
			std::string cmd = archiver + " /OUT:\"" + lib + "\" /NOLOGO /LTCG ";
			for(const std::string& o : objects)
				cmd += " \"" + o + "\" ";
			//if(stat && data().count(LIBRARIES) > 0)
			//	for(const std::string& lib : (*data().find(LIBRARIES)).second)
			//		cmd += "-l\"" + lib + "\" ";
			LOG(INFO) << cmd;
			if(OS::execReturn(cmd) != 0) exit(1);
			return lib;
		}
		const std::string compileSource(const std::string& compiler, const std::vector<std::string>& args, const kul::ext::goo_gle::StringHashSet& incs, const std::string& in, const std::string& out) const {
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

			
			//std::shared_ptr<kul::proc::Process> proc(this->setupCompilationProcess(cmd));
			//proc->start();

			if(OS::execReturn(cmd) != 0) exit(-1);
			return obj;
		}
};

class Compilers{
	private:
		Compilers(){
			// add compilers to map
			GCCompiler* 	gcc	= new GCCompiler();
			WINCompiler* 	win	= new WINCompiler();
			compilers.insert(std::pair<std::string, Compiler*>("gcc"	, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("g++"	, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("clang"	, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("cl"		, win));
			compilers.insert(std::pair<std::string, Compiler*>("cl.exe"	, win));
		}
		static Compilers* instance;
		kul::ext::goo_gle::StringToTGMap<Compiler*> compilers;
	public:
		static Compilers* INSTANCE(){ if(instance == 0) instance = new Compilers(); return instance;}
		const Compiler* get(const std::string& compiler) throw(CompilerNotFoundException){
			std::string needle = compiler;
			if(needle.find(" ") > 0)
				for(const std::string& s :kul::st_d::String::split(compiler, ' ')){
					if(Compilers::compilers.count(s) > 0)
						return (*Compilers::compilers.find(s)).second;
					if(kul::OS::localPath(s).find(kul::OS::pathSep()))
						LOG(INFO) << s.substr(s.rfind(kul::OS::pathSep()));
				}
			if(kul::OS::localPath(compiler).find(kul::OS::pathSep()))
				LOG(INFO) << compiler.substr(compiler.rfind(kul::OS::pathSep()));
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + compiler + " is not implemented");
		}
};


};};};
#endif /* _KUL_CODE_CPP_HPP_ */
