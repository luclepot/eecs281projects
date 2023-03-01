#include <queue>
#include "Escaper.h"



int main(int argc, char** argv) {
	std::cout << std::fixed << std::setprecision(2); 
	std::ios_base::sync_with_stdio(false); 
	Escaper m; 
	int ret;
	
	ret = m.getOptions(argc, argv);
	if (ret >= 0) 
		return ret;

	ret = m.getData();
	if (ret >= 0)
		return ret; 
	
	m.escapeMine();

	return 0; 
}