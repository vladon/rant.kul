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

#include "kul/smart.hpp"
#include "kul/ext/google.hpp"

namespace kul{ namespace xml{

using namespace kul::ext::goo_gle;

class Node;
class NodeValidator;
class NodeAttributeValidator;

typedef  kul::ext::goo_gle::stringToTGMap<NodeValidator> stringToNodeValidatorGMap;

class Exception : public std::runtime_error{
	private:
		Exception& operator=(Exception& e){ return *this;}
	public:
		~Exception() throw(){}
		Exception(std::string s) : std::runtime_error(s){}
		Exception(const Exception& e) : std::runtime_error(e.what()){}

		const char* what() const throw() {return std::runtime_error::what();}
};

class NodeFactory{
	private:
		static std::vector<Node*>* validate(std::vector<Node*>* ns, const pugi::xml_node& n, const NodeValidator& v);
		static void validateAttributes(const std::vector<Node*>& n, const NodeAttributeValidator& v);
		static void validateAttributes(const Node& n, const NodeValidator& v);
	public:
		static Node* create(const std::string& l, const std::string& r, const NodeValidator& v);
		static void writeToFile(std::string f, const NodeValidator& v);
		static void log(const Node* n);

};

class Node{
	private:
		const smart::Vector<Node> children;
		const stringToStringGMap attributes;
		const std::string name;
	public:
		Node(const std::vector<Node*>* c, const stringToStringGMap atts, const std::string n) :
			children(smart::Vector<Node>(c)), attributes(atts),
			name(n){}
		Node(const std::vector<Node*>* c, const std::string n) :
			children(smart::Vector<Node>(c)),
			name(n){}
		const Node& 								operator[](const std::string& s) const throw (Exception);
		const Node&				 				operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception);
		const std::string		 				txt() const throw (Exception);
		const std::string		 				att(const std::string& s) const throw (Exception);
		const std::vector<Node*>&	getChildren()		const { return *this->children.get(); }
		const stringToStringGMap&	getAttributes()	const	 { return attributes; }
		const std::string& 					getName() 			const { return this->name; }
};

class TextNode : public Node{
	private:
		const std::string text;
	public:
		TextNode(const stringToStringGMap atts, const std::string n, const std::string t) : Node(new std::vector<Node*>(), atts, n), text(t){}
		TextNode(const std::string n, const std::string t) : Node(new std::vector<Node*>(), n), text(t){}
		const std::string getText() const { return text; }
};

template <class T>
class NodeUser{
	private:
		NodeUser();
	protected:
		std::shared_ptr<const Node> rootNode;
		const std::string file;
		const void 						reset(Node* node)	{ rootNode.reset(node);}
		const void						reset() 							{ return rootNode.reset(); }

	public:
		NodeUser(std::string f) : rootNode(0), file(f) {}
		virtual ~NodeUser(){}
		virtual T* 									get() = 0;
		const virtual NodeValidator 	getValidator() = 0;
		const Node* 								getRoot() 	const		{ return rootNode.get(); }
		const std::string& 					getFile() 		const		{ return file; }
};

class NodeValidator{
	private:
		const stringToNodeValidatorGMap children;
		const std::vector<NodeAttributeValidator> attributeValidators;
		const int min;
		const int max;
		const bool text;
	public:
		NodeValidator(
				const stringToNodeValidatorGMap c,
				const std::vector<NodeAttributeValidator> a,
				const int mi, const int ma, const bool isT) :
					children(c)	, attributeValidators(a)	,
					min(mi)		, max(ma)							, text(isT){}
		NodeValidator(
				const stringToNodeValidatorGMap c,
				const int mi, const int ma, const bool isT) :
					children(c)	, attributeValidators()	,
					min(mi)		, max(ma)							, text(isT){}
		NodeValidator(
				const std::vector<NodeAttributeValidator> a,
				const int mi, const int ma, const bool isT) :
					children()	, attributeValidators(a)	,
					min(mi)		, max(ma)							, text(isT){}
		NodeValidator(
						const int mi, const int ma, const bool isT) :
							children()	, attributeValidators()	,
							min(mi)		, max(ma)							, text(isT){}
		NodeValidator(const NodeValidator& v)  :
			children(v.getChildren())	, attributeValidators(v.attributeValidators),
			min(v.getMin())					, max(v.getMax())					, text(v.isText()){}

		const NodeValidator 	operator=(const NodeValidator& n) const { return *this; }
		const stringToNodeValidatorGMap& 	getChildren() 				const { return this->children; }
		const std::vector<NodeAttributeValidator> getAtVals()			const	 { return attributeValidators; }
		const int& 						getMin() 														const { return this->min; }
		const int& 						getMax() 														const { return this->max; }
		const bool& 					isText() 														const { return this->text; }
};

class NodeAttributeValidator{
	private:
		const stringToVectorTGMap<std::string> allowedValues;
		const bool unique;
		const bool mandatory;
	public:
		NodeAttributeValidator(const stringToVectorTGMap<std::string> a, const bool u, const bool m) : allowedValues(a),unique(u), mandatory(m){}
		NodeAttributeValidator(const NodeAttributeValidator& n) : allowedValues(n.getAllowedValues()),unique(n.isUnique()), mandatory(n.isMandatory()){}
		const bool 							isUnique() 						const { return unique; }
		const bool 							isMandatory() 				const { return mandatory; }
		const stringToVectorTGMap<std::string>& 	getAllowedValues() 	const { return allowedValues;}
};


};};
#endif /* _NODE_HPP_ */
