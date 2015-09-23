

#include "kul.test.hpp"

int main(int argc, char* argv[]){
	try{
		kul::Test();
	}catch(const kul::Exception& e){ 
		KERR << e.stack();
	}
	return 0;
}

