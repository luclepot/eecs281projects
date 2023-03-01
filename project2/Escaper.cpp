#include "Escaper.h"


// twist on the old longOpts classic; process and check validity of 
// command line input options. 
int Escaper::getOptions(int argc, char** argv) {
	int optionIndex = 0, option = 0;

	struct option longOpts[] = {
		{ "help", no_argument, nullptr, 'h' },
		{ "stats", required_argument, nullptr, 's' },
		{ "median", no_argument, nullptr, 'm' },
		{ "verbose", no_argument, nullptr, 'v' },
		{ nullptr, 0, nullptr, '\0' }
	};

	while ((option = getopt_long(argc, argv,
		"hs:mv", longOpts, &optionIndex)) != -1) {
		switch (option) {
			case 'h': {
				helpFlag = true;
				break;
			}
			case 's': {
				statsFlag = true;
				if (*optarg) {
					statsN = std::atoi(optarg);
				}
				break;
			}
			case 'm': {
				medianFlag = true;
				break;
			}
			case 'v': {
				verboseFlag = true;
				break;
			}
			default: {
				break;
			}
		}
		if (helpFlag) break;
	}
	return validateOptionInput();
}

// validates the option input and returns if broken
int Escaper::validateOptionInput() {
	if (helpFlag) {
		std::cout <<
			"------ MINE ESCAPE HELP ------\n"
			"Mine escape is run as follows:\n"
			"./MineEscape [flags]\n"
			"No flags are required; a full \n"
			"list of optional arguments is \n"
			"included below, for reference.\n"
			"\n"
			"--help OR -h:\n"
			"   prints this helpful message\n"
			"--stats N OR -s N:\n"
			"   asks program to print stats\n"
			"   output for N tiles; N req. \n"
			"--median OR -m:\n"
			"   asks program to print the  \n"
			"   median difficulty of rubble\n"
			"   that it has seen cleared.  \n"
			"--verbose OR -v:\n"
			"   tells the program to print \n"
			"   every rubble value as it is\n"
			"   being cleared.\n\n";
		return 0;
	}
	return -1;
}

// reads data from input stream, hopefully you're using input redirection 
// or else type really fast
int Escaper::getData() {
	// temp asz string to read data from 
	std::string s = "";

	std::cin >> s;
	outputType = s[0];

	std::cin >> s; std::cin >> s;
	SIZE = short(std::stoi(s));
	mine.resize(SIZE*SIZE);

	std::cin >> s; std::cin >> s;
	rowStart = short(std::stoi(s));  // init y start coord

	std::cin >> s;
	colStart = short(std::stoi(s));  // init x start coord

	int cnt = validateDataInput();
	if (cnt >= 0)
		return cnt;

	if (outputType == 'M')
		getGridM();
	else if (outputType == 'R')
		getGridR();
	else
		return 1;
	if (statsFlag) {
		if (int(statsN) > int(SIZE*SIZE))
			first.reserve(SIZE*SIZE);
		else
			first.reserve(statsN);
	}

	return -1;
}

// gets the grid in the case of manual input file
void Escaper::getGridM() {
	int cnt = 0;
	int elt = 0;
	while (std::cin >> elt) {
		mine[cnt] = { cnt, short(elt), false, false, SIZE };
		++cnt;
	}
}

// gets the grid in the case of a random input file
void Escaper::getGridR() {
	std::string s = "";
	std::cin >> s; std::cin >> s;
	SEED = std::stoi(s);

	std::cin >> s; std::cin >> s;
	MAX_RUBBLE = std::stoi(s);

	std::cin >> s; std::cin >> s;
	TNT_CHANCE = std::stoi(s);

	std::stringstream ss;
	
	// straaaight from the SPEC
	P2random::PR_init(ss, SIZE, SEED, MAX_RUBBLE, TNT_CHANCE);
	int cnt = 0;
	int elt;
	while (ss >> elt) {
		mine[cnt] = { cnt, short(elt), false, false, SIZE };
		++cnt;
	}
}

// validate data input function results, or return nonzero
int Escaper::validateDataInput() {
	if (outputType != 'M' && outputType != 'R') {
		std::cerr << "Sad! You didn't specifiy M or R in your input.\n"
			"We're all leaving now.\n\n";
		return 1;
	}
	if (!(rowStart < SIZE && colStart < SIZE)) {
		std::cerr << "Everyone is laughing at you. Your input coordinate\n"
			"was not even in your grid.";
		return 1;
	}
	return -1;
}

