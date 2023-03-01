
#include "Letterman.h"

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);

	Letterman letterman;
	int ret; 

	// get options andW check for error
	ret = letterman.getOptions(argc, argv);
	if (ret >= 0) 
		return ret; 

	// get data and check for error
	ret = letterman.getData(); 
	if (ret >= 0)
		return ret; 

	// move to main part of program
	letterman.pathFind(); 

	return 0;
} 