#include "sillyDB.h"

using std::boolalpha; 

int main(int argc, char ** argv) {
	std::ios_base::sync_with_stdio(false);
	cin >> boolalpha; 
	cout << boolalpha; 
	sillyDB sdb;
	int ret = sdb.processArgs(argc, argv);
	if (ret >= 0)
		return ret; 
	sdb.mainLoop();
	return 0;
}