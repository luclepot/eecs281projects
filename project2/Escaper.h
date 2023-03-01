#include <getopt.h>
#include <queue>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "P2random.h"
#include <iomanip>

class Escaper {
public:

	int getOptions(int argc, char** argv);

	int getData();

	void printMine();

	void escapeMine(); 

	void printOutput(); 

private:
	
	struct tile {
		int index; 
		short value;
		bool used;
		bool seen; 
		short size; 
	};

	struct pqEntry {
		int index;
		short value;
		short size; 
	};


	int validateOptionInput(); 

	int validateDataInput(); 

	void getGridM();

	void getGridR(); 

	bool helpFlag = false; 
	bool statsFlag = false; 
	bool medianFlag = false; 
	bool verboseFlag = false; 
	int statsN = -1;

	char outputType = '\0';
	short SIZE;
	short rowStart = -1;
	short colStart = -1;
	int SEED = -1;
	int MAX_RUBBLE = -1;
	int TNT_CHANCE = -1;

	short getRow(int index) {
		return short(index / SIZE); 
	}

	short getCol(int index) {
		return short(index % SIZE); 
	}

	int getIndex(short row, short col) {
		return int(row) * int(SIZE) + int(col); 
	}

	struct pqComp {
		bool operator()(pqEntry & a, pqEntry & b) {
			if (b.value < a.value)
				return true;
			if (a.value < b.value)
				return false;
			if (b.index % b.size < a.index % a.size)
				return true;
			if (a.index % a.size < b.index % b.size)
				return false;
			if (b.index / b.size < a.index / a.size)
				return true;
			return false;
		}
	};

	struct antiPqComp {
		bool operator()(pqEntry & a, pqEntry & b) {
			if (b.value < a.value)
				return false;
			if (a.value < b.value)
				return true;
			if (b.index % b.size < a.index % a.size)
				return false;
			if (a.index % a.size < b.index % b.size)
				return true;
			if (b.index / b.size < a.index / a.size)
				return false;
			return true;
		}
	};

	std::vector<tile> mine; 
	std::priority_queue<pqEntry, std::vector<pqEntry>, pqComp> seenpq;
	std::priority_queue<pqEntry, std::vector<pqEntry>, pqComp> tntpq;
	std::priority_queue<pqEntry, std::vector<pqEntry>, pqComp> hardest;
	std::priority_queue<pqEntry, std::vector<pqEntry>, antiPqComp> easiest;

	struct medianLeft {
		bool operator()(short a, short b) {
			return a < b; 
		}
	};

	struct medianRight {
		bool operator()(short a, short b) {
			return b < a;
		}
	};

	std::priority_queue<short, std::vector<short>, medianLeft> left;
	std::priority_queue<short, std::vector<short>, medianRight> right;


	std::vector<pqEntry> first;
	std::deque<pqEntry> last;

	pqEntry current; 
	pqEntry temp;

	double med = 0; 

	int val = 0; 

	int tileCount = 0;
	long int rubbleCount = 0; 

	void addToSeen(int index);

	void addToTNT(int index); 

	bool escaped();
	
	void moveToTile(); 

	void seeTiles();

	void clearTile(int index);

	void clearTNT(int index);

	void addToMedian(short value); 

	void addToStats(pqEntry pqe);

	void printStats(); 
};
