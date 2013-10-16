
CXX=g++
CXXFLAGS=-std=c++0x -O3 -g0 -Wall -c -fmessage-length=0
#CLANGFLAGS=-stdlib=libc++
CLANGFLAGS=
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/ext/pugixml/trunk/src \
		-I$(CURDIR)/ext/glog/make/include \
		-I$(CURDIR)/ext/sparsehash/make/include
LIB = bin/libkul.a

all:
	$(MAKE) libs
	$(MAKE) kul

kul:
	@@echo "Making libkul.a"
	@for f in $(shell find src -type f -name '*.cpp'); do \
		if [ ! -d "$(CURDIR)/bin/$$f" ]; then \
			mkdir -p $(CURDIR)/bin/$$f; \
		fi; \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$f.o" "$(CURDIR)/$$f"; \
	done;
	$(MAKE) files
	
libs:
	@if [ ! -d "$(CURDIR)/ext/glog/make" ]; then \
		svn co http://google-glog.googlecode.com/svn/trunk/ ext/glog/trunk; \
		mkdir ./ext/glog/make; \
		cd ./ext/glog/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/glog/make; \
		$(MAKE) -C $(CURDIR)/ext/glog/make; \
		$(MAKE) -C $(CURDIR)/ext/glog/make install; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/sparsehash/make" ]; then \
		svn co http://sparsehash.googlecode.com/svn/trunk/ ext/sparsehash/trunk; \
		mkdir ./ext/sparsehash/make; \
		cd ./ext/sparsehash/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make install; \
	fi;

	@if [ ! -d "$(CURDIR)/ext/pugixml/trunk" ]; then \
		svn co http://pugixml.googlecode.com/svn/tags/latest/ ext/pugixml/trunk; \
		cd ./ext/pugixml/trunk/scripts; cmake CMakeLists.txt; \
		$(MAKE) -C $(CURDIR)/ext/pugixml/trunk/scripts; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/pugixml/trunk/scripts" ]; then \
		cd ./ext/pugixml/trunk; svn up; \
		cd ./ext/pugixml/trunk/scripts; cmake CMakeLists.txt; \
		$(MAKE) -C $(CURDIR)/ext/pugixml/trunk/scripts; \
	fi;

clang:
	@@echo "Making glog"
	@if [ ! -d "$(CURDIR)/ext/glog/llvm" ]; then \
		svn co http://google-glog.googlecode.com/svn/trunk/ ext/glog/trunk; \
		mkdir ./ext/glog/llvm; \
		cd ./ext/glog/llvm; bash ../trunk/configure --prefix=$(CURDIR)/ext/glog/llvm CC=$(CLANG) CXX=$(CLANG) CXXFLAGS=$(CLANGFLAGS); \
		$(MAKE) -C $(CURDIR)/ext/glog/llvm;\
		$(MAKE) -C $(CURDIR)/ext/glog/llvm install; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/sparsehash/make" ]; then \
		svn co http://sparsehash.googlecode.com/svn/trunk/ ext/sparsehash/trunk; \
		mkdir ./ext/sparsehash/make; \
		cd ./ext/sparsehash/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make install; \
	fi;

	CXX=$(CLANG)
	@@echo "Making pugixml"
	@if [ ! -d "$(CURDIR)/ext/pugixml/llvm" ]; then \
		svn co http://pugixml.googlecode.com/svn/tags/latest/ ext/pugixml/llvm; \
		cd ./ext/pugixml/llvm/scripts; cmake CMakeLists.txt; \
		$(MAKE) -C $(CURDIR)/ext/pugixml/llvm/scripts; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/pugixml/llvm/scripts" ]; then \
		cd ./ext/pugixml/llvm; svn up; \
		cd ./ext/pugixml/llvm/scripts; cmake CMakeLists.txt; \
		$(MAKE) -C $(CURDIR)/ext/pugixml/llvm/scripts; \
	fi;

files:
	$(eval FILES := $(foreach dir,$(shell find $(CURDIR)/bin -type f -name *.o),$(dir)))	
	cd $(CURDIR); ar -r "$(LIB)" $(FILES)

clean:
	rm -rf bin

clean-all: clean
	rm -rf ./ext/glog/make
	rm -rf ./ext/glog/llvm
	rm -rf ./ext/sparsehash/make
	rm -rf ./ext/pugixml/trunk/scripts
	rm -rf ./ext/pugixml/llvm/scripts
