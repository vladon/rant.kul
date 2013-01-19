/*
 * xml.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#include "pbd/xml.hpp"

#include "glog/logging.h"

std::vector<pbd::xml::Node*>* pbd::xml::NodeFactory::validate(std::vector<Node*>* nodes, const pugi::xml_node& node, const NodeValidator& v){

	LOG(INFO) << "Document node name: " << node.name();
	LOG(INFO) << "Document node path: " << node.path('/');
	LOG(INFO) << "Document node value: " << node.value();

	for(pugi::xml_node n : node.children(v.getName()->c_str())){
		bool found = false;
		for(const NodeValidator& vi : v.getChildren()){
			if(node.name() == vi.getName()->c_str())
				found = true;
			if(!found){
				LOG(ERROR) << "Element " << n.name() << " is unknown";
				throw Exception("XML Exception: Element " + std::string(n.name()) + " is unknown");
			}
		}
	}

	for(const NodeValidator& vi : v.getChildren()){
		int i = 0;
		for(pugi::xml_node n : node.children(vi.getName()->c_str())){
			LOG(INFO) << "Node found with name: " << n.name();
			i++;
		}
		if(vi.getMin() != 0 && i < vi.getMin()){
			LOG(ERROR) << "Invalid minimum number of Element: " << node.name();
			LOG(ERROR) << "Minimum number expected: " << vi.getMin();
			throw Exception("XML Exception: Invalid minimum number of Element: " + std::string(node.name()));
		}
		if(vi.getMax() != 0 && i > vi.getMax()){
			LOG(ERROR) << "Invalid maximum number of Element: " << node.name();
			LOG(ERROR) << "Maximum number expected: " << vi.getMin();
			throw Exception("XML Exception: Invalid maximum number of Element: " + std::string(node.name()));
		}
	}

	for(const NodeValidator& vi : v.getChildren()){
		LOG(INFO) << "Validator node name: " << vi.getName();
		if(node.name() == vi.getName()->c_str()){
			if(node.text().empty())
				for(pugi::xml_node n : node.children())
					validate(nodes, n, vi);
		}
	}

	return nodes;
}


pbd::xml::Node* pbd::xml::NodeFactory::create(const std::string& location, const NodeValidator& v){

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(location.c_str());

	if(!result){
		LOG(ERROR) << result.description();
		throw Exception("PUGIXML Exception creation document - file not found potentially\n");
	}

	if(doc.child(v.getName()->c_str()).empty()){
		throw Exception("root element \"" + *v.getName() + "\" not found, malformed document");
	}

	return new Node(pbd::xml::NodeFactory::validate(new std::vector<pbd::xml::Node*>() ,doc.child(v.getName()->c_str()), v), v.getName());
}
