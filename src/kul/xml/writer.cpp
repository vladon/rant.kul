/*
 * writer.cpp
 *
 *  Created on: 14 Jan 2013
 *      Author: philix
 *
 *	I thought this might get a bit hectic, so it gets its own file
 *  Methods provided purely for convenience, 
 */

 #include "kul/xml.hpp"

 class NodeWriterService{
 	public:
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
 			
 		}
 };

void kul::xml::NodeFactory::writeToFile(const char*n, const char*f, const NodeValidator& v){
	kul::file::Writer w(f);
	writeToFile(n, v, w, 0);
}

void kul::xml::NodeFactory::writeToFile(const char*n, const NodeValidator& v, kul::file::Writer& writer, int tabs){
	std::string s, t;
	for(int i = 0; i < tabs; i++) t+="\t";
	s += t + "<" + std::string(n) + ">";
	bool comment = v.isText() && v.minimum() == 0;
	if(comment) NodeWriterService::openingComment(s);
		
	if(!v.getAtVals().empty()){
		std::string a(" ");
		for(const NodeAttributeValidator& nav : v.getAtVals()){
			LOG(INFO) << "a: " << a;
			for(std::pair<std::string, std::vector<std::string > > atts : nav.getAllowedValues()){
				for(std::string val : atts.second){
					a += atts.first+"=\"" + val + "\" ";
					atts.second.erase(std::remove(atts.second.begin(), atts.second.end(), val)	, atts.second.end());
					break;
				}

			}
		}
		NodeWriterService::addAttributes(s, a);
	}
	LOG(INFO) << "s: " << s;
	writer.write(s.c_str(), !v.isText());
	for(std::pair<std::string, NodeValidator> pair  : v.getChildren()){
		writeToFile(pair.first.c_str(), pair.second, writer, ++tabs);
		tabs--;
	}

	s = !v.isText() ? t : "";
	s += "</" + std::string(n) + ">";
	if(comment) NodeWriterService::closingComment(s);
	LOG(INFO) << "s: " << s;
	writer.write(s.c_str(), true);
}
