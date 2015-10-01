
KUL_GIT=master
YAML_GIT=master
HASH_GIT=master

OS =
CXX=g++
CXXFLAGS=-std=c++1y -O3 -g0 -Wall -c -fmessage-length=0 
//CLANGFLAGS=-stdlib=libc++
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/os/$(OS)/inc \
		-I$(CURDIR)/os/nixish/inc \
		-I$(CURDIR)/ext/yaml/$(YAML_GIT)/src \
		-I$(CURDIR)/ext/sparsehash/$(HASH_GIT)/include
LIB = bin/libkul.a

entry:
	@@echo "Options include"
	@@echo "make nix"
	@@echo "make bsd"
	@@echo "make clean 	- removes bin dirs of kul/maiken"
	@@echo "make clean-all 	- removes bin dirs of kul/maiken/yaml"

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
	@if [ ! -d "$(CURDIR)/ext/sparsehash/$(HASH_GIT)" ]; then \
		git clone http://github.com/dekken/google.sparsehash.git --branch $(HASH_GIT) ext/sparsehash/$(HASH_GIT); \
		cd ./ext/sparsehash/$(HASH_GIT); bash ./configure --prefix=$(CURDIR)/ext/sparsehash/$(HASH_GIT); \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/$(HASH_GIT); \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/$(HASH_GIT) install; \
	fi;

pugi:
	@if [ ! -d "$(CURDIR)/ext/yaml/$(YAML_GIT)" ]; then \
		git clone https://github.com/dekken/yaml.cpp.git --branch $(YAML_GIT) ext/yaml/$(YAML_GIT); \
	fi;
	@if [ ! -d "$(CURDIR)/ext/yaml/$(YAML_GIT)/bin" ]; then \
		mkdir ext/yaml/$(YAML_GIT)/bin; \
	fi;
	$(CXX) $(CXXFLAGS) -I$(CURDIR)/ext/yaml/$(YAML_GIT)/src -o "$(CURDIR)/ext/yaml/$(YAML_GIT)/bin/yaml.o" "$(CURDIR)/ext/yaml/$(YAML_GIT)/src/yaml.cpp";
	ar -r $(CURDIR)/ext/yaml/$(YAML_GIT)/bin/libyaml.a "$(CURDIR)/ext/yaml/$(YAML_GIT)/bin/yaml.o"

clean:
	rm -rf bin

clean-all:
	rm -rf $(CURDIR)/ext
