/**

./src/kul/OS.cpp

Created on: 1 Jan 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul.win (The Kommon Usage Library for Windows Environments).

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

const char* kul::Env::CWD(){
	return getcwd(NULL, 0);
}
bool kul::Env::CWD(const char* c){
	return _chdir(c);
}
bool kul::Env::CWD(const kul::Dir& d){
	return _chdir(d.path().c_str());
}
const char* kul::Env::GET(const char* c){
	return getenv(c);
}
void kul::Env::SET(const char* var, const char* val){
	putenv(std::string(std::string(var) + "=" + std::string(val)).c_str());
}
const char* kul::Env::SEP(){
	return ";";
}

const std::string kul::Dir::LOCL(const std::string& s){
	std::string local(s);
	kul::String::replaceAll(local, "/", "\\");
	return local;
}
const std::string kul::Dir::REAL(const std::string& s) throw(fs::Exception){
	char* expanded = _fullpath(NULL, s.c_str(), _MAX_PATH);
	if(expanded){
		std::string dir(expanded);
		delete expanded;
		return dir;
	}
	KEXCEPT(fs::Exception, "Directory \"" + s + "\" does not exist");
}
const std::string kul::Dir::PRNT(const std::string& s){
	return std::string(s.substr(0, s.rfind(SEP())));
}
const std::string kul::Dir::SEP(){
	return std::string("\\");
}

bool kul::Dir::is() const{
	DWORD ftyp = GetFileAttributesA(real().c_str());
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
	return real().size() == 3;
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
	std::string home(Env::GET("HOME"));
	if(home.size() == 0)
		home = std::string(Env::GET("HOMEDRIVE")) + std::string(Env::GET("HOMEPATH"));
	return Dir(home);
}
const kul::Dir kul::os::userAppDir(const std::string& app){
	return Dir(userDir().join(app));
}
const std::string kul::os::newLine(){
	#if (_MSC_VER >= 1800 )
	return "\n";
	#else
	return "\r\n";
	#endif
}



// bool kul::os::path::exists(const char*c){
// 	struct stat buffer;   
// 	return (stat (c, &buffer) == 0); 
// }
