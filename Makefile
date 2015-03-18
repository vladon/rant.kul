
VER_GIT=master

OS =
CXX=g++
CXXFLAGS=-std=c++1y -O3 -g0 -Wall -c -fmessage-length=0 
//CLANGFLAGS=-stdlib=libc++
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/os/$(OS)/inc \
		-I$(CURDIR)/os/nixish/inc \
		-I$(CURDIR)/ext/pugixml/master/src \
		-I$(CURDIR)/ext/sparsehash/master/include
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

hash:
	@if [ ! -d "$(CURDIR)/ext/sparsehash/master" ]; then \
		git clone http://github.com/dekken/sparsehash ext/sparsehash/master; \
		cd ./ext/sparsehash/master; bash ./configure --prefix=$(CURDIR)/ext/sparsehash/master; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/master; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/master install; \
	fi;

pugi:
	@if [ ! -d "$(CURDIR)/ext/pugixml/master" ]; then \
		git clone https://github.com/dekken/pugixml ext/pugixml/master; \
	fi;
	@if [ ! -d "$(CURDIR)/ext/pugixml/master/bin" ]; then \
		mkdir ext/pugixml/master/bin; \
	fi;
	$(CXX) $(CXXFLAGS) -I$(CURDIR)/ext/pugixml/master/src -o "$(CURDIR)/ext/pugixml/master/bin/pugixml.o" "$(CURDIR)/ext/pugixml/master/src/pugixml.cpp";
	ar -r $(CURDIR)/ext/pugixml/master/bin/libpugixml.a "$(CURDIR)/ext/pugixml/master/bin/pugixml.o"

clean:
	rm -rf bin

clean-all:
	rm -rf $(CURDIR)/ext
