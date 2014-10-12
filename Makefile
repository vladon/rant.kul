
CXX=g++
CXXFLAGS=-std=c++11 -O3 -g0 -Wall -c -fmessage-length=0 
CLANG=clang++
INTEL=icpc
//CLANGFLAGS=-stdlib=libc++
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/ext/pugixml/trunk/src \
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
	$(MAKE) kullib

kullib:
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
	@if [ ! -d "$(CURDIR)/ext/sparsehash/make" ]; then \
		svn co http://sparsehash.googlecode.com/svn/trunk/ ext/sparsehash/trunk; \
		mkdir ./ext/sparsehash/make; \
		cd ./ext/sparsehash/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make; \
		$(MAKE) -C $(CURDIR)/ext/sparsehash/make install; \
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

clean-all: clean
	rm -rf $(CURDIR)/ext/sparsehash/make
	rm -rf $(CURDIR)/ext/pugixml/bin
