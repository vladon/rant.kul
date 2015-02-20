/**

./inc/kul/xml.hpp

Created on: 24 Feb 2013

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
#ifndef _KUL_XML_HPP_
#define _KUL_XML_HPP_

#include "pugixml.hpp"

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "kul/io.hpp"
#include "kul/hash.hpp"
#include "kul/array.hpp"
#include "kul/except.hpp"

namespace kul{ namespace xml{

class Node;
class NodeValidator;
class NodeAttributeValidator;

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class PugiXMLService{
	public:
		static void setupDocument(pugi::xml_document& doc, const char*f){
			pugi::xml_parse_result result = doc.load_file(f, pugi::parse_full);
			if(!result)	
				KEXCEPT(Exception, "PUGIXML Exception: " + std::string(result.description()) + " : " + std::string(f + result.offset));
		}
};

class NodeFactory{
	private:
		static std::vector<const Node> validate(std::vector<const Node> ns, const pugi::xml_node& n, const NodeValidator& v);
		static void validateAttributes(const std::vector<const Node>& n, const NodeAttributeValidator& v);
		static void validateAttributes(const Node& n, const NodeValidator& v);
		static void writeToFile(const char*n, const NodeValidator& v, kul::io::Writer& w, int t);
	public:
		static const std::shared_ptr<const Node> create(const char*f, const char*r, const NodeValidator& v);
		static void writeToFile(const char*n, const char*f,  const NodeValidator& v);		
		static void log(const Node& n);
};

class XPather{
	private:
		const char*f;
		pugi::xml_document doc;
	public:
		XPather(const char*f) : f(f){ PugiXMLService::setupDocument(doc, f); }
		~XPather(){ doc.save_file(f);}
		void writeNode(const char*x, const Node& node);
		void writeText(const char*x, const char*t);
		void writeAttr(const char*x, const char*n, const char*v);
};

class Node{
	private:
		const std::vector<const Node> c;
		const hash::map::S2S atts;
		const std::string n;
		const std::string t;
	public:
		Node(const std::vector<const Node> c, const hash::map::S2S atts, const std::string n) :
			c(c), atts(atts), n(n){}
		Node(const std::vector<const Node> c, std::string n) :
			c(c), n(n){}
		Node(const hash::map::S2S atts, const std::string& n, const std::string& t) :
			atts(atts), n(n), t(t){}
		const Node& 					operator[](const std::string& s) const throw (Exception);
		const Node&				 		operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception);
		const std::string		 		att(const std::string& s) const throw (Exception);
		const std::vector<const Node>&	children()		const { return c; }
		const hash::map::S2S&			attributes()	const { return atts; }
		const std::string&				name() 			const { return this->n; }
		const std::string&				txt() 			const { return t; }
};

template <class T>
class NodeUser{
	private:
		NodeUser();
		std::shared_ptr<const Node> rn;
	protected:
		const std::string f;
		void reset(std::shared_ptr<const Node> node){ rn = node;}
		void reset() 								{ rn.reset(); }

	public:
		NodeUser(std::string f) : rn(0), f(f) {}
		virtual ~NodeUser(){}
		virtual T* 						get() = 0;
		const virtual NodeValidator 	validator() = 0;
		const Node* 					root() 			const 	{ return rn.get(); }
		const std::string& 				file() 			const	{ return f; }
};

class NodeValidator{
	private:
		const std::vector<std::pair<std::string, NodeValidator> > children;
		const std::vector<NodeAttributeValidator> attributeValidators;
		const int mi;
		const int ma;
		const bool text;
	public:
		NodeValidator(
				const std::vector<std::pair<std::string, NodeValidator> >& c,
				const std::vector<NodeAttributeValidator>& a,
				const int& mi, const int& ma, const bool& isT) :
					children(c)	, attributeValidators(a),
					mi(mi)		, ma(ma)				, text(isT){}
		NodeValidator(
				const std::vector<std::pair<std::string, NodeValidator> >& c,
				const int& mi, const int& ma, const bool& isT) :
					children(c)	, attributeValidators()	,
					mi(mi)		, ma(ma)				, text(isT){}
		NodeValidator(
				const std::vector<NodeAttributeValidator>& a,
				const int& mi, const int& ma, const bool& isT) :
					children()	, attributeValidators(a),
					mi(mi)		, ma(ma)				, text(isT){}
		NodeValidator(
				const int& mi, const int& ma, const bool& isT) :
					children()	, attributeValidators()	,
					mi(mi)		, ma(ma)				, text(isT){}
		NodeValidator(const NodeValidator& v)  :
			children(v.getChildren())	, attributeValidators(v.attributeValidators),
			mi(v.minimum())			, ma(v.maximum())					, text(v.isText()){}

//		const NodeValidator 	operator=(const NodeValidator& n)					const { return *this; }
		const std::vector<std::pair<std::string, NodeValidator> >&	getChildren() 	const { return this->children; }
		const std::vector<NodeAttributeValidator>& 					getAtVals()		const { return attributeValidators; }
		const int& 													minimum() 		const { return this->mi; }
		const int& 													maximum() 		const { return this->ma; }
		const bool& 												isText() 		const { return this->text; }
};

class NodeAttributeValidator{
	private:
		const hash::map::S2VT<std::string> avs;
		const bool checked;
		const bool unique;
		const bool mandatory;
	public:
		NodeAttributeValidator(const hash::map::S2VT<std::string> a, const bool c, const bool u, const bool m) : avs(a), checked(c), unique(u), mandatory(m){}
		NodeAttributeValidator(const hash::map::S2VT<std::string> a, const bool u, const bool m) : avs(a), checked(0), unique(u), mandatory(m){}
		NodeAttributeValidator(const NodeAttributeValidator& n) : avs(n.allowedValues()), checked(n.isChecked()), unique(n.isUnique()), mandatory(n.isMandatory()){}
		bool 								isChecked() 	const { return checked; }
		bool 								isUnique() 		const { return unique; }
		bool	 							isMandatory() 	const { return mandatory; }
		const hash::map::S2VT<std::string>& allowedValues()	const { return avs;}
};


}}
#endif /* _KUL_XML_HPP_ */
