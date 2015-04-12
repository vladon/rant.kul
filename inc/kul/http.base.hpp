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

#include "kul/log.hpp"
#include "kul/hash.hpp"

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
		virtual void handle(const std::string& s){
			KLOG(INF) << s;
		}
	public:
		ARequest(bool text = 1) { if(text) mime("text/html");}
		virtual ~ARequest(){}
		virtual void send(const std::string& host, const int& port, const std::string& res) = 0;
		void attribute(const std::string& k, const std::string& v){
			atts[k] = v;
		}void mime(const std::string& m){
			mis.insert(m);
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
		virtual void send(const std::string& host, const int& port = 80, const std::string& res = "");
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
		virtual void send(const std::string& host, const int& port = 80, const std::string& res = "");
};

class AServer{
	protected:
		long int s;
		short p;
	public:
		AServer(const short& p) : s(kul::Now::MILLIS()), p(p){}
		virtual ~AServer(){}
		virtual void start() throw(kul::http::Exception) = 0;
		virtual std::string handle(const std::string& res, const std::string& atts) throw(kul::http::Exception) = 0;
		const long int	up()	{ return s - kul::Now::MILLIS(); }
		const short&	port()	{ return p; }
};


}// END NAMESPACE http
}// END NAMESPACE kul

#endif /* _KUL_HTTP_BASE_HPP_ */