// prints the mine. helper function, shouldn't really be publicly called but OH WELL 
void Escaper::printMine() {
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			std::cout << std::setw(4) << mine[i*SIZE + j].value << " ";
		}
		std::cout << std::endl;
	}
}

// main mine-escape loop. contains all of the rest of the program, basically
void Escaper::escapeMine() {
	// add the first tile to the pq of seen tiles. should be the only thing on this array
	addToSeen( getIndex(rowStart,colStart) ); 

	// general while loop, bad I know but who CARES
	while (true) {

		// move to the next tile in the priority queue
		moveToTile();

		// clear the tile you just moved u asz to
		clearTile(current.index);

		// set the current tile's used boolean to "true" 
		mine[current.index].used = true;

		// break if we've escaped
		if (escaped()) break; 

		// else, see the new tiles around us and go for another mf loop
		seeTiles();
	}

	// when finished, print all program output and return 
	printOutput(); 
}

// see tiles up, down, left, and right of current tile
void Escaper::seeTiles() {
	addToSeen(current.index - 1);
	addToSeen(current.index - SIZE);
	addToSeen(current.index + SIZE);
	addToSeen(current.index + 1);
}

// if on any borders, return true. otherwise FALSE that bih 
bool Escaper::escaped() {
	if (current.index / SIZE == 0 ||
		current.index / SIZE == SIZE - 1 ||
		current.index % SIZE == 0 ||
		current.index % SIZE == SIZE - 1)
		return true;
	return false;
}

// moves to the next tile on the priority queue 
void Escaper::moveToTile() {
	while (!seenpq.empty()) {
		if (!mine[seenpq.top().index].used) break;
		else seenpq.pop();
	}
	current = seenpq.top();
	seenpq.pop();
}

// specialized removing TNT function called within clearTile function ONLY. 
// handles the case of a current tile (to be cleared) being TNT. 
void Escaper::clearTNT(int index) {
	// add to the TNT pq
	addToTNT(index);

	// while the tntpq isn't empty, 
	while (!tntpq.empty()) {

		// if the value isn't that of an exploded TNT (see addToTNT function), break 
		if (tntpq.top().value != 0) break;

		temp = tntpq.top();
		addToSeen(temp.index);
		tntpq.pop();

		if (verboseFlag) std::cout << "TNT explosion at ["
			<< temp.index / int(SIZE) << "," << temp.index % int(SIZE) << "]!\n";
		if (statsFlag)
			addToStats({temp.index, -1, SIZE});
		if(temp.index % SIZE > 0)
			addToTNT( temp.index - 1 );
		if(temp.index / SIZE > 0)
			addToTNT( temp.index - SIZE );
		if(temp.index / SIZE + 1 < SIZE )
			addToTNT( temp.index + SIZE );
		if(temp.index % SIZE + 1 < SIZE )
			addToTNT( temp.index + 1);
		if (tntpq.empty()) break;
	}
	while (!tntpq.empty()) {
		if (tntpq.top().value == mine[tntpq.top().index].value) {
			if (tntpq.top().value > 0) {
				if (verboseFlag)
					std::cout << "Cleared by TNT: " << tntpq.top().value << " at [" << tntpq.top().index / int(SIZE) << "," << tntpq.top().index % int(SIZE) << "]\n";
				if (medianFlag)
					addToMedian(tntpq.top().value);
				if (statsFlag)
					addToStats(tntpq.top());
				rubbleCount += tntpq.top().value;
				tileCount++;
			}
			mine[tntpq.top().index].value = 0;
			addToSeen(tntpq.top().index);
			tntpq.pop();
		}
		else tntpq.pop();
	}
}

// remove rubble or tnt from current tile 
void Escaper::clearTile(int index) {
	// TNT case
	if (mine[index].used) return;
	if (mine[index].value < 0) {
		clearTNT(index);
	}
	else {
		if (current.value > 0) {
			if (verboseFlag)
				std::cout << "Cleared: " << current.value << " at [" << current.index / int(SIZE) << "," << current.index % int(SIZE) << "]\n";
			if (medianFlag)
				addToMedian(current.value);
			if (statsFlag)
				addToStats(current);
			++tileCount;
			rubbleCount += current.value;
			current.value = 0;
			mine[current.index].value = 0;
		}
	}
}

