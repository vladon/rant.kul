/**

./src/kul/xml/Node.cpp

Created on: 14 Jan 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "kul/xml.hpp"
#include "kul/log.hpp"

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
				}catch(const Exception& e){ KLOG(ERROR) << e.what();}
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
