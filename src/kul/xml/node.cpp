/*
 * xml.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#include "kul/xml.hpp"

#include "glog/logging.h"

void kul::xml::NodeFactory::log(const Node* node){
	if(node->getChildren()->empty()) return;
	LOG(INFO) << "printing children for node " << node->getName();
	for(Node* n : *node->getChildren())
		LOG(INFO) << n->getName() <<  " is a child of node " << node->getName();
	for(Node* n : *node->getChildren())
		log(n);
}

std::vector<kul::xml::Node*>* kul::xml::NodeFactory::validate(std::vector<Node*>* nodes, const pugi::xml_node& node, const NodeValidator& v){

	LOG(INFO) << "Document node name: 	" << node.name();
	LOG(INFO) << "Document node path: 	" << node.path('/');
	LOG(INFO) << "Document node value: 	" << node.value();

	for(pugi::xml_node n : node.children()){
		bool found = false;
		for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
			LOG(INFO) << "Document node name: " << n.name();
			LOG(INFO) << "Validator node name: " << pair.first;
			if(std::string(n.name()).compare(pair.first) == 0){
				LOG(INFO) << "Validator node equal document node";
				found = true; break;
			}
		}
		if(!found){
			LOG(ERROR) << "Element " << n.name() << " is unknown";
			throw Exception("XML Exception: Element " + std::string(n.name()) + " is unknown");
		}
	}

	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		int i = 0;
		for(pugi::xml_node n : node.children(pair.first.c_str())){
			LOG(INFO) << "Node found with name: " << n.name();
			i++;
		}
		LOG(INFO) << "Checking children of node: " << node.name();
		LOG(INFO) << "Valid minimum number of Element: " << pair.first << ": " << pair.second.getMin();
		LOG(INFO) << "Valid maximum number of Element: " << pair.first << ": " << pair.second.getMax();;
		if(pair.second.getMin() != 0 && i < pair.second.getMin()){
			LOG(ERROR) << "Invalid minimum number of Element: " << pair.first;
			LOG(ERROR) << "Minimum number expected: " << pair.second.getMin();
			throw Exception("XML Exception: Invalid minimum number of Element: " + pair.first);
		}
		if(pair.second.getMax() != 0 && i > pair.second.getMax()){
			LOG(ERROR) << "Invalid maximum number of Element: " << pair.first;
			LOG(ERROR) << "Maximum number expected: " << pair.second.getMax();
			throw Exception("XML Exception: Invalid maximum number of Element: " + pair.first);
		}
	}

	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		for(pugi::xml_node n : node.children(pair.first.c_str())){
			LOG(INFO) << "Document node name: " << n.name();
			LOG(INFO) << "Validator node name: " << pair.first;
			LOG(INFO) << "Validator node equal document node";
			if(n.text().empty()){
				LOG(INFO) << "Node " << n.name() << " has no text.";
				nodes->push_back(new Node(validate(new std::vector<Node*>, n, pair.second), pair.first));
			}
			else{
				LOG(INFO) << "Node " << n.name() << " has text.";
				nodes->push_back(new TextNode(pair.first, n.child_value()));
			}
		}
	}

	return nodes;
}

kul::xml::Node* kul::xml::NodeFactory::create(const std::string& location, const std::string& root, const NodeValidator& v){

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(location.c_str());

	if(!result){
		LOG(ERROR) << result.description();
		throw Exception("PUGIXML Exception creation document - file not found potentially\n");
	}

	if(doc.child(root.c_str()).empty()){
		throw Exception("root element \"" + root + "\" not found, malformed document");
	}
	LOG(INFO) << "Validing xml file";

	return new Node(validate(new std::vector<kul::xml::Node*>(), doc.child(root.c_str()), v), root);
}

const kul::xml::Node& kul::xml::Node::operator[](std::string s) const{

	for(const Node* n : *this->getChildren())
		if(s.compare(n->getName()) == 0)
			return *n;
	throw Exception("XML Exception: Element " + s + " doesn't exist under node " + this->getName());
}
