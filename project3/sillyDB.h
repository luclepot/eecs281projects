#include <iostream>
#include "Table.h"
#include "TableEntry.h"

class sillyDB {
public: 
	// literally just processes and checks 
	// the command line arguments. pretty simple function
	// doesn't even use that mf get_optarg function
	int processArgs(int argc, char** argv);

	int mainLoop();

private:

	enum class cmdType {
		QUIT = 'Q',
		CREATE = 'C',
		INSERT = 'I',
		DELETE = 'D',
		PRINT = 'P',
		GENERATE = 'G',
		COMMENT = '#', 
		REMOVE = 'R',
		JOIN = 'J'
	};

	void executeCMD();

	void create(); 
	void remove(); 
	void insert(); 
	void join(); 
	void print(); 
	void del();
	void generate(); 

	EntryType getType(char c); 
	// quiet output flag
	bool quiet = false;

	string temp, trash;

	cmdType currentCMD; 

	unordered_map<string, Table> tables; 

	//std::unordered_map<entryType, std::vector<TableEntry>> m;
	//m[entryType::STRING].push_back(TableEntry("chevrolet"));


	//std::unordered_map<std::string, std::pair<entryType, std::vector<TableEntry>>> map;
	//map["vendor"].first = entryType::STRING;
	//map["vendor"].second.push_back(TableEntry("chevrolet"));
};
