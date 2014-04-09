/**

./src/kul/xml/NodeFactory.cpp

Created on: 14 Jan 2013

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
#include "kul/xml.hpp"
#include "kul/log.hpp"

void kul::xml::NodeFactory::log(const Node* node){
	if(node->children().empty()) return;
	KLOG(INFO) << "printing children for node " << node->name();
	for(const Node* n : node->children())
		KLOG(INFO) << n->name() <<  " is a child of node " << node->name();	
	for(const Node* n : node->children())
		log(n);
}

std::vector<const kul::xml::Node*>* kul::xml::NodeFactory::validate(Node** p, std::vector<const Node*>* ns, const pugi::xml_node& node, const NodeValidator& v){
	for(const pugi::xml_node& n : node.children()){
		bool found = false;
		for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
			if(std::string(n.name()).compare(pair.first) == 0 || pair.first.compare("*") == 0){
				if(n.attributes().begin() != n.attributes().end()){
					for(pugi::xml_attribute a : n.attributes()){
						bool attFound = false;
						for(NodeAttributeValidator nav : pair.second.getAtVals()){
							for(std::pair<std::string, std::vector<std::string> > attVals : nav.allowedValues()){
								if(attVals.first.compare(std::string(a.name())) == 0){ attFound = true; break;}
							}
							if(attFound) break;
						}	
						if(!attFound) {
							KLOG(ERROR) << "Attribute \"" << a.name() << "\" for Element : \"" << n.name() << "\" is unknown";
							KEXCEPT(Exception, "XML Exception: Attribute: \"" + std::string(a.name()) + "\" for Element : \"" + std::string(n.name()) + "\" is unknown");
						}
					}
				}
				found = true; break;
			}
		}
		if(!found){
			KLOG(ERROR) << "Element \"" << n.name() << "\" is unknown";
			KEXCEPT(Exception, "XML Exception: Element " + std::string(n.name()) + " is unknown");
		}
	}

	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		const int i = node.select_nodes(pugi::xpath_query(pair.first.c_str(), 0)).size();
		if(pair.second.minimum() != 0 && i < pair.second.minimum()){
			KLOG(ERROR) << "Invalid minimum number of Element: " << pair.first;
			KLOG(ERROR) << "Minimum number expected: " << pair.second.minimum();
			KEXCEPT(Exception, "XML Exception: Invalid minimum number of Element: " + pair.first);
		}
		if(pair.second.maximum() != 0 && i > pair.second.maximum()){
			KLOG(ERROR) << "Invalid maximum number of Element: " << pair.first;
			KLOG(ERROR) << "Maximum number expected: " << pair.second.maximum();
			KEXCEPT(Exception, "XML Exception: Invalid maximum number of Element: " + pair.first);
		}
	}

	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		if(pair.first.compare("*") == 0){
			for(const pugi::xml_node& n : node.children()){
				hash::map::S2S atts;
				if(n.attributes().begin() != n.attributes().end())
					for(pugi::xml_attribute a : n.attributes())
						atts.insert(std::pair<std::string, std::string>(std::string(a.name()), std::string(a.value())));
				if(pair.second.isText() && n.text().empty())
					ns->push_back(new TextNode(p, atts, n.name(), ""));
				else if(n.text().empty()){
					   Node** parent = new Node*;
					ns->push_back((*parent = new Node(p, validate(parent, new std::vector<const Node*>, n, pair.second) , atts, pair.first.c_str())));
				}else if(!pair.second.isText())
					KEXCEPT(Exception, "XML Exception: text not expected in node " + std::string(n.name()));
				else
					ns->push_back(new TextNode(p, atts, n.name(), n.child_value()));
			}
		}else{
			for(const pugi::xml_node& n : node.children(pair.first.c_str())){
				hash::map::S2S atts;
				if(n.attributes().begin() != n.attributes().end())
					for(pugi::xml_attribute a : n.attributes())
						atts.insert(std::pair<std::string, std::string>(std::string(a.name()), std::string(a.value())));
				if(pair.second.isText() && n.text().empty())
					ns->push_back(new TextNode(p, atts, pair.first, ""));
				else if(n.text().empty()){
					   Node** parent = new Node*;
					ns->push_back((*parent = new Node(p, validate(parent, new std::vector<const Node*>, n, pair.second) , atts, pair.first.c_str())));
				}else if(!pair.second.isText())
					KEXCEPT(Exception, "XML Exception: text not expected in node " + std::string(n.name()));
				else
					ns->push_back(new TextNode(p, atts, pair.first, n.child_value()));
			}
		}
	}

	return ns;
}

void kul::xml::NodeFactory::validateAttributes(const std::vector<const Node*>& nodes, const NodeAttributeValidator& v){
	for(std::pair<std::string, std::vector<std::string> > valPair  : v.allowedValues()){
		if(!valPair.second.empty()){
			for(const Node* n : nodes){
				bool f = true;
				for(const std::pair<std::string, std::string>& attPair  : n->attributes())
					if(valPair.first.compare(attPair.first) == 0){
						f = false;
						for(std::string s : valPair.second)
							if(s.compare(attPair.second) == 0){ f = true; break; }
						if(f) break;
					}
				if(!f && v.isChecked()) KEXCEPT(Exception, "Attribute \"" + valPair.first + "\" on node \"" + n->name() + "\" is not one of the expected values!");
			}
		}
		for(const Node* n : nodes)
			if(v.isMandatory()){
				try{
					n->att(valPair.first);
				}catch(kul::xml::Exception& e){ KEXCEPT(Exception, "Attribute \"" + valPair.first + "\" on node \"" + n->name() + "\" is MANDATORY!");}
			}

		if(v.isUnique()){
			std::string name = "";
			std::vector<std::string> atts;
			for(const Node* n : nodes){
				name = n->name();
				for(const std::pair<std::string, std::string>& attPair  : n->attributes())
					if(valPair.first.compare(attPair.first) == 0)
						atts.push_back(attPair.second);
			}
			for(std::string s1 : atts){
				int i = 0;
				for(std::string s2 : atts)
					if(s1.compare(s2) == 0) i++;
				if(i > 1) KEXCEPT(Exception, "Attribute " + valPair.first + " on node " + name + " must be unique!");
			}
		}
	}
}

void kul::xml::NodeFactory::validateAttributes(const Node& node, const NodeValidator& v){
	for(const Node* n : node.children()){
		for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
			if(pair.first.compare(n->name()) == 0){
				for(NodeAttributeValidator nav : pair.second.getAtVals()){
					std::vector<const Node*> ns;
					for(const Node* in : node.children())
						if(n->name().compare(in->name()) == 0)
							ns.push_back(in);
					validateAttributes(ns, nav);
				}
				validateAttributes(*n, pair.second);
			}
		}
	}
}

const kul::xml::Node* kul::xml::NodeFactory::create(const char*f, const char* root, const NodeValidator& v){
	pugi::xml_document doc;	
	pugi::xml_parse_result result = doc.load_file(f);
	if(!result){
		//TODO GET LINE # OF ERROR IN FILE
		if(result.status == pugi::xml_parse_status::status_end_element_mismatch)	
			KEXCEPT(Exception, "PUGIXML Exception: " + std::string(result.description()) + " at " + (f + result.offset));
		KEXCEPT(Exception, "PUGIXML Exception: " + std::string(result.description()) + " : " + f);
	}
	if(doc.child(root).empty())
		KEXCEPT(Exception, "root element \"" + std::string(root) + "\" not found, malformed document");
	Node** n = new Node*;
	*n = new Node(0, validate(n, new std::vector<const kul::xml::Node*>(), doc.child(root), v), root);
	std::vector<const Node*> rootV; rootV.push_back(*n);
	for(const NodeAttributeValidator& nav : v.getAtVals()){
		validateAttributes(rootV, nav);
	}
	validateAttributes(**n, v);
	return *n;
}