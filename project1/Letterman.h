#include <getopt.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <deque>
#pragma once

class Letterman {
public:
	///////////////
	// FUNCTIONS //9--p
	///////////////

	int getOptions(int argc, char** argv); // parses command line arguments

	void pathFind();

	int getData();

private:

	struct changeEntry {
		int parentIndex;
		int wordIndex;
		int changeIndex;
		char changeType;
	};

	///////////////
	// FUNCTIONS //
	///////////////

	void readData(std::string* strp, bool length); // reads in the dictionary type and the number of words, then calls the correct
				 // reading function (simple or complex) on each word in the file.

	bool contStartEnd(const std::string &s, const std::string &e); // determines whether s and e are in the dictionary

	void simpleRead(std::string &s, std::string * strp, bool length); // reads in the case of a simple dictionary; accepts every word in turn

	void complexRead(std::string &s, std::string * strp, bool length); // reads in the case of a complex dictionary; breaks down words into sub words

	bool specialCharCheck(std::string &s, std::string &c, size_t &special); // helper function for complexRead. Checks for a special character in a string 
																			// and if found, updates location accordingly.

	void exitMessage(std::string msg, int exitStatus); 	// prints to cerr and returns exitStatus

	int validateOptionInput(); 	// validates program cmd line option input

	int validateDataInput(); // validates dictionary data input

	int findWord(const std::string & word);

	void findSimilar();

	bool lengthMatch();

	bool changeMatch();

	bool swapMatch();

	void updateChangeVariables(char _changeType, int _changeIndex, bool _foundChange );

	void resetChangeVariables(); 

	void addToChangeVec(); 

	changeEntry* findInChangeVec(int indexToFind); 

	void printOutput(); 

	void setUnsetUsed(std::string & str, bool used);

	bool isUsed(std::string & str); 

	//void addToOutput(bool isLast); 

	////////////////
	// CLASS DATA //
	////////////////	

	char dictType = '\0'; // dictionary type char variable; will either be 'S' or 'C'

	int N = 0; // integer number of words in the dictionary. should always be true
			   // that N == d.size()

	std::vector< std::string > d; // vector of strings representing the dictionary itself. end goal of initilization, 
							// and key component of the dictionary class itself. 

	std::vector<std::string> specialCharacters = // special character vector; only really helpful for the while loop in complexRead.
	{ "&", "[", "!", "?", "" };					 // Essentially acts as a 'helper data structure' for this function.  

	int dSize = 0; 

	std::deque< int > deq; // both the stack and the queue for this program

	// routing scheme id
	bool useStack = false;
	bool useQueue = false; 
	bool printHelp = false; 

	// output format id 
	char outputFormat = '\0';

	// variables for change, swap, and length
	bool change = false,
		swap = false,
		length = false;

	int changeParent; 


	int current, end, index, start;
	int checked = 0;
	int lCurrent, lIndex;
	std::string * wCurrent, * wIndex;
	

	std::vector<changeEntry> changesVec;
	
	// changeEntry toAdd; 

	bool foundChange = false; 
	int wordIndex; 
	int wordChangeIndex = -1; 
	int changeIndex = -1; 
	char changeChar = '\0';
	char changeType = '\0';

	// word strings for begin and end
	std::string wBegin = "", wEnd = "";
};