// add to the SEEN array! 
void Escaper::addToSeen(int index) {
	mine[index].seen = true;
	seenpq.push({ mine[index].index, mine[index].value, SIZE });
}

// add to the TNT array!
void Escaper::addToTNT(int index) {
	if (!mine[index].value == 0) {
		if (mine[index].value == -1)
			mine[index].value = 0;
		tntpq.push({index, mine[index].value, SIZE });
	}
	else if (!mine[index].seen) {
		addToSeen(index);
	}
}

// add to median output arrs
void Escaper::addToMedian(short value) {

	// diff in left and right sizes
	val = int(left.size()) - int(right.size());

	// if the difference is zero, then 
	if (!val) {

		// either both arrays are empty, in which case we push the value (arbitrarily) to the left
		// and update the median accordingly
		if (left.empty()) {
			left.push(value);
			med = left.top();
		}

		// or, both arrays are the same size and there already exists a median. In this case we should
		// check if the value is less than the furrent median, in which case we'd put the value on the left array and set the median to the top of the left array. 
		// otherwise, we'd put on the right and continue with the same. 
		else {
			if (value < med) {
				left.push(value);
				med = left.top();
			}
			else {
				right.push(value);
				med = right.top();
			}
		}
	}
	// otherwise neither array is empty, nice case. 
	else {
		
		// if the difference in sizes is positive, then the left is one larger
		if (val > 0) {
			// if the value should go on the left, we have to push the top left into the right and then add the value to the left. 
			if (value < med) {
				right.push(left.top());
				left.pop();
				left.push(value);
			}
			// otherwise we can push the value to the right as it should go. 
			else {
				right.push(value);
			}
		}
		// otherwise, opposite case: right side is larger 
		else {

			// see above but with reversed cases, no need to reexplain
			if (value > med) {
				left.push(right.top());
				right.pop();
				right.push(value);
			}
			else {
				left.push(value);
			}
		}

		// in both cases, the median becomes the average of the two tops on either side. Nice! 
		med = (double(left.top()) + double(right.top())) / double(2);
	}
	std::cout << "Median difficulty of clearing rubble is: " << med << "\n";
}

// basic logic to fill stats arrays under any circumstancess
void Escaper::addToStats(pqEntry pqe) {
	if(int(first.size()) < statsN) {
		first.push_back(pqe);
	}
	if(int(last.size()) == statsN) last.pop_back();
	last.push_front(pqe);
	easiest.push(pqe);
	if (int(easiest.size()) > statsN) easiest.pop();
	hardest.push(pqe);
	if (int(hardest.size()) > statsN) hardest.pop();
}

// prints the statistics in SPEC format
void Escaper::printStats() {
	std::cout << "First tiles cleared:\n";
	for (auto pqe : first) {
		if (pqe.value == -1)
			std::cout << "TNT";
		else
			std::cout << pqe.value;
		std::cout << " at [" << pqe.index / SIZE << "," << pqe.index % SIZE << "]\n";
	}
	std::cout << "Last tiles cleared:\n";
	for (auto pqe : last) {
		if (pqe.value == -1)
			std::cout << "TNT";
		else
			std::cout << pqe.value;
		std::cout << " at [" << pqe.index / SIZE << "," << pqe.index % SIZE << "]\n";
	}
	std::deque<pqEntry> easiestv;
	std::deque<pqEntry> hardestv;
	while (!easiest.empty() && !hardest.empty()) {
		easiestv.push_front(easiest.top());
		hardestv.push_front(hardest.top());
		easiest.pop();
		hardest.pop();
	}

	std::cout << "Easiest tiles cleared:\n";
	for (auto pqe : easiestv) {
		if (pqe.value == -1)
			std::cout << "TNT";
		else
			std::cout << pqe.value;
		std::cout << " at [" << pqe.index / SIZE << "," << pqe.index % SIZE << "]\n";
	}
	std::cout << "Hardest tiles cleared:\n";
	for (auto pqe : hardestv) {
		if (pqe.value == -1)
			std::cout << "TNT";
		else
			std::cout << pqe.value;
		std::cout << " at [" << pqe.index / SIZE << "," << pqe.index % SIZE << "]\n";
	}
}

// prints end-of-program outputs 
void Escaper::printOutput() {
	std::cout << "Cleared " << tileCount << " tiles containing " << rubbleCount << " rubble and escaped.\n";
	if (statsFlag)
		printStats();
}