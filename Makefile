
CXX = g++ -std=c++0x -O3 -g0 -Wall -c -fmessage-length=0
INCS = 	-I$(CURDIR)/inc \
		-I$(CURDIR)/ext/pugixml/trunk/src \
		-I$(CURDIR)/ext/glog/make/include \
		-I$(CURDIR)/ext/sparsehash/make/include
LIB = bin/libkul.a

all:
	@@echo "Making libkul.a"
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

	@if [ ! -d "$(CURDIR)/ext/pugixml/trunk/scripts" ]; then \
		svn co http://pugixml.googlecode.com/svn/tags/latest/ ext/pugixml/trunk; \
		cd ./ext/pugixml/trunk/scripts; cmake CMakeLists.txt; \
		$(MAKE) -C $(CURDIR)/ext/pugixml/trunk/scripts; \
	fi;

	@for f in $(shell find src -type f -name '*.cpp'); do \
		if [ ! -d "$(CURDIR)/bin/$$f" ]; then \
			mkdir -p $(CURDIR)/bin/$$f; \
		fi; \
		$(CXX) $(CXXFLAGS) $(INCS) -o "$(CURDIR)/bin/$$f.o" "$(CURDIR)/$$f"; \
	done;
	$(MAKE) files

files:
	$(eval FILES := $(foreach dir,$(shell find $(CURDIR)/bin -type f -name *.o),$(dir)))	
	cd $(CURDIR); ar -r "$(LIB)" $(FILES)

clean:
	rm -rf bin

clean-all: clean
	rm -rf ./ext/glog/make
	rm -rf ./ext/sparsehash/make
	rm -rf ./ext/pugixml/trunk/scripts/libpugixml.a