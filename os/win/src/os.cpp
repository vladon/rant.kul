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
#include <io.h>
#include <fstream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <algorithm>
#include <process.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "kul/os.hpp"
#include "kul/def.hpp"

#include <thread>
unsigned int kul::cpu::cores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return sysinfo.dwNumberOfProcessors;
}
unsigned int kul::cpu::threads() {
	return std::thread::hardware_concurrency();
}

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
	return _chdir(c);
}
bool kul::env::CWD(const kul::Dir& d){
	return _chdir(d.path().c_str());
}
const char* kul::env::GET(const char* c){
	return getenv(c);
}
void kul::env::SET(const char* var, const char* val){
	putenv(std::string(std::string(var) + "=" + std::string(val)).c_str());
}
const char kul::env::SEP(){
	return ';';
}

const std::string kul::Dir::LOCL(std::string s){
	kul::String::replaceAll(s, "/", "\\");
	return s;
}
const std::string kul::Dir::REAL(const std::string& s) throw(fs::Exception){
	char* expanded = _fullpath(NULL, s.c_str(), _MAX_PATH);
	if(expanded){
		std::string dir(expanded);
		delete expanded;
		return dir;
	}
	KEXCEPT(fs::Exception, "Item: \"" + s + "\" does not exist");
}
const std::string kul::Dir::SEP(){
	return std::string("\\");
}
bool kul::Dir::is() const{
	DWORD ftyp = GetFileAttributesA(path().c_str());
	return (ftyp != INVALID_FILE_ATTRIBUTES && ftyp & FILE_ATTRIBUTE_DIRECTORY);
}
bool kul::Dir::mk() const{
	if(path().empty()) return false;
	if(!prnt().is()) prnt().mk();
	return CreateDirectory(locl().c_str(), NULL);
}
void kul::Dir::rm() const{
	if(is()){
		for(const kul::File& f : files(false))
			unlink(f.name().c_str());
		for(const kul::Dir& d : dirs())
			d.rm();
		_rmdir(path().c_str());
	}
}
bool kul::Dir::root() const{
	return is() && real().size() == 3;
}

const std::vector<kul::Dir> kul::Dir::dirs(bool incHidden) const throw(Exception){
	if(!is()) KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	std::vector<Dir> dirs;
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char sPath[2048];
	sprintf(sPath, "%s\\*.*", path().c_str());
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
		KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	do{
		if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0){
			sprintf(sPath, "%s\\%s", path().c_str(), fdFile.cFileName);
			if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY){
				if(!incHidden && std::string(sPath).substr(std::string(sPath).rfind(kul::Dir::SEP()) + 1).substr(0, 1).compare(".") == 0) continue; 
				dirs.push_back(Dir(sPath));
			}
		}
	}while(FindNextFile(hFind, &fdFile));
	
	FindClose(hFind);
	
	return dirs;
}

const std::vector<kul::File> kul::Dir::files(bool recursive) const throw(fs::Exception){
	if(!is()) KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	std::vector<File> fs;
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char sPath[2048];
	sprintf(sPath, "%s\\*.*", path().c_str());
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) 
		KEXCEPT(fs::Exception, "Directory : \"" + path() + "\" does not exist");

	do{
		if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0){
			sprintf(sPath, "%s\\%s", path().c_str(), fdFile.cFileName);
			if(!(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)){
				std::string f(sPath);
				fs.push_back(File(f.substr(f.rfind(kul::Dir::SEP()) + 1), *this));
			}
		}
	}while(FindNextFile(hFind, &fdFile));
	FindClose(hFind);
	if(recursive){
		for(const Dir& d : dirs()){
			std::vector<File> tFiles = d.files(true);
			fs.insert(fs.end(), tFiles.begin(), tFiles.end());
		}
	}
	return fs;
}


bool kul::File::is() const{
	return (bool) std::ifstream(d.join(n).c_str());
}
bool kul::File::rm() const{
	if(is()){
		unlink(d.join(n).c_str());
		return true;
	}
	return false;
}
bool kul::File::cp(const File& f) const{
	std::ifstream src(d.join(n), std::ios::binary);
	std::ofstream dst(f.dir().join(f.name()), std::ios::binary);
	return (bool) (dst << src.rdbuf());
}
bool kul::File::mv(const File& f) const{
	return std::rename(this->full().c_str(), f.full().c_str());
}

const kul::Dir kul::os::userDir(){
	const char* h = env::GET("HOME");
	if(h) return kul::Dir(h);
	return kul::Dir(std::string(env::GET("HOMEDRIVE")) + std::string(env::GET("HOMEPATH")));
}
const kul::Dir kul::os::userAppDir(const std::string& app){
	return kul::Dir(userDir().join(app));
}
const std::string kul::os::newLine(){
	#if (_MSC_VER >= 1800 )
	return "\n";
	#else
	return "\r\n";
	#endif
}
