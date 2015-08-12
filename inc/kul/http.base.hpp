/**

./inc/kul/http.hpp

Created on: 3 Sept 2014

Copyright (c) 2014, Philip Deegan

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
#ifndef _KUL_HTTP_BASE_HPP_
#define _KUL_HTTP_BASE_HPP_

#include "kul/hash.hpp"
#include "kul/string.hpp"

namespace kul{ namespace http{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class ARequest{
	protected:
		kul::hash::map::S2S atts;
		kul::hash::set::String mis;
		virtual const std::string method() const = 0;
		virtual const std::string version() const = 0;
		virtual const std::string toString(const std::string& host, const std::string& res) const = 0;
		virtual void handle(const kul::hash::map::S2S& h, const std::string& b) = 0;
		void handle(std::string b){
			kul::hash::map::S2S h;
			std::string line;
			std::stringstream ss(b);
			while(std::getline(ss, line)){
				if(line.size() <=2) {
					b.erase(0, b.find("\n") + 1);
					break;
				}
				kul::String::trim(line);
				line.pop_back();
				if(line.find(":") != std::string::npos){
					std::vector<std::string> bits = kul::String::split(line, ':');
					std::string l(bits[0]);
					std::string r(bits[1]);
					kul::String::trim(l);
					kul::String::trim(r);
					h.insert(l, r);
				}else
					h.insert(line, "");
				b.erase(0, b.find("\n") + 1);
			}
			handle(h, b);
		}		
	public:
		ARequest(bool text = 1) { if(text) mime("text/html");}
		virtual ~ARequest(){}
		virtual void send(const std::string& host, const std::string& res, const int& port) = 0;
		ARequest& attribute(const std::string& k, const std::string& v){
			atts[k] = v;
			return *this;
		}
		ARequest& mime(const std::string& m){
			mis.insert(m);
			return *this;
		}
		const kul::hash::map::S2S& 	attributes() const { return atts; }
};

class A1_1Request : public ARequest{
	protected:
		const std::string version() const{ return "HTTP/1.1";}
};

class _1_1GetRequest : public A1_1Request{
	private:
		const std::string method() const { return "GET";}
	protected:
		virtual const std::string toString(const std::string& host, const std::string& res) const{
			std::string s(method() + " /" + res);
			if(atts.size() > 0) s = s + "?";
			for(const std::pair<std::string, std::string>& p : atts) s = s + p.first + "=" + p.second + "&";
			if(atts.size() > 0) s = s.substr(0, s.size() - 1);
			s = s + " " + version();
			s = s + "\r\nHost: " + host;
			if(mis.size() > 0 ) s = s + "\r\nAccept: ";
			for(const std::string& m : mis) s = s + m + ", ";
			if(mis.size() > 0 ) s = s.substr(0, s.size() - 2);
			s = s + "\r\nContent-Length: 0";
			s = s + "\r\nConnection: close\r\n\r\n";
			return s;
		};
	public:
		virtual void send(const std::string& host, const std::string& res = "", const int& port = 80);
};

class _1_1PostRequest : public A1_1Request{
	private:
		const std::string method() const { return "POST";}
	protected:
		virtual const std::string toString(const std::string& host, const std::string& res) const{
			std::string s(method() + " /" + res + " " + version());
			s = s + "\r\nHost: " + host;
			if(mis.size() > 0 ) s = s + "\r\nAccept: ";
			for(const std::string& m : mis) s = s + m + ", ";
			if(mis.size() > 0 ) s = s.substr(0, s.size() - 2);
			int cs = 0;
			for(const std::pair<std::string, std::string>& p : atts) cs += p.first.size() + p.second.size() + 1;
			if(atts.size() > 0) cs += atts.size() - 1;
			s = s + "\r\nContent-Length: " + std::to_string(cs);
			s = s + "\r\nConnection: close\r\n\r\n";
			for(const std::pair<std::string, std::string>& p : atts) s = s + p.first + "=" + p.second + "&";
			if(atts.size() > 0) s = s.substr(0, s.size() - 1);
			return s;
		};
	public:
		virtual void send(const std::string& host, const std::string& res = "", const int& port = 80);
};

class AServer{
	protected:
		short p;
		long int s;
		kul::hash::set::String rhs;;
		kul::hash::map::S2S asAttributes(std::string a){
			kul::hash::map::S2S atts;
			if(a.size() == 0) return atts;
			if(a[0] == '?') a = a.substr(1);
			for(const std::string& p : kul::String::split(a, '&')){
				if(p.find("=") != std::string::npos){
					std::vector<std::string> bits = kul::String::split(p, '=');
					atts[bits[0]] = bits[1];
				}else
					atts[p] = "";
			}
			return atts;
		}
	public:
		AServer(const short& p) : p(p), s(kul::Now::MILLIS()){}
		virtual ~AServer(){}
		virtual void listen() throw(kul::http::Exception) = 0;
		virtual void stop() = 0;
		
		virtual const std::pair<kul::hash::set::String, std::string> handle(const std::string& res, kul::hash::map::S2S atts){
			using namespace std;
			kul::hash::set::String set;
			stringstream ss;
			ss << res << " : ";	
			for(const pair<string, string> p : atts)
				ss << p.first << "=" << p.second << " ";
			set.insert("Content-Length: " + std::to_string(ss.str().size()));
			return std::pair<kul::hash::set::String, std::string>(set, ss.str());
		}
		const long int	up()	{ return s - kul::Now::MILLIS(); }
		const short&	port()	{ return p; }

		void addResponseHeader(const std::string& rh){ 
			rhs.insert(rh);
		}
};


}// END NAMESPACE http
}// END NAMESPACE kul

#endif /* _KUL_HTTP_BASE_HPP_ */