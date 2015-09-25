/**

./src/Writer.cpp

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

class NodeWriterService{
 	public:
 		static std::vector<std::vector<std::pair<std::string, std::string> > > attributes(const kul::xml::NodeValidator& v){

 			std::vector<std::vector<std::pair<std::string, std::string> > > entries;

 			for(const kul::xml::NodeAttributeValidator& nav : v.getAtVals()){
				std::vector<std::pair<std::string, std::string> > attPairs;
				for(const std::pair<std::string, std::vector<std::string > >& atts : nav.allowedValues())
					if(atts.second.size() == 0)
						attPairs.push_back(std::pair<std::string, std::string>(atts.first, atts.first));
					else if(atts.second.size() == 1)
						for(const std::string& val : atts.second)
							attPairs.push_back(std::pair<std::string, std::string>(atts.first, val));
					else{
						for(const std::string& val : atts.second){
							std::vector<std::string> values; values.push_back(val);
							kul::hash::map::S2VT<std::string> valMap;
							valMap.insert(std::pair<std::string, std::vector<std::string> >(atts.first, values));
							for(const kul::xml::NodeAttributeValidator& nav1 : v.getAtVals())
								for(const std::pair<std::string, std::vector<std::string > >& atts1 : nav1.allowedValues()){
									if(atts.first.compare(atts1.first) == 0 && atts1.second.size() > 1)
										continue;
									valMap.insert(atts1);
								}
							std::vector<kul::xml::NodeAttributeValidator> newNAVs;
							newNAVs.push_back(kul::xml::NodeAttributeValidator(valMap, 0, 0));
							//Unique/Mandatory aren't used in this
							kul::xml::NodeValidator newNV(newNAVs, v.minimum(), v.maximum(), v.isText());
							std::vector<std::vector<std::pair<std::string, std::string> > > nEntries = attributes(newNV);
							entries.insert(entries.begin(), nEntries.begin(), nEntries.end());
						}
						return entries;
					}
				entries.push_back(attPairs);
			}
 			return entries;
 		}
 		static void addAttributes(std::string& s, std::string& a){
			 s = s.substr(0, s.find(">")) + a + ">";
 		}
 		static void openingComment(std::string& s){
 			s = s.substr(0, s.find("<") + 1) + "!--" + s.substr(s.find("<") + 1);
 		}
 		static void closingComment(std::string& s){
 			s = s.substr(0, s.find(">")) + "-->";
 		}
 		static void comment(std::string& s){
 			s = "<!-- " + s + "-->";
 		}
 		static void addTabs(std::string& s, const int& tabs){
 			for(int i = 0; i < tabs; i++) s+="\t";
 		}
 };

void kul::xml::NodeFactory::writeToFile(const char*n, const char*f, const NodeValidator& v){
	kul::io::Writer w(f);
	w.write("<?xml version=\"1.0\"?>", true);
	writeToFile(n, v, w, 0);
}

void kul::xml::NodeFactory::writeToFile(const char*n, const NodeValidator& v, kul::io::Writer& writer, int tabs){
	std::string s, t;
	NodeWriterService::addTabs(t, tabs);
	s += t + "<" + std::string(n) + ">";
	bool comment = v.isText() && v.minimum() == 0;
	if(comment) NodeWriterService::openingComment(s);

	if(!v.getAtVals().empty()){

		for(const std::vector<std::pair<std::string, std::string> >&  vecPairs : NodeWriterService::attributes(v)){
			s = t + "<" + std::string(n) + ">";
			if(comment) NodeWriterService::openingComment(s);
			std::string a;
			for(std::pair<std::string, std::string>  pairs : vecPairs)
				a += " " + pairs.first+"=\"" + pairs.second + "\"";
			NodeWriterService::addAttributes(s, a);

			writer.write(s.c_str(), !v.isText());
			for(const std::pair<std::string, NodeValidator>& pair  : v.getChildren()){
				writeToFile(pair.first.c_str(), pair.second, writer, ++tabs);
				tabs--;
			}
			s = !v.isText() ? t : "";
			s += "</" + std::string(n) + ">";
			if(comment) NodeWriterService::closingComment(s);

			writer.write(s.c_str(), true);
			int i = 0;
			if(v.maximum() > 0 && i == v.maximum()){ // print the remaining possibile attributes values
				break;
			}
		}
	}
	else{

		writer.write(s.c_str(), !v.isText());
		for(const std::pair<std::string, NodeValidator>& pair  : v.getChildren()){
			writeToFile(pair.first.c_str(), pair.second, writer, ++tabs);
			tabs--;
		}
		s = !v.isText() ? t : "";
		s += "</" + std::string(n) + ">";
		if(comment) NodeWriterService::closingComment(s);

		writer.write(s.c_str(), true);
	}
}


