#include "sillyDB.h"

int sillyDB::processArgs(int argc, char** argv){
	string temp; 
	for (int i = 1; i < argc; ++i) {
		temp = argv[i]; 
		if (temp == "-h" || temp == "--help") {
			std::cout << "helpful message here!!\n"; 
			return 0;
		}
		else if (temp == "-q" || temp == "--quiet") {
			quiet = true; 
		}
	}
	return -1;
}

int sillyDB::mainLoop() {
	do {
		temp = "";
		cout << "% "; 
		cin >> temp;
		currentCMD = cmdType(temp[0]);
		executeCMD();
	}
	while (currentCMD != cmdType::QUIT);
	return 0; 
}

void sillyDB::executeCMD() {
	switch (currentCMD) {
		case cmdType::CREATE: {
			create(); 
			break;
		}
		case cmdType::REMOVE: {
			remove(); 
			break; 
		}
		case cmdType::INSERT: {
			insert(); 
			break;
		}
		case cmdType::JOIN: {
			join(); 
			break; 
		}
		case cmdType::PRINT: {
			print(); 
			break;
		}
		case cmdType::DELETE: {
			del(); 
			break;
		}
		case cmdType::GENERATE: {
			generate(); 
			break;
		}
		case cmdType::COMMENT: {
			std::getline(cin, trash);
			break; 
		}
		case cmdType::QUIT: {
			cout << "Thanks for being silly!\n"; 
			std::getline(cin, trash); 
			break; 
		}
		default: {
			cout << "Error: unrecognized command\n";
			std::getline(cin, trash);
			break;
		}
	}
}

void sillyDB::create() {
	int size;
	string name; 
	cin >> name;

	if (tables.find(name) != tables.end()) {
		Table::printError(1, name);
		std::getline(cin, trash);
		return;
	}

	cin >> size; 
	vector<EntryType> ets(size);
	vector<string> cols(size);
	for (int i = 0; i < size; ++i) {
		cin >> temp; 
		ets[i] = (getType(temp[0])); 
	}
	for (int i = 0; i < size; ++i) {
		cin >> temp;
		cols[i] = (temp); 
	}
	tables[name] = Table(size, cols.begin(), cols.end(), ets.begin(), ets.end(), name, quiet); 
	cout << "New table " << name << " with column(s) "; 
	for (auto str : cols) cout << str << " "; 
	cout << "created\n";
}

void sillyDB::remove() {
	string name; 
	cin >> name;
	if (tables.find(name) == tables.end()) {
		Table::printError(2, name); 
		std::getline(cin, trash);
		return;
	}
	tables.erase(name); 
	cout << "Table " << name << " deleted\n"; 
	std::getline(cin, trash); 
}

EntryType sillyDB::getType(char c) {
	switch (c) {
	case 's':
		return EntryType::String; 
	case 'b':
		return EntryType::Bool;
	case 'd':
		return EntryType::Double;
	case 'i':
		return EntryType::Int;
	}
	return EntryType::Bool; 
}

void sillyDB::insert() { 
	string name; 
	int size; 
	string t; 
	cin >> trash; 
	cin >> name; 
	cin >> size; 
	cin >> trash; 
	auto it = tables.find(name); 
	if (it == tables.end()) {
		Table::printError(2, name); 
		std::getline(cin, trash); 
		return;
	}
	int before = tables[name].rows; 
	tables[name].insertRows(size);
	tables[name].updateIndicies(); 

	cout << "Added " << size << " rows to " << name << " from position "
		<< before << " to " << tables[name].rows - 1 << "\n";
}	

void sillyDB::join() {
	string name1, name2;
	cin >> name1; 
	cin >> trash; 
	cin >> name2;
	if (tables.find(name1) == tables.end()) {
		Table::printError(2, name1); 
		std::getline(cin, trash); 
		return; 
	}
	else if (tables.find(name2) == tables.end()) {
		Table::printError(2, name2); 
		std::getline(cin, trash); 
		return; 
	}

	Table::join(tables[name1], tables[name2]); 
}

void sillyDB::print() {
	string name;
	int numRows;
	cin >> trash; 
	cin >> name;
	cin >> numRows;
	vector<string> toPrint(numRows);
	if (tables.find(name) == tables.end()) {
		Table::printError(2, name); 
		std::getline(cin, trash);
		return;
	}
	for (int i = 0; i < numRows; ++i) {
		cin >> temp; 
		toPrint[i] = temp;
	}
	cin >> temp;

	if (temp[0] == 'A')
		tables[name].printAll(toPrint.begin(), toPrint.end());
	else {
		tables[name].printWhere(toPrint.begin(), toPrint.end());
	}
}

void sillyDB::del() {
	string name; 
	cin >> trash; 
	cin >> name; 
	cin >> trash; 
	if (tables.find(name) == tables.end()) {
		Table::printError(2, name); 
		std::getline(cin, trash);
		return;
	}
	tables[name].del(); 
	std::getline(cin, trash); 
}

void sillyDB::generate() {
	string name;
	cin >> trash;
	cin >> name;
	if (tables.find(name) == tables.end()) {
		Table::printError(2, name); 
		std::getline(cin, trash);
		return;
	}
	tables[name].generate();
	std::getline(cin, trash);
}