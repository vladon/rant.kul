/*
 * code.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include "glog/logging.h"

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

class Compiler{
	public:
		virtual ~Compiler(){}
	protected:
		Compiler(const int& v) : version(v){}
		const int version;			
		virtual const std::string executable(const std::string& s)		const = 0;
		virtual const std::string sharedLibrary(const std::string& s)	const = 0;
		virtual const std::string staticLibrary(const std::string& s)	const = 0;
};

class Compilers{
	private:
		Compilers(){
			// add compilers to map
		}
		static Compilers* instance;
		kul::ext::goo_gle::StringToTGMap<Compiler*> compilers;
	public:
		static Compilers* INSTANCE(){ if(instance == 0) instance = new Compilers(); return instance;}
		const Compiler* get(const std::string s) throw(CompilerNotFoundException){
			if(Compilers::compilers.count(s) > 0)
				return (*Compilers::compilers.find(s)).second;
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + s + " is not implemented");
		}
};

// Supports all known gcc versions - i.e. known to me
class GCCompiler : public Compiler{
	public:
		const std::string executable(const std::string& s)		const { return s; }
		const std::string sharedLibrary(const std::string& s)	const { return std::string("lib" + s + ".so"); }
		const std::string staticLibrary(const std::string& s)	const { return std::string("lib" + s + ".a"); }
};

class WINCompiler : public Compiler{
	public:
		const std::string executable(const std::string& s)		const { return std::string(s + ".exe"); }
		const std::string sharedLibrary(const std::string& s)	const { return std::string(s + ".dll"); }
		const std::string staticLibrary(const std::string& s)	const { return std::string(s + ".lib"); }
};


};};};
#endif /* _KUL_CODE_CPP_HPP_ */
