/*
 * xml.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#include "glog/logging.h"


#include "kul/xml.hpp"

void kul::xml::NodeFactory::log(const Node* node){
	if(node->getChildren().empty()) return;
	LOG(INFO) << "printing children for node " << node->getName();
	for(Node* n : node->getChildren())
		LOG(INFO) << n->getName() <<  " is a child of node " << node->getName();
	for(Node* n : node->getChildren())
		log(n);
}

std::vector<kul::xml::Node*>* kul::xml::NodeFactory::validate(std::vector<Node*>* nodes, const pugi::xml_node& node, const NodeValidator& v){
	for(const pugi::xml_node& n : node.children()){
		bool found = false;
		for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
			if(std::string(n.name()).compare(pair.first) == 0){
				if(n.attributes().begin() != n.attributes().end()){
					for(pugi::xml_attribute a : n.attributes()){
						bool attFound = false;
						for(NodeAttributeValidator nav : pair.second.getAtVals()){
							for(std::pair<std::string, std::vector<std::string> > attVals : nav.getAllowedValues()){
								if(attVals.first.compare(std::string(a.name())) == 0){ attFound = true; break;}
							}
							if(attFound) break;
						}
						if(!attFound) {
							LOG(ERROR) << "Attribute \"" << a.name() << "\" for Element : \"" << n.name() << "\" is unknown";
							throw Exception("XML Exception: Attribute: \"" + std::string(a.name()) + "\" for Element : \"" + std::string(n.name()) + "\" is unknown");
						}
					}
				}
				found = true; break;
			}
		}
		if(!found){
			LOG(ERROR) << "Element \"" << n.name() << "\" is unknown";
			throw Exception("XML Exception: Element " + std::string(n.name()) + " is unknown");
		}
	}

	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		const int i = node.select_nodes(pugi::xpath_query(pair.first.c_str(), 0)).size();
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
		for(const pugi::xml_node& n : node.children(pair.first.c_str())){
			stringToStringGMap atts;
			if(n.attributes().begin() != n.attributes().end())
				for(pugi::xml_attribute a : n.attributes())
					atts.insert(std::pair<std::string, std::string>(std::string(a.name()), std::string(a.value())));
			if(pair.second.isText() && n.text().empty())
				nodes->push_back(new TextNode(atts, pair.first, ""));
			else if(n.text().empty())
				nodes->push_back(new Node(validate(new std::vector<Node*>, n, pair.second), atts, pair.first));
			else if(!pair.second.isText())
				throw Exception("XML Exception: text not expected in node " + std::string(n.name()));
			else
				nodes->push_back(new TextNode(atts, pair.first, n.child_value()));
		}
	}

	return nodes;
}

void kul::xml::NodeFactory::validateAttributes(const std::vector<Node*>& nodes, const NodeAttributeValidator& v){
	for(std::pair<std::string, std::vector<std::string> > valPair  : v.getAllowedValues()){
		if(!valPair.second.empty()){
			for(const Node* n : nodes){
				bool f = false;
				for(const std::pair<std::string, std::string>& attPair  : n->getAttributes())
					if(valPair.first.compare(attPair.first) == 0){
						for(std::string s : valPair.second)
							if(s.compare(attPair.second)){ f= true; break; }
						if(f) break;
					}
				if(!f) throw Exception("Attribute " + valPair.first + " on node " + n->getName() + " is not one of the expected values!");
			}
		}
		for(const Node* n : nodes)
			if(v.isMandatory()){
				try{
					n->att(valPair.first);
				}catch(kul::xml::Exception& e){ throw Exception("Attribute " + valPair.first + " on node " + n->getName() + " is MANDATORY!");}
			}

		if(v.isUnique()){
			std::string name = "";
			std::vector<std::string> atts;
			for(const Node* n : nodes){
				name = n->getName();
				for(const std::pair<std::string, std::string>& attPair  : n->getAttributes())
					if(valPair.first.compare(attPair.first) == 0)
						atts.push_back(attPair.second);
			}
			for(std::string s1 : atts){
				int i = 0;
				for(std::string s2 : atts)
					if(s1.compare(s2) == 0) i++;
				if(i > 1) throw Exception("Attribute " + valPair.first + " on node " + name + " must be unique!");
			}
		}
	}
}

void kul::xml::NodeFactory::validateAttributes(const Node& node, const NodeValidator& v){
	for(const Node* n : node.getChildren()){
		for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
			if(pair.first.compare(n->getName()) == 0){
				for(NodeAttributeValidator nav : pair.second.getAtVals()){
					std::vector<Node*> ns;
					for(Node* in : node.getChildren())
						if(n->getName().compare(in->getName()) == 0)
							ns.push_back(in);
					validateAttributes(ns, nav);
				}
				validateAttributes(*n, pair.second);
			}
		}
	}
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
	Node* n = new Node(validate(new std::vector<kul::xml::Node*>(), doc.child(root.c_str()), v), root);

	std::vector<Node*> rootV; rootV.push_back(n);
	for(const NodeAttributeValidator& nav : v.getAtVals()){
		validateAttributes(rootV, nav);
	}
	validateAttributes(*n, v);
	return n;
}

const kul::xml::Node& kul::xml::Node::operator[](const std::string& s) const  throw (Exception){
	for(const Node* n : this->getChildren())
		if(s.compare(n->getName()) == 0)
			return *n;
	throw Exception("XML Exception: Element " + s + " doesn't exist under node " + this->getName());
}

const kul::xml::Node& kul::xml::Node::operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception){
	for(const Node* n : this->getChildren())
			if(c.compare(n->getName()) == 0){
				try{
					if(n->att(a).compare(v) == 0) return *n;
				}catch(const Exception& e){ LOG(INFO) << e.what();}
			}
	throw Exception("XML Exception: No Element " + c + " contains attribute " + a + " with value " + v + " under node " + this->getName());
}

const std::string kul::xml::Node::txt() const throw (Exception){
	const TextNode* txt = static_cast<const TextNode*>(this);
	if(txt) return txt->getText();
	throw Exception("XML Exception: " +this->getName() + " is not a text node");
}

const std::string kul::xml::Node::att(const std::string& s) const throw (Exception){
	for(const std::pair<const std::string, std::string> p : this->getAttributes())
		if(s.compare(p.first) == 0)
			return p.second;
	throw Exception("XML Exception: Attribute " + s + " doesn't exist under node " + this->getName());
}
