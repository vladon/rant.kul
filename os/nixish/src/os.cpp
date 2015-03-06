/**

./src/kul/OS.cpp

Created on: 1 Jan 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul.nix (The Kommon Usage Library for Linux Distros).

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

const char* kul::Env::CWD(){
	return getcwd(NULL, 0);
}
bool kul::Env::CWD(const char* c){
	return chdir(c);
}
bool kul::Env::CWD(const kul::Dir& d){
	return chdir(d.path().c_str());
}
const char* kul::Env::GET(const char* c){
	return getenv(c);
}
void kul::Env::SET(const char* var, const char* val){
	setenv(var, val, 1);
}
const char* kul::Env::SEP(){
	return ":";
}

const std::string kul::Dir::LOCL(const std::string& s){
	std::string local(s);
	kul::String::replaceAll(local, "\\", "/");
	return local;
}
const std::string kul::Dir::REAL(const std::string& s) throw(fs::Exception){
	char* expanded = realpath(s.c_str() , NULL);
	if(expanded){
		std::string dir(expanded);
		return dir;
	}
	KEXCEPT(fs::Exception, "Directory \"" + s + "\" does not exist");
}
const std::string kul::Dir::PRNT(const std::string& s){
	return std::string(s.substr(0, s.rfind(SEP())));
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
	if(!prnt().is()) prnt().mk();
	return mkdir(locl().c_str(), 0777) == 0;
}
void kul::Dir::rm() const{
	if(is()){
		for(const kul::File& f : files())
			f.rm();
		for(const kul::Dir& d : dirs())
			d.rm();
		rmdir(path().c_str());
	}
}
bool kul::Dir::root() const{
	return real().size() == 1;
}

const std::vector<kul::Dir> kul::Dir::dirs(bool incHidden) const throw(fs::Exception){
	if(!is()) KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	std::vector<Dir> ds;
	DIR *dir = opendir(real().c_str());
	struct dirent *entry = readdir(dir);
	while (entry != NULL){
		if (entry->d_type == DT_DIR){
			std::string d(entry->d_name);
			if(d.compare(".") != 0 && d.compare("..") != 0
				&& !(d.substr(0, 1).compare(".") == 0 && !incHidden))
			ds.push_back(Dir(JOIN(real(), entry->d_name)));
		}
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
		if (entry->d_type == DT_REG){
			fs.push_back(File(entry->d_name, *this));
		}
		entry = readdir(dir);
	}
	closedir(dir);
	if(recursive){
		for(const kul::Dir& d : dirs()){
			std::vector<kul::File> tFs = d.files(true);
			fs.insert(fs.end(), tFs.begin(), tFs.end());
		}
	}
	return fs;
}


bool kul::File::is() const{
	return (bool) std::ifstream(real().c_str());
}
bool kul::File::rm() const{
	if(is()){
		unlink(real().c_str());
		return true;
	}
	return false;
}
bool kul::File::cp(const File& f) const{
	std::ifstream src(real(), std::ios::binary);
	std::ofstream dst(f.real(), std::ios::binary);
	return (bool) (dst << src.rdbuf());
}
bool kul::File::mv(const Dir& d) const{
	return false;
}

const kul::Dir kul::os::userDir(){
	return Dir(Env::GET("HOME"));
}
const kul::Dir kul::os::userAppDir(const std::string& app){
	return Dir(Dir::JOIN(Env::GET("HOME"), "." + app));
}
const std::string kul::os::newLine(){
	return "\n";
}
