/*
 * node.hpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "pugixml.hpp"
#include "glog/logging.h"

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "kul/hash.hpp"
#include "kul/file.hpp"
#include "kul/smart.hpp"
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
				throw Exception(__FILE__, __LINE__, "PUGIXML Exception: " + std::string(result.description()) + " : " + std::string(f + result.offset));
		}
};

class NodeFactory{
	private:
		static std::vector<const Node*>* validate(Node** p, std::vector<const Node*>* ns, const pugi::xml_node& n, const NodeValidator& v);
		static void validateAttributes(const std::vector<const Node*>& n, const NodeAttributeValidator& v);
		static void validateAttributes(const Node& n, const NodeValidator& v);
		static void writeToFile(const char*n, const NodeValidator& v, kul::file::Writer& w, int t);
	public:
		static const Node* create(const char*f, const char*r, const NodeValidator& v);
		static void writeToFile(const char*n, const char*f,  const NodeValidator& v);		
		static void log(const Node* n);
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
		Node**const prev;
		const smart::Vector<const Node> kinder;
		const hash::map::S2S atts;
		const std::string n;
	public:
		Node(Node**const p, const std::vector<const Node*>* c, const hash::map::S2S atts, const std::string n) :
			prev(p),
			kinder(smart::Vector<const Node>(c)), atts(atts),
			n(n){}
		Node(Node**const p, const std::vector<const Node*>* c, std::string n) :
			prev(p),
			kinder(smart::Vector<const Node>(c)),
			n(n){}
		const Node& 					operator[](const std::string& s) const throw (Exception);
		const Node&				 		operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception);
		const Node*						parent() 		const { return *prev; }
		const std::string		 		txt() 			const throw (Exception);
		const std::string		 		att(const std::string& s) const throw (Exception);
		const std::vector<const Node*>&	children()		const { return *this->kinder.get(); }
		const hash::map::S2S&			attributes()	const { return atts; }
		const std::string&				name() 			const { return this->n; }
};

class TextNode : public Node{
	private:
		const std::string t;
	public:
		TextNode(Node**const p, const hash::map::S2S atts, const std::string n, const std::string t) :
			Node(p, new std::vector<const Node*>(), atts, n), t(t){}
		TextNode(Node**const p, const std::string n, const std::string t) :
			Node(p, new std::vector<const Node*>(), n), t(t){}
		const std::string txt() const { return t; }
};

template <class T>
class NodeUser{
	private:
		NodeUser();
	protected:
		std::shared_ptr<const Node> rootNode;
		const std::string f;
		const void 						reset(const Node* node)	{ rootNode.reset(node);}
		const void						reset() 				{ rootNode.reset(); }

	public:
		NodeUser(std::string f) : rootNode(0), f(f) {}
		virtual ~NodeUser(){}
		virtual T* 						get() = 0;
		const virtual NodeValidator 	validator() = 0;
		const Node* 					root() 			const 	{ return rootNode.get(); }
		const std::string& 				file() 			const	{ return f; }
};

class NodeValidator{
	private:
		const std::vector<std::pair<std::string, NodeValidator> > children;
		const std::vector<NodeAttributeValidator> attributeValidators;
		const int min;
		const int max;
		const bool text;
	public:
		NodeValidator(
				const std::vector<std::pair<std::string, NodeValidator> >& c,
				const std::vector<NodeAttributeValidator>& a,
				const int& mi, const int& ma, const bool& isT) :
					children(c)	, attributeValidators(a),
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
				const std::vector<std::pair<std::string, NodeValidator> >& c,
				const int& mi, const int& ma, const bool& isT) :
					children(c)	, attributeValidators()	,
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
				const std::vector<NodeAttributeValidator>& a,
				const int& mi, const int& ma, const bool& isT) :
					children()	, attributeValidators(a),
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
						const int& mi, const int& ma, const bool& isT) :
							children()	, attributeValidators()	,
							min(mi)		, max(ma)				, text(isT){}
		NodeValidator(const NodeValidator& v)  :
			children(v.getChildren())	, attributeValidators(v.attributeValidators),
			min(v.minimum())			, max(v.maximum())					, text(v.isText()){}

		const NodeValidator 	operator=(const NodeValidator& n)					const { return *this; }
		const std::vector<std::pair<std::string, NodeValidator> >&	getChildren() 	const { return this->children; }
		const std::vector<NodeAttributeValidator>& 					getAtVals()		const { return attributeValidators; }
		const int& 													minimum() 		const { return this->min; }
		const int& 													maximum() 		const { return this->max; }
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
		const bool 								isChecked() 		const { return checked; }
		const bool 								isUnique() 			const { return unique; }
		const bool 								isMandatory() 		const { return mandatory; }
		const hash::map::S2VT<std::string>& 	allowedValues() 	const { return avs;}
};

class NodeTextFunction{
	private:
		const char* n;
	public:
		NodeTextFunction(const char* n) : n(n){}
		virtual ~NodeTextFunction(){}
		virtual const char* txt() = 0;
		const char*const name(){ return n; }


};


};};
#endif /* _NODE_HPP_ */
