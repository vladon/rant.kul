
VER_GIT=$(VER_GIT)

OS =
CXX=g++
CXXFLAGS=-std=c++1y -O3 -g0 -Wall -c -fmessage-length=0 
//CLANGFLAGS=-stdlib=libc++
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/os/$(OS)/inc \
		-I$(CURDIR)/os/nixish/inc \
		-I$(CURDIR)/ext/pugixml/$(VER_GIT)/src \
		-I$(CURDIR)/ext/sparsehash/$(VER_GIT)/include
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
	@if [ ! -d "$(CURDIR)/ext/sparsehash/$(VER_GIT)" ]; then \
		git clone http://github.com/dekken/google.sparsehash --branch $(VER_GIT) ext/sparsehash/$(VER_GIT); \
		cd ./ext/sparsehash/$(VER_GIT); bash ./configure --prefix=$(CURDIR)/ext/sparsehash/$(VER_GIT); \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/$(VER_GIT); \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/$(VER_GIT) install; \
	fi;

pugi:
	@if [ ! -d "$(CURDIR)/ext/pugixml/$(VER_GIT)" ]; then \
		git clone https://github.com/dekken/pugixml --branch $(VER_GIT) ext/pugixml/$(VER_GIT); \
	fi;
	@if [ ! -d "$(CURDIR)/ext/pugixml/$(VER_GIT)/bin" ]; then \
		mkdir ext/pugixml/$(VER_GIT)/bin; \
	fi;
	$(CXX) $(CXXFLAGS) -I$(CURDIR)/ext/pugixml/$(VER_GIT)/src -o "$(CURDIR)/ext/pugixml/$(VER_GIT)/bin/pugixml.o" "$(CURDIR)/ext/pugixml/$(VER_GIT)/src/pugixml.cpp";
	ar -r $(CURDIR)/ext/pugixml/$(VER_GIT)/bin/libpugixml.a "$(CURDIR)/ext/pugixml/$(VER_GIT)/bin/pugixml.o"

clean:
	rm -rf bin

clean-all:
	rm -rf $(CURDIR)/ext
