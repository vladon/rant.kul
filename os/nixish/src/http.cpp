/**

./src/kul/OS.cpp

Created on: 10 Sept 2014

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
#include "kul/log.hpp"
#include "kul/byte.hpp"
#include "kul/http.hpp"

void kul::http::Server::start(){
	int32_t sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int  n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("ERROR opening socket");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = this->port();
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = kul::byte::Endian::UINT32(portno);
 	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		 perror("ERROR on binding");
		 exit(1);
	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0){
			perror("ERROR on accept");
			exit(1);
		}

		bzero(buffer,256);
		n = read(newsockfd, buffer, 255);
		if (n < 0) perror("ERROR reading from socket");

		std::string b(buffer);
		kul::String::replaceAll(b, kul::os::newLine(), "");
		KLOG(DBG) << b;

		std::string d = kul::String::split(b, kul::os::newLine())[0];
		if(d.find("/?") != std::string::npos){
		 	d = d.substr(d.find("/?") + 2);
			d = d.substr(0, d.rfind(" "));
			kul::String::replaceAll(d, "%20", " ");
			while(d.find("  ") != std::string::npos)
				kul::String::replaceAll(d, "  ", " ");
		}
		KLOG(INF) << d;
		n = write(newsockfd, d.c_str(), d.length());
		close(newsockfd);
	}
}

void kul::http::Client::start(const std::string& u, const short& p){
	struct sockaddr_in serv_addr;
	struct hostent *serv;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		KLOG(ERR) << "ERROR opening socket";
		exit(0);
	}
	serv = gethostbyname(u.c_str());
	if (serv == NULL) {
		KLOG(ERR) << "No such host";
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)serv->h_addr, 
		 (char *)&serv_addr.sin_addr.s_addr,
		 serv->h_length);
	serv_addr.sin_port = kul::net::Endian::INT16(p);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		KLOG(ERR) << "ERROR connecting";
		exit(0);
	}
	KOUT(NON) << "CONNECTED";	
}

void kul::http::Client::send(const ARequest& r) const{
	r.send(this->sockfd);
	close(sockfd);
}

void kul::http::GetRequest::send(const int& sockfd) const{
	char buffer[256];
	bzero(buffer,256);
	int n = write(sockfd, message().c_str(), message().length());
	if (n < 0) KLOG(ERR) << "ERROR writing to socket";
	n = read(sockfd,buffer,255);
	if (n < 0) KLOG(ERR) << "ERROR reading from socket";
	KLOG(INF) << buffer;
}

void kul::http::PostRequest::send(const int& sockfd) const{}
