
VER_GIT=master

CXX=g++
CXXFLAGS=-std=c++1y -O3 -g0 -Wall -c -fmessage-length=0 
CLANG=clang++
INTEL=icpc
//CLANGFLAGS=-stdlib=libc++
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/os/nixish/inc \
		-I$(CURDIR)/ext/pugixml/trunk/src \
		-I$(CURDIR)/ext/sparsehash/trunk/include
LIB = bin/libkul.a

entry:
	@@echo "Options include"
	@@echo "make nix"
	@@echo "make bsd"
	@@echo "make clean 	- removes bin dirs of kul/maiken"
	@@echo "make clean-all 	- removes bin dirs of kul/maiken/pugixml"

nix:
	@@echo "Making nix libkul.a"
	$(MAKE) com
	@for f in $(shell find ./os/nix/src -type f -name '*.cpp'); do \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$(basename $$f).o" "$(CURDIR)/$$f"; \
	done;
	$(MAKE) files

bsd:
	@@echo "Making bsd libkul.a"
	@for f in $(shell find ./os/bsd/src -type f -name '*.cpp'); do \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$(basename $$f).o" "$(CURDIR)/$$f"; \
	done;
	$(MAKE) files
	$(MAKE) com

com:
	$(MAKE) libs
	@if [ ! -d "$(CURDIR)/bin" ]; then \
		mkdir -p $(CURDIR)/bin; \
	fi;
	@for f in $(shell find src -type f -name '*.cpp'); do \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$(basename $$f).o" "$(CURDIR)/$$f"; \
	done;
	@for f in $(shell find ./os/nixish/src -type f -name '*.cpp'); do \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$(basename $$f).o" "$(CURDIR)/$$f"; \
	done;

files:
	$(eval FILES := $(foreach dir,$(shell find $(CURDIR)/bin -type f -name *.o),$(dir)))
	cd $(CURDIR); ar -r "$(LIB)" $(FILES)

libs:
	$(MAKE) hash
	$(MAKE) pugi

clang:
	$(MAKE) hash
	$(MAKE) pugi CXX=$(CLANG)

intel:
	$(MAKE) hash
	$(MAKE) pugi CXX=$(INTEL)

hash:
	@if [ ! -d "$(CURDIR)/ext/sparsehash/trunk" ]; then \
		svn co http://sparsehash.googlecode.com/svn/trunk/ ext/sparsehash/trunk; \
		cd ./ext/sparsehash/trunk; bash ./configure --prefix=$(CURDIR)/ext/sparsehash/trunk; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/trunk; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/trunk install; \
	fi;

pugi:
	@if [ ! -d "$(CURDIR)/ext/pugixml/trunk" ]; then \
		svn co http://pugixml.googlecode.com/svn/tags/latest/ ext/pugixml/trunk; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/pugixml/trunk/bin" ]; then \
		mkdir ext/pugixml/trunk/bin; \
	fi;
	$(CXX) $(CXXFLAGS) -I$(CURDIR)/ext/pugixml/trunk/src -o "$(CURDIR)/ext/pugixml/trunk/bin/pugixml.o" "$(CURDIR)/ext/pugixml/trunk/src/pugixml.cpp";
	ar -r $(CURDIR)/ext/pugixml/trunk/bin/libpugixml.a "$(CURDIR)/ext/pugixml/trunk/bin/pugixml.o"

clean:
	rm -rf bin

clean-all:
	rm -rf $(CURDIR)/ext
