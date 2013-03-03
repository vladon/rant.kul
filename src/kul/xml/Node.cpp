/*
 * Node.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 */

#include "glog/logging.h"

#include "kul/xml.hpp"

const kul::xml::Node& kul::xml::Node::operator[](const std::string& s) const  throw (Exception){
	for(const Node* n : this->children())
		if(s.compare(n->name()) == 0)
			return *n;
	throw Exception(__FILE__, __LINE__, "XML Exception: Element " + s + " doesn't exist under node " + this->name());
}

const kul::xml::Node& kul::xml::Node::operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception){
	for(const Node* n : this->children())
			if(c.compare(n->name()) == 0){
				try{
					if(n->att(a).compare(v) == 0) return *n;
				}catch(const Exception& e){ LOG(INFO) << e.what();}
			}
	throw Exception(__FILE__, __LINE__, "XML Exception: No Element " + c + " contains attribute " + a + " with value " + v + " under node " + this->name());
}

const std::string kul::xml::Node::txt() const throw (Exception){
	const TextNode* txt = static_cast<const TextNode*>(this);
	if(txt) return txt->txt();
	throw Exception(__FILE__, __LINE__, "XML Exception: " + std::string(this->name()) + " is not a text node");
}

const std::string kul::xml::Node::att(const std::string& s) const throw (Exception){
	for(const std::pair<const std::string, std::string> p : this->attributes())
		if(s.compare(p.first) == 0)
			return p.second;
	throw Exception(__FILE__, __LINE__, "XML Exception: Attribute " + s + " doesn't exist under node " + this->name());
}
