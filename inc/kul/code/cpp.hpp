/*
 * code.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
 */

#ifndef _KUL_CODE_CPP_HPP_
#define _KUL_CODE_CPP_HPP_

#include <stdexcept>

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
	private:
		static kul::ext::goo_gle::StringToTGMap<Compiler> compilers;
		static void init(){}
	protected:
		Compiler(const int& v) : version(v){}
		const int version;
		virtual const std::string executable(const std::string& s);
		virtual const std::string sharedLibrary(const std::string& s);		
		virtual const std::string staticLibrary(const std::string& s);
	public:
		static const Compiler& get(const std::string& s, const int& version = 0) throw(CompilerNotFoundException){
			if(Compiler::compilers.size() == 0) init();
			if(Compiler::compilers.count(s) > 0)
				return (*Compiler::compilers.find(s)).second;
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + s + " is not implemented");
		}
};

// Supports all known gcc versions - i.e. known to me
class GCCompiler : public Compiler{
	public:
		const std::string executable(const std::string& s)		{ return s; }
		const std::string sharedLibrary(const std::string& s)	{ return std::string("lib" + s + ".so"); }
		const std::string staticLibrary(const std::string& s)	{ return std::string("lib" + s + ".a"); }
};

class WINCompiler : public Compiler{
	public:
		const std::string executable(const std::string& s)		{ return std::string(s + ".exe"); }
		const std::string sharedLibrary(const std::string& s)	{ return std::string("lib" + s + ".dll"); }
		const std::string staticLibrary(const std::string& s)	{ return std::string("lib" + s + ".lib"); }
};


};};};
#endif /* _KUL_CODE_CPP_HPP_ */
