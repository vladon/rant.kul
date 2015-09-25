/**

./src/kul/OS.cpp

Created on: 1 Jan 2013

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
#include <pwd.h>
#include <thread>
#include <fstream>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include "kul/os.hpp"

int kul::os::execReturn(const std::string& cmd){
	return system(cmd.c_str());
}
int kul::os::execNoPrintReturn(const std::string& cmd){
	const std::string call = cmd + " > /dev/null";
	return system(call.c_str());
}
int kul::os::exec(const std::string& cmd, char*const args[]){
	return execvp(cmd.c_str(), args);
}

const char* kul::env::CWD(){
	return getcwd(NULL, 0);
}
bool kul::env::CWD(const char* c){
	return chdir(c);
}
bool kul::env::CWD(const kul::Dir& d){
	return chdir(d.path().c_str());
}
const char* kul::env::GET(const char* c){
	return getenv(c);
}
void kul::env::SET(const char* var, const char* val){
	setenv(var, val, 1);
}
const char kul::env::SEP(){
	return ':';
}

const std::string kul::Dir::LOCL(std::string s){
	kul::String::replaceAll(s, "\\", "/");
	return s;
}
const std::string kul::Dir::REAL(const std::string& s) throw(fs::Exception){
	char* expanded = realpath(s.c_str() , NULL);
	if(expanded){
		std::string dir(expanded);
		return dir;
	}
	KEXCEPT(fs::Exception, "Directory \"" + s + "\" does not exist");
}
const std::string kul::Dir::SEP(){
	return std::string("/");
}

bool kul::Dir::is() const{
	DIR * d = opendir(path().c_str());
	if(d) closedir(d);
	return d;
}
bool kul::Dir::mk() const{
	if(path().empty()) return false;
	if(!parent().is()) parent().mk();
	return mkdir(locl().c_str(), 0777) == 0;
}
void kul::Dir::rm() const{
	if(is()){
		for(const auto& a : files()) a.rm();
		for(const auto& a : dirs())  a.rm();
		remove(real().c_str());
	}
}
bool kul::Dir::root() const{
	return is() && real().size() == 1;
}

const std::vector<kul::Dir> kul::Dir::dirs(bool incHidden) const throw(fs::Exception){
	if(!is()) KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	std::vector<Dir> ds;
	DIR *dir = opendir(real().c_str());
	struct dirent *entry = readdir(dir);
	while (entry != NULL){
		std::string d(entry->d_name);
		kul::Dir dd(JOIN(real(), entry->d_name));
		if(d.compare(".") != 0 && d.compare("..") != 0
			&& !(d.substr(0, 1).compare(".") == 0 && !incHidden)
			&& dd.is())
			ds.push_back(dd);
		entry = readdir(dir);
	}
	closedir(dir);
	return ds;
}

const std::vector<kul::File> kul::Dir::files(bool recursive) const throw(fs::Exception){
	if(!is()) KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	std::vector<File> fs;
	DIR *dir = opendir(path().c_str());
	struct dirent *entry = readdir(dir);
	while (entry != NULL){
		if(!kul::Dir(JOIN(real(), entry->d_name)).is())
			fs.push_back(File(entry->d_name, *this));
		entry = readdir(dir);
	}
	closedir(dir);
	if(recursive){
		for(const kul::Dir& d : dirs()){
			const std::vector<kul::File>& tFs = d.files(true);
			fs.insert(fs.end(), tFs.begin(), tFs.end());
		}
	}
	return fs;
}

bool kul::File::is() const{
	struct stat buffer;
	return (stat (d.join(n).c_str(), &buffer) == 0);
}

bool kul::File::cp(const File& f) const{
	std::ifstream src(real(), std::ios::binary);
	std::ofstream dst(f.real(), std::ios::binary);
	return (bool) (dst << src.rdbuf());
}
bool kul::File::mk() const{
	FILE* pFile;
	pFile = fopen(full().c_str(),"w");
	if(pFile != NULL){
		fclose(pFile);
	}
	return pFile != NULL;
}
bool kul::File::rm() const{
	if(is()){
		remove(real().c_str());
		return true;
	}
	return false;
}

const kul::Dir kul::os::userDir(){
	return Dir(env::GET("HOME"));
}
const kul::Dir kul::os::userAppDir(const std::string& app){
	return Dir(Dir::JOIN(env::GET("HOME"), "." + app));
}
const std::string kul::os::newLine(){
	return "\n";
}

