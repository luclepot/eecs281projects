#include "pokeSolver.h"

int main(int argc, char** argv) {
	// required precision/fixed args (from spec). 
	// also syncing stdio.
	std::ios_base::sync_with_stdio(false);
	cout << std::setprecision(2);
	cout << std::fixed;
	// declare pokeSolver obj
	pokeSolver ps;

	// get opts and return if failed 
	int ret = ps.getOptions(argc, argv);
	if (ret >= 0)
		return ret; 
	// run the mode selected
	ret = ps.runMode();
	if (ret >= 0)
		return ret; 
	return 0; 
}