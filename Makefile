
OS =
CXX = g++ -std=c++0x -O3 -g0 -Wall -c -fmessage-length=0
INCS = -I$(CURDIR)/inc -I$(CURDIR)/ext/pugixml/trunk/src -I$(CURDIR)/ext/glog/make/include -I$(CURDIR)/ext/sparsehash/make/include
LIB = bin/libkul.a

debian:
  @@echo "Making for debian"
	$(eval CXXFLAGS := $(CXXFLAGS))
	$(eval OS := debian)
	$(MAKE) general CXXFLAGS=$(CXXFLAGS) OS=$(OS)	
	
fedora:
	@@echo "Macking for fedora"
	$(eval CXXFLAGS := $(CXXFLAGS))
	$(eval OS := fedora)
	$(MAKE) general CXXFLAGS=$(CXXFLAGS) OS=$(OS)
	
general:
	@@echo CXXFLAGS = $(CXXFLAGS)	
	svn co http://google-glog.googlecode.com/svn/trunk/ ext/glog/trunk
	rm -rf ./ext/glog/make
	mkdir ./ext/glog/make
	cd ./ext/glog/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/glog/make
	$(MAKE) -C ext/glog/make
	$(MAKE) -C ext/glog/make install
	
	svn co http://sparsehash.googlecode.com/svn/trunk/ ext/sparsehash/trunk
	rm -rf ./ext/sparsehash/make
	mkdir ./ext/sparsehash/make
	cd ./ext/sparsehash/make; bash ../trunk/configure --prefix=$(CURDIR)/ext/sparsehash/make
	$(MAKE) -C ext/sparsehash/make
	$(MAKE) -C ext/sparsehash/make install

	svn co http://pugixml.googlecode.com/svn/tags/latest/ ext/pugixml/trunk
	cd ./ext/pugixml/trunk/scripts; cmake CMakeLists.txt

	rm -rf bin
	 
	@for f in $(shell find src -type f -name *.cpp); do \
		mkdir -p $(CURDIR)/bin/$$f; \
		$(CXX) $(INCS) -o "$(CURDIR)/bin/$$f.o" "$(CURDIR)/$$f"; \
	done;

	$(eval OBJS:=)
	$(eval FILES := $(foreach dir,$(shell find bin -type f -name *.o),$(dir)))
	
	cd $(CURDIR); ar -r "$(LIB)" $(FILES)
