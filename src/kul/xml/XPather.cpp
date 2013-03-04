/*
 * XPather.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 *	
 *  Methods provided purely for convenience, 
 */

#include "kul/ext/google.hpp"

#include "kul/xml.hpp"

void kul::xml::XPather::writeNode(const char*x, const Node& node){

}

void kul::xml::XPather::writeText(const char*x, const char*t){
	//doc.select_single_node(x).node().set_value(t);	
	doc.select_single_node(x).node().append_child(pugi::node_pcdata).set_value(t);	
}

void kul::xml::XPather::writeAttr(const char*x, const char*n, const char*v){
	
}
