/*
 * node.hpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "pugixml.hpp"

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "kul/file.hpp"
#include "kul/smart.hpp"
#include "kul/except.hpp"
#include "kul/ext/google.hpp"

namespace kul{ namespace xml{

using namespace kul::ext::goo_gle;

class Node;
class NodeValidator;
class NodeAttributeValidator;

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class NodeFactory{
	private:
		static std::vector<const Node*>* validate(Node** p, std::vector<const Node*>* ns, const pugi::xml_node& n, const NodeValidator& v);
		static void validateAttributes(const std::vector<const Node*>& n, const NodeAttributeValidator& v);
		static void validateAttributes(const Node& n, const NodeValidator& v);
		static void writeToFile(const char*n, const NodeValidator& v, kul::file::Writer& w, int t);
	public:
		static const Node* create(const char*l, const char*r, const NodeValidator& v);
		static void writeToFile(const char*n, const char*f,  const NodeValidator& v);
		static void log(const Node* n);

};

class Node{
	private:
		Node**const prev;
		const smart::Vector<const Node> kinder;
		const StringToStringHashMap atts;
		const char* n;
	public:
		Node(Node**const p, const std::vector<const Node*>* c, const StringToStringHashMap atts, const char*n) :
			prev(p),
			kinder(smart::Vector<const Node>(c)), atts(atts),
			n(n){}
		Node(Node**const p, const std::vector<const Node*>* c, const char*n) :
			prev(p),
			kinder(smart::Vector<const Node>(c)),
			n(n){}
		const Node& 					operator[](const std::string& s) const throw (Exception);
		const Node&				 		operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception);
		const Node*						parent() const{ return *prev; }
		const std::string		 		txt() 			const throw (Exception);
		const std::string		 		att(const std::string& s) const throw (Exception);
		const std::vector<const Node*>&	children()		const { return *this->kinder.get(); }
		const StringToStringHashMap&	attributes()	const	 { return atts; }
		const char* 				name() 			const { return this->n; }
};

class TextNode : public Node{
	private:
		const std::string text;
	public:
		TextNode(Node**const p, const StringToStringHashMap atts, const char*n, const std::string t) :
			Node(p, new std::vector<const Node*>(), atts, n), text(t){}
		TextNode(Node**const p, const char*n, const std::string t) :
			Node(p, new std::vector<const Node*>(), n), text(t){}
		const std::string txt() const { return text; }
};

template <class T>
class NodeUser{
	private:
		NodeUser();
	protected:
		std::shared_ptr<const Node> rootNode;
		const std::string f;
		const void 						reset(const Node* node)	{ rootNode.reset(node);}
		const void						reset() 				{ return rootNode.reset(); }

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
				const std::vector<std::pair<std::string, NodeValidator> > c,
				const std::vector<NodeAttributeValidator> a,
				const int mi, const int ma, const bool isT) :
					children(c)	, attributeValidators(a),
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
				const std::vector<std::pair<std::string, NodeValidator> > c,
				const int mi, const int ma, const bool isT) :
					children(c)	, attributeValidators()	,
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
				const std::vector<NodeAttributeValidator> a,
				const int mi, const int ma, const bool isT) :
					children()	, attributeValidators(a),
					min(mi)		, max(ma)				, text(isT){}
		NodeValidator(
						const int mi, const int ma, const bool isT) :
							children()	, attributeValidators()	,
							min(mi)		, max(ma)				, text(isT){}
		NodeValidator(const NodeValidator& v)  :
			children(v.getChildren())	, attributeValidators(v.attributeValidators),
			min(v.minimum())			, max(v.maximum())					, text(v.isText()){}

		const NodeValidator 	operator=(const NodeValidator& n)					const { return *this; }
		const std::vector<std::pair<std::string, NodeValidator> >&	getChildren() 	const { return this->children; }
		const std::vector<NodeAttributeValidator> 					getAtVals()		const { return attributeValidators; }
		const int& 													minimum() 		const { return this->min; }
		const int& 													maximum() 		const { return this->max; }
		const bool& 												isText() 		const { return this->text; }
};

class NodeAttributeValidator{
	private:
		const stringToVectorTGMap<std::string> allowedValues;
		const bool unique;
		const bool mandatory;
	public:
		NodeAttributeValidator(const stringToVectorTGMap<std::string> a, const bool u, const bool m) : allowedValues(a),unique(u), mandatory(m){}
		NodeAttributeValidator(const NodeAttributeValidator& n) : allowedValues(n.getAllowedValues()),unique(n.isUnique()), mandatory(n.isMandatory()){}
		const bool 								isUnique() 			const { return unique; }
		const bool 								isMandatory() 		const { return mandatory; }
		const stringToVectorTGMap<std::string>& getAllowedValues() 	const { return allowedValues;}
};

class NodeTextFunction{
	private:
		const char* f;
	public:
		NodeTextFunction(const char* f) : f(f){}
		virtual ~NodeTextFunction(){}
		virtual const char* txt() = 0;


};


};};
#endif /* _NODE_HPP_ */
