/*
 * node.hpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "pugixml.hpp"

#include "kul/smart/container.hpp"

#include "sparsehash/sparse_hash_map"

namespace kul{ namespace xml{

struct StdStringComparator{
	public:
		bool operator()(std::string s1, std::string s2) const{
			return (s1.compare(s2) == 0);
		}
};

class Node;
class NodeValidator;

typedef google::sparse_hash_map<std::string, NodeValidator, std::tr1::hash<std::string>, kul::xml::StdStringComparator> validatorMap;
typedef typename validatorMap::iterator it;

class NodeFactory{
	private:
		static std::vector<Node*>* validate(std::vector<Node*>* nodes, const pugi::xml_node& node, const NodeValidator& validator);
	public:
		static Node* create(const std::string& location, const std::string& root, const NodeValidator& validator);
		static void writeToFile(std::string file, const NodeValidator& validator);
		static void log(const Node* node);

};

class Node{
	private:
		const SmartVector<Node> children;
		//TODO attributes
		const std::string name;
	public:
		Node(const std::vector<Node*>* children, const std::string name) :
			children(SmartVector<Node>(children)),
			name(name){}
		const Node& operator[](std::string s) const;
		const std::vector<Node*>*	getChildren()		const { return this->children.get(); }
		const std::string& 					getName() 			const { return this->name; }
};

class TextNode : public Node{
	private:
		const std::string text;
	public:
		TextNode(const std::string name, const std::string text) : Node(new std::vector<Node*>(), name), text(text){}
		const std::string getText() const { return text; }
};

template <class T>
class NodeUser{
	private:
		NodeUser();
	protected:
		std::auto_ptr<const Node> rootNode;
		const std::string file;
		const void 						reset(Node* node)	{ rootNode.reset(node);}
		const Node*					release() 					{ return rootNode.release(); }

	public:
		NodeUser(std::string file) : rootNode(std::auto_ptr<const Node>(0)), file(file) {}
		virtual ~NodeUser(){}
		virtual T* 									get() = 0;
		const virtual NodeValidator 	getValidator() = 0;
		const Node* 								getRoot() 	const		{ return rootNode.get(); }
		const std::string& 					getFile() 		const		{ return file; }
};

class NodeValidator{
	private:
		const validatorMap children;
		const int min;
		const int max;
		const bool text;
	public:
		NodeValidator(
				const validatorMap children,
				const int min, const int max, const bool isText) :
					children(children)	, min(min),
					max(max)					, text(text){}
		NodeValidator(const NodeValidator& v)  :
			children(v.getChildren())	, min(v.getMin()),
			max(v.getMax())					, text(v.isText()){}

		const NodeValidator 	operator=(const NodeValidator& n) const { return *this; }
		const validatorMap& 	getChildren() 											const { return this->children; }
		const int& 						getMin() 														const { return this->min; }
		const int& 						getMax() 														const { return this->max; }
		const bool& 					isText() 														const { return this->text; }
};


class Exception : public std::runtime_error{
	private:
		Exception& operator=(Exception& e){ return *this;}
	public:
		~Exception() throw(){}
		Exception(std::string s) : std::runtime_error(s){}
		Exception(const Exception& e) : std::runtime_error(e.what()){}

		const char* what() const throw() {return std::runtime_error::what();}
};



};};
#endif /* _NODE_HPP_ */
